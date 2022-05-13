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

/******************
Building wxwidgets on windows:
* Make sure to set path in cmd shell to the correct compiler!
* Use option TARGET_CBU=X64  (otherwise you might get incompatible libs, which are skipped)
    D:\Program\wxWidgets-3.1.5\build\msw> mingw32-make -f makefile.gcc SHARED=0 BUILD=debug MONOLITHIC=0 TARGET_CPU=X64
* Use the SAME compiler to build the project, make sure is 86-X64

WIndwos libs:
kernel32
user32
gdi32
comdlg32
winspool
winmm
shell32
comctl32
ole32
oleaut32
uuid
rpcrt4
advapi32
wsock32
wininet
*******************/


#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "FlukeEmuWxMain.h"
#include "EmuPanel.h"

#define VER_STRING  "0.94"

//helper functions
wxString flukeEmuWxBuildinfo(void)
{
    return wxString("FlukeEmuWx " VER_STRING  " May 2022\n\nBy Clemens / (Novintic)");
}

enum wxbuildinfoformat { short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

BEGIN_EVENT_TABLE(FlukeEmuWxFrame, wxFrame)
    EVT_CLOSE(FlukeEmuWxFrame::OnClose)
    EVT_MENU(idMenuQuit, FlukeEmuWxFrame::OnQuit)
    EVT_MENU(idMenuAbout, FlukeEmuWxFrame::OnAbout)
    EVT_MENU(idMenuFullScreen, FlukeEmuWxFrame::OnFullScreen)
    EVT_MENU(idMenuTapeFile, FlukeEmuWxFrame::OnMenuTapeFile)
    EVT_MENU(idMenuSerialPortOpts, FlukeEmuWxFrame::OnMenuSerPortOptions)
    EVT_MENU(idMenuSerialPortInFile, FlukeEmuWxFrame::OnMenuSerPortInFile)
    EVT_MENU(idMenuSerialPortOutFile, FlukeEmuWxFrame::OnMenuSerPortOutFile)
    EVT_MOUSE_EVENTS(FlukeEmuWxFrame::OnMouseEvent)
END_EVENT_TABLE()

FlukeEmuWxFrame::FlukeEmuWxFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    // File menu
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit FlukeEmuWx"));
    mbar->Append(fileMenu, _("&File"));
    // Settings menu
    wxMenu* settingsMenu = new wxMenu(_T(""));
    settingsMenu->Append(idMenuFullScreen, _("&Fullscreen\tF5"), _("Switch to fullscreen"));
    settingsMenu->Append(idMenuTapeFile, _("&Tape file"), _("Set tape file"));
    // serial port menu
    wxMenu* serPortMenu = new wxMenu(_T(""));
    serPortMenu->Append(idMenuSerialPortOpts, _("&Set port"), _("Serial port settings"));
    serPortMenu->Append(idMenuSerialPortInFile, _("&Send file"), _("Send file to emulator"));
    serPortMenu->Append(idMenuSerialPortOutFile, _("&Receive file"), _("Receive file from emulator"));
    // attach popup menu
    settingsMenu->Append(idMenuSerialPort, _("&Serial port"), serPortMenu, _("Serial port options"));
    //settingsMenu->Append(idMenuSerialPort, _("&Serial Port"), _("Serial Port"));

    mbar->Append(settingsMenu, _("&Settings"));
    // Help menu
    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about FlukeEmuWx"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);

    m_portName = "/dev/serial0";
    m_tapeFile = "";

    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(3);
    SetStatusText(_("Tape file: ") + "<none>", 0);
    SetStatusText(_("Serial port: ") + "<none>", 1);
    SetStatusText( "FlukeEmuWx " VER_STRING " using " + wxbuildinfo(short_f), 2);

    m_Emu = new EmuPanel( this, wxID_ANY, wxPoint(0,0), wxSize(10,10));
    m_Emu->Bind(wxEVT_LEFT_DOWN, &FlukeEmuWxFrame::OnMouseEvent, this);

    Fit();
    m_fullScreen = false;
}


FlukeEmuWxFrame::~FlukeEmuWxFrame()
{
}

void FlukeEmuWxFrame::OnFullScreen(wxCommandEvent &event)
{
    wxLogDebug("Menu: FS");
    m_fullScreen = !IsFullScreen();
    ShowFullScreen(m_fullScreen, wxFULLSCREEN_ALL);
}

void FlukeEmuWxFrame::OnMenuTapeFile(wxCommandEvent& event)
{
    m_Emu->TapeFileDialog();
}

void FlukeEmuWxFrame::OnMenuSerPortOptions(wxCommandEvent& event)
{
    wxLogDebug("Menu: SerPort");
    m_Emu->SerPortDialog();
/*
    wxString pName = wxGetTextFromUser(wxString("Port Name"), wxString("Serial Port"), m_portName);
    //wxLogDebug("SerPort: %s", pName);

    if(m_Emu->setSerialPort(pName, 9600) != 0)
    {
        SetStatusText(_("Serial port: ") + (pName == "" ? "<none>" : pName) + " [8N1 9600]", 1);
        m_portName = pName;
    }
    else
    {
        wxMessageBox (wxString::Format("Failed to open port: %s", pName), wxString("Serial Port"));
        SetStatusText(_("Serial port: ") + "<none>", 1);
        m_portName = "";
    }
*/
}

void FlukeEmuWxFrame::OnMenuSerPortInFile(wxCommandEvent& event)
{
    wxLogDebug("Menu: Ser In File");
    m_Emu->SerInFileDialog();
}

void FlukeEmuWxFrame::OnMenuSerPortOutFile(wxCommandEvent& event)
{
    wxLogDebug("Menu: Ser Out File");
    m_Emu->SerOutFileDialog();
}

void FlukeEmuWxFrame::OnMouseEvent(wxMouseEvent& event)
{
    bool notConsumed = true;
    //wxLogDebug("MS EV");
    if (event.LeftDown())
    {
        wxLogDebug("Menu: LD");
        wxRect dp = m_Emu->GetDispPanelRect();
        if(dp.Contains(event.GetPosition()))
        {
            // Show popupmenu at position
            wxMenu menu(wxT("Fullscreen"));
            menu.Append(idMenuFullScreen, wxT("&Enter/Exit\tF5"));
            PopupMenu(&menu, event.GetPosition());
            notConsumed = false;
        }
    }

    if(notConsumed)
        event.Skip(); // The official way of doing it

}

void FlukeEmuWxFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void FlukeEmuWxFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void FlukeEmuWxFrame::OnAbout(wxCommandEvent &event)
{
    wxString msg = flukeEmuWxBuildinfo();
    msg << _T("\n\n");
    msg << wxbuildinfo(long_f);
    wxMessageBox(msg, _("FlukeEmuWx " VER_STRING));
}
