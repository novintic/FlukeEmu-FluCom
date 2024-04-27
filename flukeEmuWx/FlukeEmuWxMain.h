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

#ifndef WXWFLUKEEMUMAIN_H
#define WXWFLUKEEMUMAIN_H

#include <wx/wx.h>

#include "FlukeEmuWxApp.h"
#include "EmuPanel.h"

class FlukeEmuWxFrame: public wxFrame
{
    public:
        FlukeEmuWxFrame(wxFrame *frame, const wxString& title, bool fullscreen);
        ~FlukeEmuWxFrame();

    private:
        EmuPanel* m_Emu;
        bool      m_fullScreen;
        wxString  m_portName;
        wxString  m_tapeFile;
        wxMenu*   m_settingsMenu;

        enum
        {
            idMenuQuit = 1000,
            idMenuAbout,
            idMenuFullScreen,
            idMenuTapeFile,
            idMenuSerialPort,
            idMenuSerialPortOpts,
            idMenuSerialPortInFile,
            idMenuSerialPortOutFile
        };
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnFullScreen(wxCommandEvent &event);
        void OnMenuTapeFile(wxCommandEvent& event);
        void OnMouseEvent(wxMouseEvent& event);
        void OnMenuSerPortOptions(wxCommandEvent& event);
        void OnMenuSerPortInFile(wxCommandEvent& event);
        void OnMenuSerPortOutFile(wxCommandEvent& event);
        DECLARE_EVENT_TABLE()
};


#endif // WXWFLUKEEMUMAIN_H
