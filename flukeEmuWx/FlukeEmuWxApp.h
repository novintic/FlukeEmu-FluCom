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

#ifndef WXWFLUKEEMUAPP_H
#define WXWFLUKEEMUAPP_H

#include <wx/app.h>
#include <wx/cmdline.h>

class FlukeEmuWxApp : public wxApp
{
    public:
        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

    private:
        bool fullscreen;
        double userEmuSpeedFactor = 1.0;

};

static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
    { wxCMD_LINE_SWITCH, "f", "fullscreen", "start fullscreen" },
    { wxCMD_LINE_OPTION, "s", "", "emulation speed factor (default 1.0)", wxCMD_LINE_VAL_DOUBLE },
    { wxCMD_LINE_NONE }
};

#endif // WXWFLUKEEMUAPP_H
