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

#include "wx/wx.h"
#include "EmuTapeUnit.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

///////////////////////////////////////////////////////////////////////////////
emuTapeUnit::emuTapeUnit(void)
{
    m_status = 0x00;
    m_tapeFileName = "";
    m_tapeFileSet = false;
    m_tapeFileWP = false;
    m_tfHandle = NULL;
    m_status |= NO_CASSETTE_LOADED;

    ClearBuffer();

    // init state variables
    m_state = IDLE;
    m_prevState = IDLE;
}

bool emuTapeUnit::getTapeFileSet(void)
{
    return m_tapeFileSet;
}

void emuTapeUnit::setTapeFileName(wxString fileName)
{
    // check if file exists
    wxLogDebug("TAPE: File set: %s", fileName);
    // eject cassette ?
    if(fileName == "")
    {
        wxLogDebug("TAPE: Ejecting cassette");
        m_tapeFileSet = false;
        m_status |= NO_CASSETTE_LOADED;
        return;
    }

    m_tfHandle = fopen(fileName, "r");
    if(m_tfHandle != NULL)
    {
        wxLogDebug("TAPE: File exists");
        // check if file is read only (try open with write)
        fclose(m_tfHandle);
        m_tfHandle = fopen(fileName, "r+");
        if(m_tfHandle != NULL)
        {
            m_tapeFileWP = false;
            wxLogDebug("TAPE: File is writeable");
        }
        else
        {
            m_tapeFileWP = true;
            m_tfHandle = fopen(fileName, "r");
            wxLogDebug("TAPE: File is read-only");
        }
    }
    else
    {   // create a new file
        int errnopv = errno;
        wxLogDebug("TAPE: File open failed: %s", strerror(errnopv));

        m_tfHandle = fopen(fileName, "w+");
        wxLogDebug("TAPE: File created");
    }

    if(m_tfHandle != NULL)
    {
        fclose(m_tfHandle);
        m_tapeFileName = fileName;
        m_tapeFileSet = true;
        m_status &= ~NO_CASSETTE_LOADED;
        if(m_tapeFileWP)
            m_status |= CASSETTE_WP;
        // Load file into buffer
        LoadTapeFile();
    }
    else
        wxLogDebug("TAPE: File set error!");
}

///////////////////////////////////////////////////////////////////////////////
void emuTapeUnit::Write(uint16_t address, uint8_t data)
{
    if (address & 0x0001)
    {
        // sending a command to the tape unit's uP
        m_status = m_status | 0x08; // last write was a command
        m_prevState = m_state;

        if (data == WRITEMODE_REWIND_START)
        {
            wxLogDebug("TAPE: RWD & START WRITE");
            m_status &= ~OUTPUT_BUFFER_FULL;
            m_tapeBuffPtr = 0;
            m_state = WRITE_MODE;
        }
        else if (data == READMODE_REWIND_START)
        {
            wxLogDebug("TAPE: RWD & START READ");
            m_status |= OUTPUT_BUFFER_FULL;
            m_tapeBuffPtr = 0;
            m_state = READ_MODE;
        }
        else if (data == STOP_TAPE)
        {
            wxLogDebug("TAPE: STOP");
            if (m_state == WRITE_MODE)
            {
                m_bufferSize = m_tapeBuffPtr;
                // Write to file
                SaveTapeFile();
            }
            else
                m_bufferSize = 0;
        }
        else
        {
            wxLogDebug("TAPE: Writing %X as a command to tape... not sure what this will do\n");
        }

    }
    else
    {
        // writing data to the tape
        m_status = m_status & 0xF7; // last write was data
        wxLogDebug("TAPE: Writing %02x", data);
        m_tapeBuffer[m_tapeBuffPtr++] = data;
        if (m_tapeBuffPtr >= MAX_BUFFER_SIZE)
        {
            wxLogDebug("TAPE: overflow in write !!!!\n");
            m_tapeBuffPtr = 0;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
uint8_t emuTapeUnit::Read(uint16_t address)
{
    uint8_t returnData;

    if (address & 0x0001)
    {
        // reading status from the tape unit's uP
        returnData = m_status;
        //printf("Reading %x status from tape unit \n",returnData);

        // once status is read, we want to clear the end-of tape bit
        //status = status & 0xDF; // clear end-of-tape bit
    }
    else
    {
        // reading data from the tape
        returnData = m_tapeBuffer[m_tapeBuffPtr++];
        wxLogDebug("TAPE: Reading %02x", returnData);
        if (m_tapeBuffPtr >= MAX_BUFFER_SIZE)
        {
            wxLogDebug("TAPE: overflow in read!!!!\n");
            m_tapeBuffPtr = 0;
        }
    }
    return returnData;
}

//********

void emuTapeUnit::SaveTapeFile(void)
{
    if(m_tapeFileSet && !m_tapeFileWP)
    {
        FILE* fh = fopen(m_tapeFileName, "r+");
        if(fh != NULL)
        {
            fprintf(fh, "# FLUKE 9010A tape file\n");
            // Write data
            for (int i = 0; i < m_bufferSize; i++)
            {
                fprintf(fh, "%04x %02x\n", i, m_tapeBuffer[i]);
            }
            fclose(fh);
            wxLogDebug("TAPE: Write file done!");
        }
        else
            wxLogDebug("TAPE: Write file error!");
    }
    else
        wxLogDebug("TAPE: Write file not set or write protected!");
}

void emuTapeUnit::LoadTapeFile(void)
{
    if(m_tapeFileSet)
    {
        FILE* fh = fopen(m_tapeFileName, "r");
        if(fh != NULL)
        {
            char line[128];
            ClearBuffer();
            int lnum = 0;
            while( fgets(line, 128, fh) )
            {
                //wxLogDebug("TAPE: Read: %s", line);
                // ignore comments
                if(line[0] != '#')
                {
                    int a, val;
                    int n = sscanf(line, "%04x %02x", &a, &val);
                    if(n == 2)
                    {
                        m_tapeBuffer[lnum] = val;
                        lnum++;
                        //wxLogDebug("TAPE: Stored: %02x", val);
                    }
                }
            }

            fclose(fh);
            wxLogDebug("TAPE: Read file done!");
        }
        else
            wxLogDebug("TAPE: Read file error!");
    }
    else
        wxLogDebug("TAPE: Read file not set!");
}

void emuTapeUnit::ClearBuffer(void)
{
    // clear tape buffer
    for (int i = 0; i < MAX_BUFFER_SIZE; i++)
        m_tapeBuffer[i] = 0x00;

    m_tapeBuffPtr = 0;
    m_bufferSize = 0;
}
