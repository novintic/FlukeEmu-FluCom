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

#ifndef EMUPODPROBE_H
#define EMUPODPROBE_H

// PIA R6520 emulation
// Clemens Bürgi 2022
#include <stdint.h>
#include <stdio.h>


class emuPodProbe
{
    public:
        emuPodProbe(void);
        ~emuPodProbe(void);

        // Direct Fluke 09010 emulation (move to upper class)
        void WriteF9010(uint16_t address, uint8_t data);
        uint8_t ReadF9010(uint16_t address);
        void beeper(bool trig);
        bool beepF9010(void);

        void setCS(bool cs0, bool cs1, bool ncs2);
        void setRS(bool rs0, bool rs1);
        void setCxn(int xab, bool n2, bool val);
        void WriteData(uint8_t data);
        uint8_t ReadData(uint16_t address);

        // Functions to handle pin mapping to Emu HW
        /*virtual*/ void InitEmuIO(void);
        /*virtual*/ void SetPIDir(int piSel, uint8_t DDReg);
        /*virtual*/ void SetPI(int piSel, uint8_t OReg);
        /*virtual*/ uint8_t GetPI(int piSel, uint8_t PIBstate);
        /*virtual*/ void setCx2(int xab, bool val, bool edgeUp);
        /*virtual*/ bool getCxn(int xab, bool n2);

        const int SELPIA = 0;
        const int SELPIB = 1;

        // Control register bits
        const int CRx_IRQx1_EN      = 0x01; // enable interrupt A|B lines for int 1
        const int CRx_IRQx1_POSEDGE = 0x02; // enable interrupt on pos edge on input 1
        const int CRx_IRQx1_SELOR   = 0x04;
        const int CRx_IRQx2_EN      = 0x08; // enable interrupt A|B lines for int 2
        const int CRx_IRQx2_POSEDGE = 0x10; // enable interrupt on pos edge on input 2

        const int CRx_Cx2_STROBECTRL= 0x08; // Output auto/strobe returns
        const int CRx_Cx2_OUTCTRL   = 0x10; // Set mode: 1:direct control by CRx_Cx2_STROBECTRL, 0:auto return

        const int CRx_Cx2_MODE_OUT  = 0x20; // set C[A|B]2 output mode
        const int CRx_IRQx2_PEND    = 0x40; // int [A|B]2 is pending
        const int CRx_IRQx1_PEND    = 0x80; // int [A|B]1 is pending

    private:
        // registers
        uint8_t CR[2];
        uint8_t OR[2];
        uint8_t DDR[2];
        uint8_t PIB[2];
        // Control signals
        bool RS0;
        bool RS1;
        bool CSEL;  // chipsel
        bool Cx1[2];
        bool Cx2[2];

        // F9010A stuff (move to upper class)
        bool m_beepTrig;
        bool m_beepTrigLast;
};



#endif // EMUPODPROBE_H
