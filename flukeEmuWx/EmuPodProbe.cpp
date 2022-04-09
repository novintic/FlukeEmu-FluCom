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
// PIA R6520 emulation

#include "EmuPodProbe.h"
#include "wx/wx.h"  // for DEBUG msg

//#define DEBUG_PIAREG      // activate PIA6520 register messages in console
//#define DEBUG_PODIO       // activate POD io messages in console

emuPodProbe::emuPodProbe(void)
{
    // Control signals
    RS0 = false;
    RS1 = false ;
    CSEL = false;  // chipsel
    Cx1[SELPIA] = false;
    Cx1[SELPIB] = false;
    Cx2[SELPIA] = false;
    Cx2[SELPIB] = false;

    RS1 = false ;
    CSEL = false;  // chipsel
    // Init registers
    CR[SELPIA] = 0;
    OR[SELPIA] = 0;
    DDR[SELPIA] = 0;
    PIB[SELPIA] = 0;
    CR[SELPIB] = 0;
    OR[SELPIB] = 0;
    DDR[SELPIB] = 0;
    PIB[SELPIB] = 0;

    m_beepTrig = false;
    m_beepTrigLast = false;
    // Init emu IOs
    InitEmuIO();
}

emuPodProbe::~emuPodProbe(void)
{
}

// Fluke 9010 connections
//        - 6520
// Z80 A0 - rs 0  -> Control register sel 0 |  10 = read/write CRA
//                                             00 & CRA bit2 = 0 read/write DDRA
//                                             00 & CRA bit2 = 1 read PIBA /write ORA
// Z80 A1 - rs 1  -> Control register sel 1 |  11 = read/write CRB
//                                             01 & CRB bit2 = 0 read/write DDRD
//                                             01 & CRB bit2 = 1 read PIBB /write ORB

// high   - cs 0
// high   - cs 1
// ?? - ncs 2

// PA0    - POD nMAINSTAT
// PA1    - POD nPODSTAT
// PA2    - POD POWERFAIL
// PA3    - POD nRESET

// PA 5,6,7 -> Probe
// CB2      -> Probe

// PB0-7    -> POD 0-7

void emuPodProbe::WriteF9010(uint16_t address, uint8_t data)
{
    setCS(true, true, false);
    setRS((address & 0x1) ? true : false, (address & 0x2) ? true : false);
    WriteData(data);

#ifdef DEBUG_PODIO
    // Debug messages
    int pibsel = RS1 ? SELPIB : SELPIA;
    if(!RS0 && (CR[pibsel] & CRx_IRQx1_SELOR))
    {
        if(pibsel == SELPIA)
            wxLogDebug("W:A: %02x | nMAINSTAT:%d  nRESET:%d", OR[pibsel], OR[pibsel] & 0x1 ? 1:0, OR[pibsel] & 0x8 ? 1:0);
        else
            wxLogDebug("W:B: %02x", OR[pibsel]);
    }
#endif
}

uint8_t emuPodProbe::ReadF9010(uint16_t address)
{
    setCS(true, true, false);
    setRS((address & 0x1) ? true : false, (address & 0x2) ? true : false);

#ifdef DEBUG_PODIO
    int pibsel = RS1 ? SELPIB : SELPIA;
    if(!RS0 && (CR[pibsel] & CRx_IRQx1_SELOR))
    {
        if(pibsel == SELPIA)
            wxLogDebug("R:A: %02x | nPODSTAT:%d  PWRFAIL:%d", PIB[pibsel], PIB[pibsel] & 0x2 ? 1:0, PIB[pibsel] & 0x4 ? 1:0);
        else
            wxLogDebug("R:B: %02x", PIB[pibsel]);
    }
#endif
    return ReadData(address);
}

bool emuPodProbe::beepF9010(void)
{
    bool beep = m_beepTrig;
    m_beepTrig = false;
    return beep;
}

// &530 proper emulation
void emuPodProbe::setCS(bool cs0, bool cs1, bool ncs2)
{
    CSEL = cs0 && cs1 && !ncs2;  // chipsel
}

void emuPodProbe::setRS(bool rs0, bool rs1)
{
    if(CSEL)
    {
        RS0 = rs0;
        RS1 = rs1;
    }
}

// set Cxn input, external inputs
void emuPodProbe::setCxn(int xab, bool n2, bool val)
{
    if(!n2) // n=1
    {
        Cx1[xab] = val;
    }
    else
    {
        // input mode?
        if(!(CR[xab] & CRx_Cx2_MODE_OUT))
        {
            Cx2[xab] = val;
        }
    }
}

void emuPodProbe::WriteData(uint8_t data)
{
    if(CSEL)
    {
        int pibsel = RS1 ? SELPIB : SELPIA;
        //printf("PI%c %02x\n", RS0 ? 'B':'A', data);
        // write CRA
        if(RS0)
        {
            CR[pibsel] = data;
            // check C[A|B][1|2]
            if(CR[pibsel] & CRx_Cx2_MODE_OUT)
            {
                if(CR[pibsel] & CRx_Cx2_OUTCTRL)
                {
                    bool oldCx2 = Cx2[pibsel];
                    Cx2[pibsel] = CR[pibsel] & CRx_Cx2_STROBECTRL ? true : false;
                    setCx2(pibsel, Cx2[pibsel], (oldCx2 == false) && (Cx2[pibsel] == true));
                }
            }
        }
        else
        {
            // 00 & CRA bit2 = 1 read PIBA /write ORA
            // 00 & CRA bit2 = 0 read/write DDRA
            if(CR[pibsel] & CRx_IRQx1_SELOR)
            {
                OR[pibsel] = data;
                // Set output pins  here
                SetPI(pibsel, OR[pibsel]);
            }
            else
            {
                DDR[pibsel] = data;
                // Set pin direction here
                SetPIDir(pibsel, DDR[pibsel]);
            }
        }
    }
}

uint8_t emuPodProbe::ReadData(uint16_t address)
{
    uint8_t data = 0;
    if(CSEL)
    {
        int pibsel = RS1 ? SELPIB : SELPIA;
        // Read CRA
        if(RS0)
        {
            data = CR[pibsel];
            // reset int lines Cx1,2 on read of ctrl reg
            CR[pibsel] &= ~CRx_IRQx1_PEND;
            CR[pibsel] &= ~CRx_IRQx2_PEND;
        }
        else
        {
            // 00 & CRA bit2 = 1 read PIBA /write ORA
            // 00 & CRA bit2 = 0 read/write DDRA
            if(CR[pibsel] & CRx_IRQx1_SELOR)
            {
                PIB[pibsel] = GetPI(pibsel, PIB[pibsel]);    // read pin data here
                //PIB[pibsel]  |= (DDR[pibsel] & OR[pibsel]); // OR in output data
                data = PIB[pibsel];
            }
            else
            {
                data = DDR[pibsel];
            }
        }
        //wxLogDebug("PIA RD: %04x -> %02x\n", data);
    }
    return data;
}

// Overwrite for your pin mapping
// Pin mapping for PodHat by Mirko Biry
#define PIA_D0 26   // Fluke 9010 nMAINSTAT OUT
#define PIA_D1 11   // Fluke 9010 nPODSTAT  IN
#define PIA_D2 06   // Fluke 9010 POWERFAIL IN
#define PIA_D3 10   // Fluke 9010 nRESET    OUT
#define PIA_D4 -1   // Fluke 9010 Beeper    OUT (at startup)
#define PIA_D5 -1   // Fluke 9010 Probe     IN/OUT (OUT at startup)
#define PIA_D6 -1   // Fluke 9010 Probe     IN/OUT (OUT at startup)
#define PIA_D7 -1   // Fluke 9010 Probe     IN/OUT (OUT at startup)
                    // CB2 pin controls write read to probe circuit

#define PIB_D0 27       // GPIO16
#define PIB_D1 28
#define PIB_D2 29
#define PIB_D3 25
#define PIB_D4 24
#define PIB_D5 23
#define PIB_D6 22
#define PIB_D7 21

int pinMAPA[] = {PIA_D0, PIA_D1, PIA_D2, PIA_D3, PIA_D4, PIA_D5, PIA_D6, PIA_D7};
int pinMAPB[] = {PIB_D0, PIB_D1, PIB_D2, PIB_D3, PIB_D4, PIB_D5, PIB_D6, PIB_D7};

//#define WIRINGPI

#ifdef WIRINGPI
#include "wiringPi.h"
#endif // WIRINGPI

void emuPodProbe::InitEmuIO(void)
{
#ifdef WIRINGPI
    wiringPiSetup () ;
#endif // WIRINGPI
}

void emuPodProbe::SetPIDir(int piSel, uint8_t DDReg)
{
    // direction as in DDRx register, 0 = input, 1 = output
    int* pMap = NULL;
    if(piSel == SELPIA) pMap = pinMAPA;
    if(piSel == SELPIB) pMap = pinMAPB;

    if(pMap != NULL)
    {
        for(int pix = 0; pix < 8; pix++)
        {
            if(pMap[pix] >= 0)
            {
#ifdef WIRINGPI
                int mode = DDReg & (1 << pix) ? OUTPUT : INPUT;
                pinMode (pMap[pix], mode);
#endif // WIRINGPI
            }
        }
#ifdef DEBUG_PIAREG
        wxLogDebug("6520: DDR%c:%02x", piSel == SELPIA ? 'A':'B', DDR[piSel]);
#endif
    }
}

void emuPodProbe::SetPI(int piSel, uint8_t OReg)
{
    // output data in OR[piSel] register
    int* pMap = NULL;
    if(piSel == SELPIA) pMap = pinMAPA;
    if(piSel == SELPIB) pMap = pinMAPB;

    if(pMap != NULL)
    {
        for(int pix = 0; pix < 8; pix++)
        {
            if(pMap[pix] >= 0)
            {
#ifdef WIRINGPI
                int val = OReg & (1 << pix) ? HIGH : LOW;
                digitalWrite(pMap[pix], val);
#endif // WIRINGPI
            }
        }
#ifdef DEBUG_PIAREG
        wxLogDebug("6520: OR%c:%02x\n", piSel == SELPIA ? 'A':'B', OR[piSel]);
#endif
    }
    // Beeper
    if(piSel == SELPIA)
    {
        beeper(OReg & 0x10);
    }
    // Probe
    if(piSel == SELPIA)
    {

    }
}

uint8_t emuPodProbe::GetPI(int piSel, uint8_t PIBstate)
{
    // read data into PI[piSel] register
    int* pMap = NULL;
    if(piSel == SELPIA) pMap = pinMAPA;
    if(piSel == SELPIB) pMap = pinMAPB;

    if(pMap != NULL)
    {
        for(int pix = 0; pix < 8; pix++)
        {
            if(pMap[pix] >= 0)
            {
#ifdef WIRINGPI
                int val = digitalRead(pMap[pix]);
                if(val == HIGH)
                    PIBstate |= (1 << pix);
                else
                    PIBstate &= ~(1 << pix);
#endif // WIRINGPI
            }
        }
#ifdef DEBUG_PIAREG
        wxLogDebug("6520: PIB%c:%02x\n", piSel == SELPIA ? 'A':'B', PIB[piSel]);
#endif
    }
    return PIBstate;
}

// set Cx2 output
void emuPodProbe::setCx2(int xab, bool val, bool edgeUp)
{
    // set mapped HW pins

    // Probe circuit
    if(xab == SELPIB)
    {
        //wxLogDebug("W:CB2: %d (%d)", val, edgeUp);
        // a hi on CB2 enables the outputs q1-q3 on U46
        if(val)
        {   // read back PIA_D5, PIA_D6, PIA_D7
            // the probe returns HI, LO, or tristate
            // only one (and at least one) shall be active at a moment in time
            // PA5 signal tristate
            // PA6 signal hi
            // PA7 signal low

            // just set tristate (always)
            PIB[SELPIA] |= (1 << 5);
            //PIB[SELPIA] |= 0xc0;
        }
        // a low resets the latches to 0
        else
        {
        }

        // A rising edge on CB2 latches D5,D6,D7 into U46
        if(edgeUp)
        {
            // PA5 gen hi pulse
            // PA6 gen lo pulse
            // PA7 select sync/freerun
        }
    }
}

// get Cxn input
bool emuPodProbe::getCxn(int xab, bool n2)
{
    // read mapped HW pins
    if(xab == SELPIA)
    {
    }
    else
    {
    }
    return false;
}

// beeper model
void emuPodProbe::beeper(bool trig)
{
    if(!m_beepTrigLast && trig)
    {
        wxLogDebug("6520: BEEP");
        m_beepTrig = true;
    }
    m_beepTrigLast = trig;
}
