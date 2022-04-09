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
// based on code by Adam Courchesne (@onecircuit)
// Fluke 9010 tape emulation emulation

#ifndef _EMUSERPORT_
#define _EMUSERPORT_

#include <wx/wx.h>
#include <stdio.h>
#include <stdint.h>

#if defined(__UNIX__)
#define LINUX_SERPORT       // activate linux serial port implementation
#endif

#define SER_PARITY_ERROR    0x01
#define SER_FRAMING_ERROR   0x02
#define SER_OVERRUN_ERROR   0x04
#define SER_DATA_RX         0x08
#define SER_TX_EMPTY        0x10

#define SERPORT_RXBUF_SIZE  (16*1024)

class emuSerPort
{
  public:
    emuSerPort(void);

    void Reset(void);
    void Write(uint16_t addr, uint8_t data);
    uint8_t Read(uint16_t addr);

    int setSerPort(wxString pname, int speed);
    int openSerPort(wxString pname, int speed);
    int closeSerPort(void);
    int readSerPort(uint8_t* pdata, int maxNum);
    int writeSerPort(uint8_t* pdata, int num);

  private:
    uint8_t     m_status;

    int         m_fhPort;	    // serial port handle
    wxString    m_portName;
    int         m_baudRate;
    bool        m_portOpen;

    uint8_t     m_rxBuf[SERPORT_RXBUF_SIZE];
    int         m_rxBufWind;
    int         m_rxBufRind;
};

#endif
