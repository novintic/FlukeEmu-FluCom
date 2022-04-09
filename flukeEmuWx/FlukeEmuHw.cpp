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
// based on code by Adam Courchesne (@onecircuit)

#include "FlukeEmuHw.h"
#include "wx/wx.h"

flukeEmuHw::flukeEmuHw() : wxThread(wxTHREAD_JOINABLE)
{
    // set address limits
    ROMStartAddr = 0x0000;
    ROMLimitAddr = 0xBFFF;
    RAMStartAddr = 0xC000;
    RAMLimitAddr = 0xFFFF;
    // Load ROMS
    LoadROMs(ROM, flukeROMs);
    // reset HW
    resetHw();

    m_dispUpd = false;
    m_ledUpd  = false;

    // execution control
    m_runEmu    = false;
    m_resetEmu  = true;
    m_exitEmu   = false;
    m_execIntUs = 2000;
    m_emuCycles = (int)(((int64_t)3250000*(int64_t)m_execIntUs)/1000000);

    m_totEmuTime = 0;
    m_cntEmuRuns = 0;
    m_avgEmuTime = 0;
    m_maxEmuTime = 0;
    m_cntEmuRunsLast = 0;
}

flukeEmuHw::~flukeEmuHw()
{
}

// The thread execution function
wxThread::ExitCode flukeEmuHw::Entry()
{
    int64_t lastT = 0;
    wxLogDebug("EMU: ec: %d  ei: %d us", m_emuCycles,  m_execIntUs);
    // emulation loop
    while(!m_exitEmu)
    {
        //wxLongLong wxStT = wxGetLocalTimeMillis();
        wxLongLong wxStT = wxGetUTCTimeUSec();
        int64_t stT = wxStT.GetValue();

        if(m_resetEmu)
        {
            resetHw();
            m_resetEmu = false;
        }
        if(m_runEmu)
        {
            runHw(m_emuCycles);
        }
        //wxLongLong wxEnT = wxGetLocalTimeMillis();
        wxLongLong wxEnT = wxGetUTCTimeUSec();
        int64_t enT = wxEnT.GetValue();
        int64_t tdiff = enT - stT;
        // update stats if emu has run
        if(m_runEmu)
        {
            m_totEmuTime += tdiff;
            m_cntEmuRuns++;
            m_avgEmuTime = m_totEmuTime / m_cntEmuRuns;
            m_maxEmuTime = tdiff > m_maxEmuTime ? tdiff : m_maxEmuTime;
        }

        if((enT - lastT) >= 1000000)
        {
            lastT = enT;
            dispStats();
        }

        int slt = (m_execIntUs - tdiff) >= 0 ? m_execIntUs - tdiff : 0;

        // Sleep
        wxMicroSleep(slt);
    }

    return 0;
}

// Start, pause, reset, end emulation thread
void flukeEmuHw::execCtrl(int cmd)
{
    if(cmd == EMU_RESET)
        m_resetEmu = true;
    else if(cmd == EMU_RUN)
        m_runEmu = true;
    else if(cmd == EMU_PAUSE)
        m_runEmu = false;
    else if(cmd == EMU_EXIT)
        m_exitEmu = true;
}

// Start, pause, reset, end emulation thread
void flukeEmuHw::dispStats(void)
{
    wxLogDebug("EMU: avg:%.1f max: %.1f ms runs: %d - %d", m_avgEmuTime, m_maxEmuTime, m_cntEmuRuns, m_cntEmuRuns - m_cntEmuRunsLast);
    m_cntEmuRunsLast = m_cntEmuRuns;
}

void flukeEmuHw::resetHw(void)
{
    ResetZ80(&Z80Regs);
    m_dispKeyb.Reset();
    //m_podProbe.Reset();
    //m_tapeUnit.Reset();
    m_evCntSigGen.Reset();
    m_serPort.Reset();
}

int flukeEmuHw::runHw(int nCycles)
{
    int val = ExecZ80(&Z80Regs, nCycles);
    //z80_raise_IRQ(0xff);
    //z80_lower_IRQ();
    return val;
}

// *******************************************************************
// Port IO read / writes

void flukeEmuHw::writeIO(uint16_t addr, uint8_t val)
{
    // alias address as in HW (SW won't work correctly otherwise)
    int ioSel = (addr & 0x00E0) >> 5;
    //wxLogDebug("Writing to address: %X\n",addr);
    if (ioSel <= 3)
    {
        // write to event cntr or sig gen
        m_evCntSigGen.Write(addr, val);
        wxLogDebug("W:SIG GEN: %02x %02x", addr, val);
    }
    else if (ioSel == 4)
    {
        // write to keyboard/display unit
        m_dispKeyb.Write(addr, val);
    }
    else if (ioSel == 5)
    {
        // write to tape drive
        m_tapeUnit.Write(addr, val);
    }
    else if (ioSel == 6)
    {
        // write to POD/Probe PIA
        m_podProbe.WriteF9010(addr, val);
    }
    else if (ioSel == 7)
    {
        // write to RS232
        m_serPort.Write(addr, val);
    }
    else
    {
    }
}

uint8_t flukeEmuHw::readIO(uint16_t addr)
{
    uint8_t val = 0;
    // alias address as in HW (SW won't work correctly otherwise)
    int ioSel = (addr & 0x00E0) >> 5;
    //wxLogDebug("Read address: %X\n",addr);
    if (ioSel <= 3)
    {   // read from event cntr or sig gen
        val = m_evCntSigGen.Read(addr);
    }
    else if (ioSel == 4)
    {
        // get status from display unit
        if (addr == 0x0080)
            val = m_dispKeyb.Read(addr);
        else if (addr == 0x0081)
            //return display->GetStatus() | keyboard->GetStatus();
            val = m_dispKeyb.GetStatus();
    }
    else if (ioSel == 5)
    {
        // read from tape drive
        val = m_tapeUnit.Read(addr);
    }
    else if (ioSel == 6)
    {
        // read from to POD/Probe PIA
        val = m_podProbe.ReadF9010(addr);
    }
    else if (ioSel == 7)
    {
        // read from RS232
        val = m_serPort.Read(addr);
    }
    else
    {
        wxLogDebug("R: IO: %04x", addr);
    }

    return val;
}

// *******************************************************************
// Memory read / writes

void flukeEmuHw::writeMem(uint16_t addr, uint8_t val)
{
    // check address
    if((addr >= RAMStartAddr) && (addr <= RAMLimitAddr))
        RAM[addr - RAMStartAddr] = val;
    else
        wxLogDebug("Z80 WR: ADDR %02x (Val %02x) NOT in MAP\n", addr,val);

}

uint8_t flukeEmuHw::readMem(uint16_t addr)
{
    uint8_t val = 0;
    // check address
    if((addr >= RAMStartAddr) && (addr <= RAMLimitAddr))
        val = RAM[addr - RAMStartAddr];
    else if((addr >= ROMStartAddr) && (addr <= ROMLimitAddr))
        val = ROM[addr - ROMStartAddr];
    else
        wxLogDebug("Z80 RR: ADDR %02x NOT in MAP\n", addr);
    return val;
}

void flukeEmuHw::LoadROMs(uint8_t *ROM, ROMPage *romp)
{
	FILE *romFile;
	while (romp->name)
	{
        if ((romFile = fopen(romp->name,"rb")) == NULL)
		{
			wxLogDebug("Unable to open file %s\n",romp->name);
		}

		if (fread(ROM + romp->startAddress,1,romp->size,romFile) != romp->size)
		{
			wxLogDebug("Unable to read file %s\n",romp->name);
			fclose(romFile);
		}
		fclose(romFile);
		romp++;
	}
}


// *******************************************************************
// C-Hooks for Z80 emulator

// Mapping to Marat Z80 emulator functions
flukeEmuHw* emuInst = NULL;

// Call this function to set the pointer to emuCore instance
// This is required to correctly reroute the Z80 emulator calls
void flukeEmuHwSetInst(flukeEmuHw* emuInstPar)
{
    emuInst = emuInstPar;
}

// Z80 needs these functions
// Z80 Write handler
void WrZ80(register word Addr,register byte Value)
{
    if(emuInst != NULL)
        emuInst->writeMem(Addr, Value);
};

// Z80 Read handler
byte RdZ80(register word Addr)
{
    if(emuInst != NULL)
        return emuInst->readMem(Addr);
    return 0;
}

// Z80 Port out handler
void OutZ80(register word Port,register byte Value)
{
    if(emuInst != NULL)
        emuInst->writeIO(Port, Value);
};

// Z80 Port in handler
byte InZ80(register word Port)
{
    if(emuInst != NULL)
        return emuInst->readIO(Port);
    return 0;
};

// Z80 Patch instruction handler
void PatchZ80(register Z80 *R)
{};


word LoopZ80(register Z80 *R)
{
    return INT_NONE;    // INT_QUIT
};


