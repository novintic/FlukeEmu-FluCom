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
// Fluke 9010 event counter and signature generator emulation

#include "EmuEvCntSigGen.h"

emuEvCntSigGen::emuEvCntSigGen(void)
{
    Reset();
}

emuEvCntSigGen::~emuEvCntSigGen(void)
{

}

void emuEvCntSigGen::Reset(void)
{
    m_fb = 0x80;
    m_evCnt = 0 | m_fb; // bit 7 is !(Fuse blown)
    m_sigGen = 0;
}

void emuEvCntSigGen::Write(uint16_t addr, uint8_t data)
{
    wxLogDebug("W: SIGGEN: %04x %02x (UKN)", addr, data);
}

uint8_t emuEvCntSigGen::Read(uint16_t addr)
{
    uint8_t val = 0;
    int ioSel = (addr & 0x00E0) >> 5;
    if(ioSel == 0) // read event counter & FB bit
    {
        val = m_evCnt;
    }
    else if(ioSel == 1)   // reset event cnt and sig gen
    {
        m_evCnt = 0 | 0x80; // bit 7 is !(Fuse blown)
        m_sigGen = 0;
        //wxLogDebug("R: SIGGEN: %04x (RESET)", addr);
    }
    else if(ioSel == 2)   // read signature hi byte
    {
        val = m_sigGen >> 8;
        //wxLogDebug("R: SIGGEN: %04x (SIG HI)", addr);
    }
    else if(ioSel == 3)    // read signature low byte
    {
        val = m_sigGen & 0xff;
        //wxLogDebug("R: SIGGEN: %04x (SIG LO)", addr);
    }
    else
    {   // should not get here!
        wxLogDebug("R: SIGGEN: %04x (UKN)", addr);
    }
    return val;
}

void emuEvCntSigGen::onSyncEvent(bool event)
{
    // count event, 7 bit overflow
    if(event)
        m_evCnt = ((m_evCnt & 0x7f) + 1) | m_fb;
    // run sig gen LFSR with taps 6, 8, 11, 15
    int fback = ((m_sigGen & 0x0040) ? 0x1 : 0x0) ^
                ((m_sigGen & 0x0100) ? 0x1 : 0x0) ^
                ((m_sigGen & 0x0800) ? 0x1 : 0x0) ^
                ((m_sigGen & 0x8000) ? 0x1 : 0x0);
    int in = (event ? 0x1 : 0x0) ^ fback;
    m_sigGen = (m_sigGen << 1) | in;
}

