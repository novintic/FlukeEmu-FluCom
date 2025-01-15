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

#include <wx/wx.h>
#include <wx/sound.h>

//#define USE_WX_SOUND      // disable to use pulse audio

// wxSound needs SDL dev linraries installed, when building wxWidgets
// sudo apt-get install libsdl2-dev
// Then rebuild wxWidgets (go to wxwidgest build-gtk, run configure --with-sdl, make)


#ifndef USE_WX_SOUND
#include <pulse/simple.h>
#include <pulse/error.h>
#include "wavdefs.h"
#endif // USE_WX_SOUND

#define MAX_SND_SAMPLES   16
typedef struct
{
    bool        fileLoaded;
#ifdef USE_WX_SOUND
    wxSound*    wxs;
#else
    int8_t*     sndDataPtr;
    uint32_t    sndDataSize;
#endif
}SNDSAMPLE_MGMT_t;

class wxSndWrap // : public wxThread
{
public:
    wxSndWrap();
    ~wxSndWrap();

    void init(void);
    int Load(int id, wxString fname);
    bool Play(int id, bool loop=false);
    void Stop(int id);

    bool openPACon(void);
    wxThread::ExitCode Entry();
protected:

#ifndef USE_WX_SOUND
    pa_simple *m_pPas;
    pa_sample_spec m_Pass;
#endif

    SNDSAMPLE_MGMT_t m_samp[MAX_SND_SAMPLES];
    bool             m_paConnected;
};

