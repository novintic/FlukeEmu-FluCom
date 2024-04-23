/***************************************************************************
This work is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License Version 2 as published by the
Free Software Foundation. This work is distributed in the hope that it will
be useful, but without any warranty; without even the implied warranty of
merchantability or fitness for a particular purpose.
See the GNU General Public License for more details. You should have
received a copy of the GNU General Public License along with this program;
if not, write to:
Free Software Foundation,
Inc., 59 Temple Place, Suite 330,
Boston, MA 02111-1307 USA
****************************************************************************/
// By Clemens (novintic), 2022

// FluCom3 Probe board communication
// Runs an extra thread to communicate with the probe board
// over SPI.

#include "ProbeBoard.h"
#include "wx/wx.h"

#ifdef LINUX_SPI
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#endif // LINUX_SPI
#ifdef WIRINGPI
#include "wiringPi.h"
#endif // WIRINGPI

ProbeBoard::ProbeBoard() : wxThread(wxTHREAD_JOINABLE)
{
    m_reset = true;
    m_run   = true;
    m_exit  = false;
    m_execIntUs = 1000; // run once per ms
    hSPIDev = 0;
    // init SPI messages
    memset(&m_outMsg, 0, sizeof(PROBE_SPIOUT_REGS_t));
    m_outMsg.header  = 0x5A;   // #define SPI_INMSG_HEAD      0xA5
    m_outMsg.trailer = 0xA5;   // #define SPI_INMSG_TAIL      0x5A

    memset(&m_inMsg, 0, sizeof(PROBE_SPIIN_REGS_t));
    m_inMsg.header   = 0xA5;   // #define SPI_INMSG_HEAD     0xA5
//    m_inMsg.pulseCfg = 0;
//    m_inMsg.rstCmd   = 0;
    m_inMsg.res[0]   = 0x11;
    m_inMsg.res[1]   = 0x22;
    m_inMsg.res[2]   = 0x33;
    m_inMsg.trailer  = 0x5A;   // #define SPI_INMSG_TAIL     0x5A

    m_pPodProbe      = NULL;
    m_pEvSigGen      = NULL;
}

ProbeBoard::~ProbeBoard()
{
}

// The thread execution function
wxThread::ExitCode ProbeBoard::Entry()
{
    int64_t lastT = 0;
    wxLogDebug("PBOARD: COMM started");
    // loop
    while(!m_exit)
    {
        //wxLongLong wxStT = wxGetLocalTimeMillis();
        wxLongLong wxStT = wxGetUTCTimeUSec();
        int64_t stT = wxStT.GetValue();

        if(m_reset)
        {
            reset();
            m_reset = false;
        }
        if(m_run)
        {
            probeBoardComm();
        }
        //wxLongLong wxEnT = wxGetLocalTimeMillis();
        wxLongLong wxEnT = wxGetUTCTimeUSec();
        int64_t enT = wxEnT.GetValue();
        int64_t tdiff = enT - stT;
        // update stats if emu has run
        if(m_run)
        {
            m_totEmuTime += tdiff;
            m_cntEmuRuns++;
            m_avgEmuTime = m_totEmuTime / m_cntEmuRuns;
            m_maxEmuTime = tdiff > m_maxEmuTime ? tdiff : m_maxEmuTime;
        }

        if((enT - lastT) >= 1000000)
        {
            lastT = enT;
            //dispStats();
        }

        int slt = (m_execIntUs - tdiff) >= 0 ? m_execIntUs - tdiff : 0;

        // Sleep
        //wxMicroSleep(slt);
        wxMilliSleep(10);
    }

    return 0;
}

// Start, pause, reset, end emulation thread
void ProbeBoard::execCtrl(int cmd)
{
    if(cmd == ECMD_RESET)
        m_reset = true;
    else if(cmd == ECMD_RUN)
        m_run = true;
    else if(cmd == ECMD_PAUSE)
        m_run = false;
    else if(cmd == ECMD_EXIT)
        m_exit = true;
}


void ProbeBoard::reset(void)
{
    init();

}

void ProbeBoard::init(void)
{
    // open SPI
	hSPIDev = openSPI("/dev/spidev0.0", 1000000);
#ifdef WIRINGPI
    pinMode (GPIO_SPI_CS, OUTPUT);
    digitalWrite(GPIO_SPI_CS, HIGH);
#endif // WIRINGPI

	// open ports

}

void ProbeBoard::regPodProbe(emuPodProbe *pPodProbe)
{
    m_pPodProbe = pPodProbe;
}

void ProbeBoard::regEvSigGen(emuEvCntSigGen *pEvSigGen)
{
    m_pEvSigGen = pEvSigGen;
}

uint32_t ProbeBoard::getProbeStatus(void)
{
    return m_outMsg.levStat;
}

void ProbeBoard::setPulseMode(uint32_t pulseCfg)
{
    m_inMsg.pulseCfg = pulseCfg;
}

uint32_t ProbeBoard::getCount(void)
{
    return m_outMsg.evCount;
}

void ProbeBoard::resetSigCnt(void)
{
    m_inMsg.rstCmd |= SPI_INMSG_CMDRST;
    // clear local values
    m_outMsg.signature = 0;
    m_outMsg.evCount = 0;
}

// Get latest data from emulation for sending to probe board
void ProbeBoard::getData(uint8_t *pBuf, uint32_t maxSize)
{
    uint8_t pia567 = 0;
    uint8_t rstCnt = 0;
    uint8_t pm = 0;
    if(m_pPodProbe != NULL)
        pia567 = m_pPodProbe->getPulesMode();
    // Hi pulse
    pm |= pia567 & (1 << 5) ? SPI_INMSG_PULSEHI : 0;
    // LO pulse
    pm |= pia567 & (1 << 6) ? SPI_INMSG_PULSELO : 0;
    // Snyc mode
    pm |= pia567 & (1 << 7) ? 0 : SPI_INMSG_SYNC; // Meaning of this bit is inverted
    m_inMsg.pulseCfg = pm;
    if(m_pEvSigGen != NULL)
        rstCnt = m_pEvSigGen->getRstCmd();
    m_inMsg.rstCmd = rstCnt ? SPI_INMSG_CMDRST : 0;

    memcpy(pBuf, &m_inMsg, maxSize);
    //wxLogDebug("SPI TX %02x %02x %02x %02x %02x %02x %02x\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6]);
}

// new data
void ProbeBoard::setData(uint8_t *pBuf, uint32_t maxSize)
{
    // Copy message
    memcpy(&m_outMsg, pBuf, sizeof(PROBE_SPIIN_REGS_t));
    //wxLogDebug("SPI RX %02x %02x %02x %02x %02x %02x %02x\n",bufRx[0],bufRx[1],bufRx[2],bufRx[3],bufRx[4],bufRx[5],bufRx[6]);
    // check framing
    if((m_outMsg.header == SPI_OUTMSG_HEAD) && (m_outMsg.trailer == SPI_OUTMSG_TAIL))
    {
        uint8_t ls = m_outMsg.levStat;
        uint8_t pia567 = 0;
        pia567 |= ls & SPI_OUTMSG_LEVIN ? (1 << 5) : 0;  // Inalid
        pia567 |= ls & SPI_OUTMSG_LEVLO ? (1 << 6) : 0;  // GREEN/BOTTOM LED = LO
        pia567 |= ls & SPI_OUTMSG_LEVHI ? (1 << 7) : 0;  // RED/TOP LED = Hi
        if(m_pPodProbe != NULL)
        {
            m_pPodProbe->setLevels(pia567);
            m_pPodProbe->setProbeLEDStates(ls & SPI_OUTMSG_LEDHI, ls & SPI_OUTMSG_LEDLO);
        }
        if(m_pEvSigGen != NULL)
        {
            m_pEvSigGen->setEvCnt(m_outMsg.evCount);
            m_pEvSigGen->setSignature(m_outMsg.signature);
        }
    }
}

// Send new data and receive new data
void ProbeBoard::probeBoardComm(void)
{
    uint8_t buf[sizeof(PROBE_SPIIN_REGS_t)];
    uint8_t bufRx[sizeof(PROBE_SPIOUT_REGS_t)];
    getData(buf, sizeof(PROBE_SPIIN_REGS_t));
#ifdef WIRINGPI
    digitalWrite(GPIO_SPI_CS, LOW);
#endif // WIRINGPI
    wxMicroSleep(500); // wait 0.5ms for SPI slave to prepare data
#ifdef LINUX_SPI
//  int res = write(hSPIDev, buf, sizeof(PROBE_SPIIN_REGS_t));
//	tcdrain(hSPIDev);

	struct spi_ioc_transfer tr;
	memset(&tr, 0, sizeof(tr));
	tr.tx_buf = (unsigned long)buf;
	tr.rx_buf = (unsigned long)bufRx;
	tr.len = sizeof(PROBE_SPIIN_REGS_t);
	tr.speed_hz = 1000000;
	tr.bits_per_word = 8;
	//tr.delay_usecs = 0;
	//tr.cs_change = 1;
	int ret = ioctl(hSPIDev, SPI_IOC_MESSAGE(1), &tr);

#endif
#ifdef WIRINGPI
    digitalWrite(GPIO_SPI_CS, HIGH);
#endif // WIRINGPI
    // process received message
    setData(bufRx, sizeof(PROBE_SPIOUT_REGS_t));
    // reset coammands after sending
    m_inMsg.rstCmd = 0;
}

int ProbeBoard::openSPI(const char* devName, int speedHz)
{
    int hSPIDev = -1;
	wxLogDebug("opening %s\n", devName);
#ifdef LINUX_SPI
	hSPIDev = open(devName, O_RDWR | O_NOCTTY);
	if (hSPIDev == -1)
		wxLogDebug("%s: open failed\n", devName);
	// set SPI speed
	int ret = ioctl(hSPIDev, SPI_IOC_WR_MAX_SPEED_HZ, &speedHz);
	if (ret == -1)
		wxLogDebug("%s: can't set speed\n", devName);
	// set SPI mode
	int spi_mode = SPI_MODE_3 | SPI_NO_CS;

    ret = ioctl(hSPIDev, SPI_IOC_RD_MODE, &spi_mode);
	wxLogDebug("%s: SPIMode %d\n", devName, spi_mode);
	spi_mode = SPI_MODE_3 | SPI_NO_CS;

	ret = ioctl(hSPIDev, SPI_IOC_WR_MODE, &spi_mode);
    if(ret < 0)
		wxLogDebug("%s: can't set SPIMode\n", devName);

	// SPI bits per word
	int spi_bitsPerWord;
    ret = ioctl(hSPIDev, SPI_IOC_RD_BITS_PER_WORD, &spi_bitsPerWord);
    wxLogDebug("%s: Bits per word %d\n", devName, spi_bitsPerWord);
	spi_bitsPerWord = 8;
    ret = ioctl(hSPIDev, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord);
    if(ret < 0)
		wxLogDebug("Could not set SPI bitsPerWord (WR)...ioctl fail");
 #endif
	return hSPIDev;
}
