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
// Fluke 9010 display and keyboard emulation

#ifndef EMUDISPKEYB_H
#define EMUDISPKEYB_H



#include <stdint.h>
#include "wx/wx.h"

class emuDispKeyb
{
 public:
    emuDispKeyb(void); // constructor
    ~emuDispKeyb(void);
    void Reset(void);

    void Write(uint16_t address, uint8_t data);
    uint8_t Read(uint16_t address);
    void KeyPressed(uint8_t key);

    int GetLedStatus(void);
    char* GetDispChars(void);
    int GetFlashPos(void);

    void Update(void);

    uint8_t GetStatus(void);

    bool updDisp(int64_t tms);
    bool updLed(int64_t tms);

 private:
    // Variables
    uint8_t m_lastKey;
    bool    m_keyValid;

    bool    m_dispUpd;   // signal display update
    bool    m_ledUpd;   // signal led update
    uint8_t m_ledState;
    uint8_t m_ledFlash;
    int     m_ledFlashPerMs;
    int     m_ledFlashDurMs;
    bool    m_ledFlashOn;

    uint8_t charReg[32];    // 32 character register
    uint8_t m_dispPos;      // current position within this regester

    bool    m_flashMode;
    uint8_t m_flashPos;
    uint8_t m_flashChar;
    int     m_flashPerMs;
    int     m_flashDurMs;
    bool    m_flashOn;

    uint8_t status;
    bool scanSkipMode;
    int scans;


    // Functions
    void Clear(void);
    void Disable(uint8_t data);
};

#endif // EMUDISPKEYB_H
