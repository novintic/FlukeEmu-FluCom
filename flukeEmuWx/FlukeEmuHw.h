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
// Fluke 9010 main HW emulation


#ifndef FLUKEEMUHW_H
#define FLUKEEMUHW_H

#include "stdint.h"

#include "Z80/z80emu.h"
#include "Z80/z80user.h"
#include "EmuPodProbe.h"
#include "EmuTapeUnit.h"
#include "EmuDispKeyb.h"
#include "EmuEvCntSigGen.h"
#include "EmuSerPort.h"

#include "ProbeBoard.h"

typedef struct RM
{
	const char *name;
	unsigned int startAddress;
	unsigned int size;
}ROMPage;

#define EMU_RESET   0
#define EMU_RUN     1
#define EMU_PAUSE   2
#define EMU_EXIT    3

class flukeEmuHw : public wxThread
{
 public:
    flukeEmuHw();
    ~flukeEmuHw();
    wxThread::ExitCode Entry();

    void execCtrl(int cmd);
    void dispStats(void);

    void initHw(void);
    void resetHw(void);
    int runHw(int nCycles);

    void writeIO(uint16_t addr, uint8_t val);
    uint8_t readIO(uint16_t addr);
    void writeMem(uint16_t addr, uint8_t val);
    uint8_t readMem(uint16_t addr);

    void LoadROMs(uint8_t *ROM, ROMPage *romp);

    // Event counter/sig generator
    emuEvCntSigGen m_evCntSigGen;
    ProbeBoard m_probeBoard;
    ProbeBoard *m_pPB;    // pointer to probeboard instance

    // Display/LEDs/keyboard
    emuDispKeyb m_dispKeyb;
    // IO chip
    emuPodProbe m_podProbe;
    // Tape unit
    emuTapeUnit m_tapeUnit;
    // Serial Port
    emuSerPort  m_serPort;

 protected:
    Z80_STATE Z80State;    // Z80 registers
    ROMPage flukeROMs[4] =
    {
        {"media/128C900.U9",  0x0000, 0x4000},
        {"media/128C901.U19", 0x4000, 0x4000},
        {"media/128C902.U21", 0x8000, 0x4000},
        {0,0,0}	// end of table
    };
    // Vars
    uint16_t RAMStartAddr;
    uint16_t RAMLimitAddr;
    uint16_t ROMStartAddr;
    uint16_t ROMLimitAddr;

    uint8_t ROM[49152];
    uint8_t RAM[16384];

    // GUI update requests
    bool    m_dispUpd;
    bool    m_ledUpd;

    // execution control
    bool    m_runEmu;
    bool    m_resetEmu;
    bool    m_exitEmu;
    int     m_execIntUs;
    int     m_emuCycles;
    // execution statistics
    float   m_totEmuTime;
    int     m_cntEmuRuns;
    int     m_cntEmuRunsLast;
    float   m_avgEmuTime;
    float   m_maxEmuTime;
};

// Function to set emu instance. Required for interface to Z80
void flukeEmuHwSetInst(flukeEmuHw* emuInst);

#endif // FLUKEEMUHW_H
