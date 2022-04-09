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
// Fluke 9010 tape emulation

#ifndef _EMUTAPEUNIT_
#define _EMUTAPEUNIT_

#include <stdio.h>
#include <stdint.h>
#include "wx/wx.h"

#define MAX_BUFFER_SIZE 10190

// Tape uP status
#define OUTPUT_BUFFER_FULL 0x01
#define INPUT_BUFFER_FULL  0x02
#define NO_CASSETTE_LOADED 0x04
#define LAST_WRITE_COMMAND 0x08
#define CASSETTE_WP        0x10
#define END_OF_TAPE        0x20
#define ERROR_DETECTED     0x40
#define END_OF_FILE        0x80

#define WRITEMODE_REWIND_START 0x11
#define READMODE_REWIND_START  0x12
#define STOP_TAPE              0x13

#define WRITE_MODE 1
#define READ_MODE  2
#define IDLE       0

class emuTapeUnit
{
  public:
    emuTapeUnit(void);

    void Write(uint16_t address, uint8_t data);
    uint8_t Read(uint16_t address);

    void setTapeFileName(wxString fileName);
    bool getTapeFileSet(void);

  private:
    wxString  m_tapeFileName;
    bool  m_tapeFileSet;
    bool  m_tapeFileWP;
    FILE* m_tfHandle;

    uint8_t m_status;
    uint8_t m_tapeBuffer[MAX_BUFFER_SIZE];
    int m_tapeBuffPtr;
    int m_bufferSize;
    int m_state;
    int m_prevState;

    void SaveTapeFile(void);
    void LoadTapeFile(void);
    void ClearBuffer(void);
};

#endif
