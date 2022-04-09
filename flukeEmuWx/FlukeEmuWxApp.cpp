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

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "FlukeEmuWxApp.h"
#include "FlukeEmuWxMain.h"

wxIMPLEMENT_APP(FlukeEmuWxApp);

bool FlukeEmuWxApp::OnInit()
{
    // Log window
    //wxLogWindow* logWnd = new wxLogWindow(NULL, "FlukeEmuLog", true, false);
    //logWnd->SetActiveTarget(logWnd);
    FlukeEmuWxFrame* frame = new FlukeEmuWxFrame(0L, _("FlukeEmWx"));
    wxLogDebug("FlukeEmuWx LOG started\n");
    //frame->SetIcon(wxICON(aaaa)); // To Set App Icon
    frame->Show();
    return true;
}
