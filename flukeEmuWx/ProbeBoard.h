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

#ifndef PROBEBOARD_H
#define PROBEBOARD_H

#include "stdint.h"
#include "wx/wx.h"

#include "EmuPodProbe.h"
#include "EmuEvCntSigGen.h"

#define LINUX_SPI

#define ECMD_RESET   0
#define ECMD_RUN     1
#define ECMD_PAUSE   2
#define ECMD_EXIT    3

#define GPIO_SPI_CS  3      // Wiring PI numbering BCM:GPIO22

// SPI out register for comm with emulator
typedef struct
{
    uint8_t    header;      // 0xA5
    uint8_t    evCount;     // Event counter 7bits
    uint16_t   signature;   // Signature LFSR
    uint8_t    levStat;     // Level status: HI,LO,TRI
    uint8_t    version;     // Board FW version
    uint8_t    trailer;     // 0x5A
}PROBE_SPIOUT_REGS_t;

#define SPI_OUTMSG_HEAD     0x5A
#define SPI_OUTMSG_TAIL     0xA5

#define SPI_OUTMSG_LEVHI   0x01
#define SPI_OUTMSG_LEVLO   0x02
#define SPI_OUTMSG_LEVIN   0x04

// SPI in register for comm with emulator
typedef struct
{
    uint8_t    header;      // 0x5A
    uint8_t    pulseCfg;    // Pulse configuration, hi, lo, sync
    uint8_t    rstCmd;      // reset cmd for evCnt + signature (bit 0x1)
    uint8_t    res[3];      // reserved
    uint8_t    trailer;     // 0x5A
}PROBE_SPIIN_REGS_t;

#define SPI_INMSG_HEAD      0xA5
#define SPI_INMSG_TAIL      0x5A

#define SPI_INMSG_CMDRST    0x01
#define SPI_INMSG_PULSEHI   0x01
#define SPI_INMSG_PULSELO   0x02
#define SPI_INMSG_SYNC      0x04

class ProbeBoard : public wxThread
{
 public:
    ProbeBoard();
    ~ProbeBoard();
    wxThread::ExitCode Entry();

    void execCtrl(int cmd);

    void init(void);
    void reset(void);
    void probeBoardComm(void);

    void getData(uint8_t *pBuf, uint32_t maxSize);
    void setData(uint8_t *pBuf, uint32_t maxSize);

    uint32_t getProbeStatus(void);
    void     setPulseMode(uint32_t pulseCfg);
    uint32_t getCount(void);
    uint32_t getSignature(void);
    void     resetSigCnt(void);

    int openSPI(const char* devName, int speedHz);
    void regPodProbe(emuPodProbe *pPodProbe);
    void regEvSigGen(emuEvCntSigGen *pEvSigGen);

 protected:
    // execution control
    bool    m_run;
    bool    m_reset;
    bool    m_exit;
    int     m_execIntUs;
//    int     m_emuCycles;
    // execution statistics
    float   m_totEmuTime;
    int     m_cntEmuRuns;
    int     m_cntEmuRunsLast;
    float   m_avgEmuTime;
    float   m_maxEmuTime;

    int hSPIDev;	// SPI device handle

    PROBE_SPIOUT_REGS_t m_outMsg;  // OUT is in on RPI
    PROBE_SPIIN_REGS_t  m_inMsg; // IN is out on RPI

    emuPodProbe     *m_pPodProbe;
    emuEvCntSigGen  *m_pEvSigGen;
};

#endif // PROBEBOARD_H
