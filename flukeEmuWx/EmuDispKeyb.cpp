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

#include "EmuDispKeyb.h"

#define OUTPUT_BUFF_FULL   0x01
#define INPUT_BUFF_FULL    0x02
#define LAST_WRITE_CONTROL 0x04
#define OVERRUN_ERROR      0x80


emuDispKeyb::emuDispKeyb(void)
{
    Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Destructor
emuDispKeyb::~emuDispKeyb(void)
{
}

///////////////////////////////////////////////////////////////////////////////
// Return the status byte of the display
uint8_t emuDispKeyb::GetStatus(void)
{
    //return status;
    uint8_t st = m_keyValid ? OUTPUT_BUFF_FULL : 0;
    return st;
}

///////////////////////////////////////////////////////////////////////////////
// Write a byte to the display unit
void emuDispKeyb::Write(uint16_t address, uint8_t data)
{
    // if address == 0080, write to register pointed to by position pointer
    //wxLogDebug("WR: A:%04X D:%02x %d %c", address, data, data, data);
    if (address == 0x0080)
    {
        charReg[m_dispPos] = data;
        m_dispPos = ++m_dispPos % 32; // Overflow is used by the ROMs!!
        // last write was data
        status = status & (!LAST_WRITE_CONTROL);
        m_dispUpd = true;

        // Debug output
        //char str[64];
        //memcpy(str, charReg, 32);
        //str[32] = 0;
        //wxLogDebug("DISP: %s", str);
        return;
    }
    else if (address == 0x0081)
    {
        // save tag for use later
        uint8_t tag = data & 0xE0;
        uint8_t val = data & 0x1F;

        //wxLogDebug("WR: A:%04X D:%02x %d %c", address, data, data, data);

        if (data == 0xFF)
        {
            wxLogDebug("DISP RESET");
            Reset();
        }
        else if(tag == 0x00)
        {   // 000x xxxx
            if (val == 0x00)
            {
                // reset test mode
            }
            else if (val == 0x01)
            {
                // display segment self test
            }
            else if (val == 0x02)
            {
                // digit drivers self test
            }
            else if (val == 0x04)
            {
                // annunciator self test
            }
            else if (val == 0x08)
            {
                // keyboard self test
            }
            else
                wxLogDebug("TAG 0: %02x", val);
        }
        else if (tag == 0x20)
        {   // 001x xxxx Set write pointer
            m_dispPos = val;
            //wxLogDebug("DPOS %02d", m_dispPos);
        }
        else if (tag == 0x40)
        {   // 010x xxxx Set LED state
            wxLogDebug("LED ON %02x -> S: %02x", m_ledState, m_ledState | val);
            m_ledState |= val;
            m_ledUpd = true;
        }
        else if (tag == 0x60)
        {   // 011x xxxx Set flash position and turn on flash mode
            m_flashPos  = val;
            m_flashMode = true;
            m_dispUpd = true;
            m_flashChar = charReg[m_flashPos];
            wxLogDebug("FLASH POS: %02d [%c]", m_flashPos, m_flashChar);
        }
        else if (tag == 0x80)
        {   // 100x xxxx turn of flash mode
            m_flashMode = false;
            wxLogDebug("FLASH OFF");
        }
        else if (tag == 0xA0)
        {   // 101x xxxx Set LED BLINK state (found by observation, not in service manual)
            wxLogDebug("LED OFF %02x -> S: %02x", m_ledState, m_ledState & ~val);
            m_ledState &= ~val;

            m_ledUpd = true;
        }
        else if (tag == 0xE0)  // Service manual page 4-32 says C0. By observation 0xE0
        {   // 110x xxxx clears the display, or sets scan mode
            if(val == 0x00)
            {
                Clear();
                wxLogDebug("DISP CLEAR");
            }
            else
                wxLogDebug("TAG E: %02x", val);
        }
        else if (tag == 0xC0) // Correct?
        {
            // turns of display for N scans of the display
            scans = val;
            wxLogDebug("DISP SCAN C %d", val);
        }
        else
        {
            wxLogDebug("DISP: unknown CMD A:%04x D:%02x [T:%02x V:%02x]\n", address, data, tag, val);
            // ?? Check for this
        }
        // last write was control
        status = status | LAST_WRITE_CONTROL;
    }
}


uint8_t emuDispKeyb::Read(uint16_t address)
{
    uint8_t val = 0xff;
    if (m_keyValid)
    {
        //wxLogDebug("Key read %d", m_lastKey);
        val = m_lastKey;
        m_keyValid = false;
        m_lastKey = 0xff;
    }
    return val;
}

int emuDispKeyb::GetLedStatus(void)
{
    return m_ledFlashOn ? m_ledState : 0;
}

char* emuDispKeyb::GetDispChars(void)
{
    return (char*)charReg;
}

int emuDispKeyb::GetFlashPos(void)
{
    return m_flashMode ? m_flashPos : -1;
}

// Returns true if display update needed
bool emuDispKeyb::updDisp(int64_t tms)
{
    bool ret = m_dispUpd;
    bool flashEdge = false;
    m_dispUpd = false;

    if(m_flashMode)
    {
        int tinp = tms % m_flashPerMs;
        bool flashOn = tinp < m_flashDurMs;
        flashEdge = (m_flashOn != flashOn);
        m_flashOn = flashOn;
        if(flashEdge)
        {
            charReg[m_flashPos] = m_flashOn ? m_flashChar : ' ';
            //wxLogDebug("FLASH %d %s", m_flashPos, m_flashOn ? "ON":"OFF");
        }
    }
    return ret || flashEdge;
}

// Returns true if LED update needed
bool emuDispKeyb::updLed(int64_t tms)
{
    bool ret = m_ledUpd;
    bool flashEdge = false;
    int tinp = tms % m_ledFlashPerMs;
    bool flashOn = tinp < m_ledFlashDurMs;

    flashEdge = (m_ledFlashOn != flashOn);
    m_ledFlashOn = flashOn;
    m_ledUpd = false;
    return ret || flashEdge;
}


void emuDispKeyb::KeyPressed(uint8_t key)
{
    //wxLogDebug("Key pressed %d NNC",key);
    if (!m_keyValid)
    {
        wxLogDebug("Key pressed %d",key);
        m_lastKey = key;
        m_keyValid = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
void emuDispKeyb::Reset(void)
{
    m_dispPos = 0;
    m_flashMode  = true;
    m_flashPos   = 8;
    m_flashChar  = '*';
    m_flashPerMs = 1000;
    m_flashDurMs = 500;
    m_flashOn    = false;
    scanSkipMode = false;
    scans = 0;
    Clear();
    charReg[0] = 'B';
    charReg[1] = 'O';
    charReg[2] = 'O';
    charReg[3] = 'T';
    charReg[4] = 'I';
    charReg[5] = 'N';
    charReg[6] = 'G';
    charReg[7] = ' ';
    charReg[8] = '*';

    m_ledState = 0x00;
    m_ledFlash = 0x00;
    m_ledFlashPerMs = 300;
    m_ledFlashDurMs = 200;
    m_ledFlashOn = false;

    // init status
    status = 0x00;

    m_keyValid = false;
    m_lastKey = -1;
}

///////////////////////////////////////////////////////////////////////////////
void emuDispKeyb::Clear(void)
{
    int i;

    for (i=0;i<32;i++)
        charReg[i] = ' ';

    m_dispPos = 0;
    m_flashMode = false; // turn off flashing
}

///////////////////////////////////////////////////////////////////////////////
void emuDispKeyb::Update(void)
{
    // check to see if the display is disabled
    if (scanSkipMode)
    {
        if (scans == 0)
            scanSkipMode = false;
        else
            scans--;
    }
}


