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

#ifndef EMUEVCNTSIGGEN_H
#define EMUEVCNTSIGGEN_H

#include <stdint.h>
#include "wx/wx.h"

class emuEvCntSigGen
{
  public:
    emuEvCntSigGen(void);
    ~emuEvCntSigGen(void);

    void Reset(void);
    void onSyncEvent(bool event);
    void Write(uint16_t address, uint8_t data);
    uint8_t Read(uint16_t address);

  protected:
    uint8_t m_fb;
    uint8_t m_evCnt;
    uint16_t m_sigGen;


};



#endif
