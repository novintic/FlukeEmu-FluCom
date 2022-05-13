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

// Fluke serial port emulation emulation
// The emulation uses a real serial port on the
// host device

#include "EmuSerPort.h"


emuSerPort::emuSerPort(void)
{
    m_portName  = "/dev/tty0";
    m_baudRate  = 9600;
    m_portOpen  = false;
    m_fhPort    = 0;

    m_inFileName  = "";
    m_outFileName = "";
    m_fhInFile    = NULL;
    m_fhOutFile   = NULL;
    m_inFileOpen  = false;
    m_outFileOpen = false;
    m_serMode     = SER_MODE_PORT;
    m_serStat     = 0;

    Reset();
}

void emuSerPort::Reset(void)
{
    // Fluke will report no aux if port not open
    //m_status = 0x00 | (m_portOpen ? SER_TX_EMPTY : 0);

    m_status = 0x00 | SER_TX_EMPTY;
}

void emuSerPort::Write(uint16_t addr, uint8_t data)
{
    if (addr == 0x00E3)
    {   // Send data
        wxLogDebug("SERPORT: TX %02x : %s", data, m_serMode == SER_MODE_PORT ? "PORT":"FILE");
        int ok;
        if(m_serMode == SER_MODE_PORT)
            ok = writeSerPort(&data, 1); // Write to port
        else
            ok = writeOutFile(&data, 1);// Write to file
        m_status |= SER_TX_EMPTY; // indicate byte sent
    }
    else
    {
        wxLogDebug("SERPORT: Unknown write addr %04x val %d", addr, data);
    }
}


uint8_t emuSerPort::Read(uint16_t addr)
{
    uint8_t val = 0;
    addr &= 0x00FF;

    if(addr == 0x00E1)
    {   // read received data
        // is there still data in the buiffer byte?
        if(m_status & SER_DATA_RX)
        {
            val = m_rxBuf[0];
            m_status &= ~SER_DATA_RX;
            wxLogDebug("SERPORT: RX %02x ACK", val);
        }
    }
    else if (addr == 0x00E2)
    {   // read status
        val = m_status | SER_TX_EMPTY;
        // Read the real port
        if((m_status & SER_DATA_RX) == 0)
        {
            uint8_t b;
            int n;
            if(m_serMode == SER_MODE_PORT)
                n = readSerPort(&b, 1);
            else
                n = readInFile(&b, 1);
            if(n == 1)
            {
                m_rxBuf[0] = b;
                m_status |= SER_DATA_RX;
                wxLogDebug("SERPORT: RX %02x : %s", b, m_serMode == SER_MODE_PORT ? "PORT":"FILE");
            }
            else if(m_serMode == SER_MODE_FILE)
            {   // File sent -> close
                if(m_inFileOpen)
                {
                    wxLogDebug("SERPORT: File sent!");
                    fclose(m_fhInFile);
                    m_serStat &= ~SER_INFILE_OPEN;
                    m_serStat |= SER_UPD_FLAG;
                    m_inFileOpen = false;
                }
            }
        }
    }
    else
    {
        wxLogDebug("SERPORT: Unknown read addr %04x", addr);
    }

    return val;
}

// ------------

int emuSerPort::setSerPort(wxString pname, int speed)
{
    int ret = 0;
    // FLUKE 9010 supports baudrates 110, 150, 300, 600, 1200, 2400, 4800, 9600
    // 1 or 2 stop bits, parity/no paroty
    // Only 8N1 is supported here

    // if port already open
    if(m_portOpen)
    {   // close
        closeSerPort();
        m_serStat &= ~SER_PORT_OPEN;
        m_serStat |= SER_UPD_FLAG;
        m_portOpen = false;
    }

    if(pname != "")
    {
        if(openSerPort(pname, speed) > 0)
        {
            m_portName  = pname;
            m_baudRate  = speed;
            m_portOpen = true;
            m_serStat |= SER_PORT_OPEN | SER_UPD_FLAG;
            setMode(SER_MODE_PORT);
            ret = 1;
        }
    }
    else
        ret = 1;    // signal ok, since no port was to be opened

    return ret;
}

#ifdef LINUX_SERPORT
// Prototypes
int set_interface_attribs (int fd, int speed, int parity);
void set_blocking(int fd, int should_block);
#endif // LINUX_SERPORT

int emuSerPort::openSerPort(wxString pname, int speed)
{
    int ret = 0;
#ifdef LINUX_SERPORT
	m_fhPort = open (pname, O_RDWR | O_NOCTTY | O_SYNC);
	if (m_fhPort < 0)
		wxLogDebug("SERPORT: error %d opening %s %s\n", errno, pname, strerror (errno));
	else
	{
	    int ok = set_interface_attribs (m_fhPort, speed, 0);
	    set_blocking (m_fhPort, 0);   // set no blocking
        ret = ok == 0 ? 1 : 0;
    }
#endif // LINUX_SERPORT
    return ret;
}

int emuSerPort::readSerPort(uint8_t* pdata, int maxNum)
{
    int ret = -1;
    if(m_portOpen)
    {
#ifdef LINUX_SERPORT
        int n = read(m_fhPort, pdata, maxNum);  // read up to buffer size bytes
        if(n < 0)
            wxLogDebug("SERPORT: Cannot read");
        else
            ret = n;
#endif
    }
    return ret;
}

int emuSerPort::writeSerPort(uint8_t* pdata, int num)
{
    int ret = -1;
    if(m_portOpen)
    {
#ifdef LINUX_SERPORT
        int n = write(m_fhPort, pdata, num);  // send data
        if(n == num)
            ret = 1;
#endif // LINUX_SERPORT
    }
    return ret;
}

int emuSerPort::closeSerPort(void)
{
#ifdef LINUX_SERPORT
    close(m_fhPort);
#endif // LINUX_SERPORT
    wxLogDebug("SERPORT: Port %s closed\n", m_portName);
    return 0;
}

// -- serial port stuff for linux
#ifdef LINUX_SERPORT
#include <termios.h>
//#include <unistd.h>
#include <sys/ioctl.h>

int set_interface_attribs (int fd, int speed, int parity)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0)
    {
            fprintf(stderr, "error %d from tcgetattr\n", errno);
            return -1;
    }
    // try to map to baudrate defines
    // add init_uart_clock=16000000 to your  /boot/config.txt to if baud rates above 115200 do not work
    int brdef = -1;
    if(speed == 2400) brdef = B2400;
    if(speed == 4800) brdef = B4800;
    if(speed == 9600) brdef = B9600;
    if(speed == 19200) brdef = B19200;
    if(speed == 38000) brdef = B38400;
    if(speed == 57600) brdef = B57600;
    if(speed == 115200) brdef = B115200;
    if(speed == 230400) brdef = B230400;
    if(speed == 460800) brdef = B460800;
    if(speed == 921600) brdef = B921600;
    if(speed == 500000) brdef = B500000;
    if(speed == 1000000) brdef = B1000000;
    if(brdef) speed = brdef;
    cfsetospeed (&tty, speed);
    cfsetispeed (&tty, speed);
    //fprintf(stdout, "Setting baud rate %d == %d\n", speed, brdef);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
                                    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 1;            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    tty.c_iflag &= ~(IGNCR | INLCR | ICRNL | ONLCR); // shut off all NL/CR processing

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
    {
            fprintf(stdout, "error %d from tcsetattr\n", errno);
            return -1;
    }
    return 0;
}

void set_blocking(int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                fprintf(stdout, "error %d from tggetattr\n", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 5 : 0;
        tty.c_cc[VTIME] = 0;            // 0*100ms seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                  fprintf(stdout, "error %d setting term attributes\n", errno);
}
#endif

// Serial port to file mapping
void emuSerPort::setMode(int newMode)
{
    if(newMode != m_serMode)
    {
        if(newMode == SER_MODE_PORT)
        {
            if(m_outFileOpen)
            {
                fclose(m_fhOutFile);
                m_outFileOpen = false;
            }
            if(m_inFileOpen)
            {
                fclose(m_fhInFile);
                m_inFileOpen = false;
            }
            m_serStat &= ~(SER_INFILE_OPEN | SER_OUTFILE_OPEN);
            m_serStat |= SER_UPD_FLAG;
            m_serMode = SER_MODE_PORT;
        }
        else if(newMode == SER_MODE_FILE)
        {
            if(m_portOpen)
            {   // close
                closeSerPort();
                m_portOpen = false;
            }
            m_serStat &= ~SER_PORT_OPEN;
            m_serStat |= SER_UPD_FLAG;
            m_serMode = SER_MODE_FILE;
        }
        else
        {}
    }
}

int emuSerPort::getMode(void)
{
    int modeStat = m_serMode | m_serStat;
    m_serStat &= ~SER_UPD_FLAG;
    return modeStat;
}

int emuSerPort::openInFile(wxString pname)
{
    int ret = 0;
    if(m_inFileOpen)
    {
        fclose(m_fhInFile);
        m_serStat &= ~SER_INFILE_OPEN;
        m_serStat |= SER_UPD_FLAG;
        m_inFileOpen = false;
    }

    if(pname != "")
    {
        m_fhInFile = fopen (pname, "r");
        if (m_fhInFile < 0)
            wxLogDebug("m_fhInFile: error %d opening %s %s\n", errno, pname, strerror (errno));
        else
        {
            ret = 1;
            m_inFileOpen = true;
            m_serStat |= SER_UPD_FLAG | SER_INFILE_OPEN;
            setMode(SER_MODE_FILE);
        }
    }
	return ret;
}

int emuSerPort::openOutFile(wxString pname)
{
    int ret = 0;
    if(m_outFileOpen)
    {
        fclose(m_fhOutFile);
        m_outFileOpen = false;
        m_serStat &= ~SER_OUTFILE_OPEN;
        m_serStat |= SER_UPD_FLAG;
    }

    if(pname != "")
    {
        m_fhOutFile = fopen (pname, "w");
        if (m_fhOutFile < 0)
            wxLogDebug("m_fhOutFile: error %d opening %s %s\n", errno, pname, strerror (errno));
        else
        {
            ret = 1;
            m_outFileOpen = true;
            m_serStat |= SER_UPD_FLAG | SER_OUTFILE_OPEN;
            setMode(SER_MODE_FILE);
        }
    }
	return ret;
}

int emuSerPort::readInFile(uint8_t* pdata, int maxNum)
{
    int ret = -1;
    if(m_inFileOpen)
    {
        int n = fread(pdata, 1, maxNum, m_fhInFile);  // read data
        //wxLogDebug("SERPORT: File read %d", n);
        if(n > 0)
            ret = 1;
    }
    return ret;
}

int emuSerPort::writeOutFile(uint8_t* pdata, int num)
{
    int ret = -1;
    if(m_outFileOpen)
    {
        int n = fwrite(pdata, 1, num, m_fhOutFile);  // write data
        if(n == num)
            ret = 1;
    }
    return ret;
}
