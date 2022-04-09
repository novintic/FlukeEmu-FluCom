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

// Simple wrapper for wxSound and pulseAudio
// wxSound:    https://docs.wxwidgets.org/trunk/classwx_sound.html#ac63033709ec767b57602342a3c86f166
// PulseAudio: https://www.freedesktop.org/software/pulseaudio/doxygen/simple.html


#include "wxSndWrap.h"
// wxSndWrap
// Starts a thread with a connection to the pa server
// Loads WAV files
// Play sounds on trigger


wxSndWrap::wxSndWrap()
{
    m_paConnected = false;

    for(int ix = 0; ix < MAX_SND_SAMPLES; ix++)
    {
        m_samp[ix].fileLoaded = false;
#ifdef USE_WX_SOUND
        m_samp[ix].wxs = NULL;
#else
        m_samp[ix].sndDataPtr = NULL;
        m_samp[ix].sndDataSize = 0;
#endif
    }
}

wxSndWrap::~wxSndWrap()
{
#ifndef USE_WX_SOUND
    pa_simple_free(m_pPas);
#endif
    for(int ix = 0; ix < MAX_SND_SAMPLES; ix++)
    {
        if(m_samp[ix].fileLoaded)
        {
#ifdef USE_WX_SOUND
            delete(m_samp[ix].wxs);
#else
            free(m_samp[ix].sndDataPtr);
#endif
        }
    }
}

void wxSndWrap::init(void)
{
    openPACon();
}

bool wxSndWrap::openPACon(void)
{
#ifdef USE_WX_SOUND
    m_paConnected = true;
#else
    // setup pa
    m_Pass.format   = PA_SAMPLE_S16NE;
    m_Pass.channels = 2;
    m_Pass.rate     = 44100;
    // Setup sound server connection
    m_pPas = pa_simple_new(NULL,           // Use the default server.
                  "wxSndWrap",        // Our application's name.
                  PA_STREAM_PLAYBACK,
                  NULL,               // Use the default device.
                  "Sounds",           // Description of our stream.
                  &m_Pass,            // Our sample format.
                  NULL,               // Use default channel map
                  NULL,               // Use default buffering attributes.
                  NULL               // Ignore error code.
                );

    if(m_pPas)
    {
        wxLogDebug("WAV: Pulse audio connected");
        m_paConnected = true;
    }
#endif
    return m_paConnected;
}

int wxSndWrap::Load(int id, wxString fname)
{
#ifdef USE_WX_SOUND
    if(id < MAX_SND_SAMPLES)
    {
        m_samp[id].wxs = new wxSound(fname);
        if(m_samp[id].wxs->IsOk())
        {
            m_samp[id].fileLoaded = true;
            wxLogDebug("WAV: SAMPLE %d (%s) loaded", id, fname);
        }
    }
#else
    //wxLogDebug("WAV: File %s", fname);
    FILE *fp;
    fp = fopen(fname,"rb");
    if (fp)
    {
        WAVE_HEADER_t wHead;
        //read header
        fread((void*)&wHead, sizeof(WAVE_HEADER_t), 1, fp);
        // check header
        if((strncmp((char*)wHead.riff, "RIFF", 4) == 0) &&
           (strncmp((char*)wHead.wave, "WAVE", 4) == 0) ) //&&
         //  (strncmp((char*)wHead.fmt_chunk_marker, "fmt", 3) == 0) &&
         //  (wHead.format_type ==  1) )
        {   // its a PCM wav file
            if( (wHead.channels == 2) &&
                (wHead.sample_rate == 44100) &&
                (wHead.bits_per_sample == 16)   )
            {
                //wxLogDebug("WAV: FILE OK, size:%d", wHead.data_size);

                if(id < MAX_SND_SAMPLES)
                {
                    // allocate memory
                    void* pmem = malloc(wHead.data_size);
                    if(pmem)
                    {
                        int nr = fread(pmem, 1, wHead.data_size, fp);
                        //wxLogDebug("WAV: Data read %d == %d", nr, wHead.data_size);
                        if(nr == wHead.data_size)
                        {
                            m_samp[id].fileLoaded = true;
                            m_samp[id].sndDataPtr = (int8_t*)pmem;
                            m_samp[id].sndDataSize = wHead.data_size;
                            wxLogDebug("WAV: SAMPLE %d (%s) loaded", id, fname);
                        }
                        else
                            free(pmem);
                    }
                }
            }
            else
            {
                wxLogDebug("WAV: FILE Format not supported");
                wxLogDebug("WAV: CH:%d SR:%d BPS:%d", wHead.channels, wHead.sample_rate, wHead.bits_per_sample);
            }
        }
    }
#endif
    return 0;
}


bool wxSndWrap::Play(int id)
{
    if(m_paConnected)
    {
        if(m_samp[id].fileLoaded)
        {
            wxLogDebug("WAV: Playing sample %d", id);
            wxLongLong wxTmsS = wxGetLocalTimeMillis();
#ifdef USE_WX_SOUND
            m_samp[id].wxs->Play();
#else
            pa_simple_write(m_pPas, m_samp[id].sndDataPtr, m_samp[id].sndDataSize, NULL);
#endif
            wxLongLong wxTmsE = wxGetLocalTimeMillis();
            int64_t tms = wxTmsE.GetValue() - wxTmsS.GetValue();
            wxLogDebug("WAV: Play time: %d", (int)tms);
        }
        else
            wxLogDebug("WAV: NO sample %d", id);
    }
    else
        wxLogDebug("WAV: Play: Pulse audio not connected");
    return 0;
}



