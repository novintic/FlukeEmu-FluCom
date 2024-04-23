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

// ********************************
// Emulator window
#include <dirent.h>
#include <errno.h>

#include "wx/wx.h"
#include "wx/image.h"
#include <wx/sound.h>
#include <wx/display.h>

#include "FlukeEmuWxMain.h"
#include "EmuPanel.h"

#define SND_ID_BEEP      0
#define SND_ID_BUTT      1
#define SND_ID_POWON1    2
#define SND_ID_POWON2    3
#define SND_ID_POWOFF1   4
#define SND_ID_POWOFF2   5


wxIMPLEMENT_CLASS(EmuPanel, wxWindow);

wxBEGIN_EVENT_TABLE(EmuPanel, wxWindow)
  EVT_PAINT(EmuPanel::OnPaint)
  EVT_SIZE(EmuPanel::OnResize)
  //EVT_ERASE_BACKGROUND(EmuPanel::OnEraseBackground)
  EVT_MOUSE_EVENTS(EmuPanel::OnMouseEvent)
  EVT_KEY_DOWN(EmuPanel::OnKeyEventDown)
  EVT_KEY_UP(EmuPanel::OnKeyEventUp)
  EVT_TIMER(RUNEMU_TIMER_ID, EmuPanel::OnRunEmu)
wxEND_EVENT_TABLE()

EmuPanel::EmuPanel(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
         :wxWindow( parent, id, pos, size, wxSUNKEN_BORDER )
{
    // Check media path
    DIR* mediaDir = opendir("media");
    if(mediaDir)
    {   // all ok, just close dir
        closedir(mediaDir);
    }
    else if (ENOENT == errno)
    {   // dir does not exists
        wxMessageBox (wxString::Format("Media directory not found!\nThe folder 'media' must be in the same folder as\nthe flukeEmuWx executable!"), wxString("Media Folder missing"));
        //throw std::domain_error("Media directory not found!");
        exit(EXIT_FAILURE);
    }
    else
    {   // error opening dir
        wxMessageBox (wxString::Format("Media directory not accessible!\nDo you have the correct permissions?"), wxString("Media Folder"));
        //throw std::domain_error("Media directory not accessible!");
        exit(EXIT_FAILURE);
    }

    // Load background bitmap
    if (m_bgndImage.LoadFile(F9010PANEL_BMP, wxBITMAP_TYPE_BMP))
    {
        wxLogDebug("BGND Image ok");
        m_bgndPanel = new wxBitmap(m_bgndImage);
    }
    else
    {
        wxMessageBox (wxString::Format("Panel image: %s not found!\nThe image must be in the folder 'media'!", F9010PANEL_BMP), wxString("Panel image missing"));
        //throw std::domain_error("Panel image not found!");
        exit(EXIT_FAILURE);
    }

    m_keyDown = false;
    m_PulseHikeyDown = false;
    m_PulseLokeyDown = false;
    m_KeyRectHiPulse = GetKeyRect(KEY_HIGH_DOWN);
    m_KeyRectLoPulse = GetKeyRect(KEY_LOW_DOWN);
    m_ShowKeys   = false;

    // Display
    m_dispFont = new wxFont(wxFontInfo(50).Family(wxFONTFAMILY_MODERN).Bold());
    // disp panel (used only for exit full screen menu)
    m_dispPanelRect = m_NativeDispPanelRect;

    SetBackgroundColour(* wxWHITE);
    SetCursor(wxCursor(wxCURSOR_ARROW));

    // Panel elements
    // LEDs
    m_LEDState = 0;
    // Probe LEDS
    wxRect kr = GetKeyRect(KEY_HIGH_DOWN);
    m_NativeProbeLEDHiRect.x = kr.x + kr.width/2 - LED_SIZE.x/2;
    m_NativeProbeLEDHiRect.y = kr.y + kr.height/2 - LED_SIZE.y/2 - kr.height/4;
    m_NativeProbeLEDHiRect.width = LED_SIZE.x;
    m_NativeProbeLEDHiRect.height = LED_SIZE.y;

    kr = GetKeyRect(KEY_LOW_DOWN);
    m_NativeProbeLEDLoRect.x = kr.x + kr.width/2 - LED_SIZE.x/2;
    m_NativeProbeLEDLoRect.y = kr.y + kr.height/2 - LED_SIZE.y/2  - kr.height/4;
    m_NativeProbeLEDLoRect.width = LED_SIZE.x;
    m_NativeProbeLEDLoRect.height = LED_SIZE.y;
    //m_NativeProbeLEDHiRect = GetKeyRect(KEY_HIGH_DOWN);
    //m_NativeProbeLEDLoRect = GetKeyRect(KEY_LOW_DOWN);
    m_ProbeLEDHiRect = m_NativeProbeLEDHiRect;
    m_ProbeLEDLoRect = m_NativeProbeLEDLoRect;

    // Power button
    m_NativePwrRect = POWER_BUT_RECT;  // Values aligned to background image
    m_pwrNativeScale = 1.0f;
    m_PwrRect   = m_NativePwrRect;
    m_PwrState  = false;
    m_PwrStateLast = false;
    m_PwrButDown= false;

    // tape drive
    m_NativeTapeDriveRect = TAPEDRIVE_RECT;  // Values aligned to background image
    m_TapeDriveNativeScale = 1.0f;
    m_TapeDriveRect   = m_NativeTapeDriveRect;
    m_tapeFileName = "";
    m_tapeFileSet = false;
    m_tapeFont = new wxFont(wxFontInfo(15).Family(wxFONTFAMILY_MODERN).Bold());

    // Serial port files
    m_serPortDispName = "/dev/serial0";
    m_serInFileDispName  = "";
    m_serOutFileDispName = "";


    // Sounds
    m_snd.init();
    m_snd.Load(SND_ID_BEEP, "media/beep.wav");
    m_snd.Load(SND_ID_BUTT, "media/keyclick.wav");

    m_snd.Load(SND_ID_POWON1, "media/pwr_offon1.wav");
    m_snd.Load(SND_ID_POWON2, "media/pwr_offon2.wav");
    m_snd.Load(SND_ID_POWOFF1, "media/pwr_onoff1.wav");
    m_snd.Load(SND_ID_POWOFF2, "media/pwr_onoff2.wav");

    // Set initial window size
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxDisplay display(wxDisplay::GetFromWindow(parent));
    wxRect screen = display.GetClientArea();
    wxRect bgsize = GetNativeSize();
    float rx = (float)bgsize.width  / (float)screen.width;
    float ry = (float)bgsize.height / (float)screen.height;
    float ra = rx > ry ? rx : ry;
    float rt = 0.9f;    // target maximum ratio
    wxLogDebug("Screen size: %d %d", screen.width, screen.height);
    if(ra > rt)
    {
        bgsize.width = bgsize.width * (rt / ra);
        bgsize.height = bgsize.height * (rt / ra);
    }
    wxLogDebug("Init size: %d %d", bgsize.width, bgsize.height);
    SetSize(bgsize); // This causes a onResize call
    //bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL)

    // setup timer for timed emulator calls
    m_timer = new wxTimer(this, RUNEMU_TIMER_ID);
    flukeEmuHwSetInst(&m_emuHw);
    m_timer->Start(10);    // 1 second interval
    wxLogDebug("EMU started");

    // Start emu thread
    if ( m_emuHw.Run() != wxTHREAD_NO_ERROR )
        wxLogError("Can't create ENU thread!");

}

EmuPanel::~EmuPanel()
{
}

wxRect EmuPanel::scaleRect(wxRect rect, float ps, float ss)
{
    wxRect srect = wxRect(rect.x * ps,
                          rect.y * ps,
                          rect.width * ss,
                          rect.height * ss);
    return srect;
}
// Check for any emu outputs
void EmuPanel::OnRunEmu(wxTimerEvent& event)
{
    if(m_PwrState)
    {
        // Get time to hand on to disp and led updates
        wxLongLong wxTms = wxGetLocalTimeMillis();
        int64_t tms = wxTms.GetValue();
        // read back information
        if(m_emuHw.m_dispKeyb.updDisp(tms))
        {
            char* pdisp = m_emuHw.m_dispKeyb.GetDispChars();
            char buf[DISP_NUMCHAR + 1];
            memcpy(buf, pdisp, DISP_NUMCHAR);
            buf[DISP_NUMCHAR] = 0;
            m_dispStr = wxString(buf);
            Refresh(true, NULL); // TEST DO BETTER
        }

        if(m_emuHw.m_dispKeyb.updLed(tms))
        {
            int ledStat = m_emuHw.m_dispKeyb.GetLedStatus();
            m_LEDState = ledStat;
            Refresh(true, NULL); // TEST DO BETTER
        }
        // Check beep sound
        if(m_emuHw.m_podProbe.beepF9010())
        {
            m_snd.Play(SND_ID_BEEP);
        }
        // Check probe LED
        bool hl = m_emuHw.m_podProbe.getProbeLEDHiState();
        bool ll = m_emuHw.m_podProbe.getProbeLEDLoState();
        bool hlchange = m_ProbeLEDHiState ^ hl;
        bool llchange = m_ProbeLEDLoState ^ ll;
        m_ProbeLEDHiState = hl;
        m_ProbeLEDLoState = ll;
        if(hlchange || llchange)
        {
            printf("Probe LEDs H:%d L:%d\n", m_ProbeLEDHiState, m_ProbeLEDLoState);
            Refresh(true, NULL); // TEST DO BETTER
         //   wxRect      m_ProbeLEDHiRect;
        //wxRect      m_ProbeLEDLoRect;
        //    RefreshRect(const wxRect& rect);
        }
    }
    // Update tape
    bool fset = m_emuHw.m_tapeUnit.getTapeFileSet();
    if(m_tapeFileSet != fset)
    {
        m_tapeFileSet = fset;
        Refresh(true, NULL); // TEST DO BETTER
    }
    // update serial port/file status
    updateSerialStatus();
}

wxSize EmuPanel::GetNativeSize()
{
    if(m_bgndImage.IsOk())
        return wxSize(m_bgndImage.GetWidth(), m_bgndImage.GetHeight());
    else
        return wxSize(500, 300);
}

void EmuPanel::OnPaint( wxPaintEvent &event )
{
    //wxPaintDC dc( this );
    wxAutoBufferedPaintDC dc( this );
    PrepareDC( dc );
    DrawBackGnd(dc);
    DrawDisplay(dc);    // Draw display
    DrawLEDS(dc);       // LEDS
    DrawPwrButton(dc);  // Power button
    DrawTapeDrive(dc);  // Draw tape drive
    DrawActiveKeys(dc); // Draw active keys
    DrawKeyOverlay(dc); // Draw key overlay
}

void EmuPanel::ResizeElements(wxSize newSize)
{
    //wxLogDebug("Scaling: %d  %d", sz.GetWidth(), sz.GetHeight());
    float bgrx = (float)newSize.x / (float)m_bgndImage.GetWidth();
    float bgry = (float)newSize.y / (float)m_bgndImage.GetHeight();
    m_PanelSize = newSize;
    m_PanelSizeUpd = true;
    bool xdim = bgrx < bgry;
    m_PanelScale = xdim ? bgrx : bgry;

    int wxs = m_bgndImage.GetWidth()*m_PanelScale;
    int wys = m_bgndImage.GetHeight()*m_PanelScale;

    //wxLogDebug("Panel scale: %.5f", m_PanelScale);
    // Update rectangles for mouse click detection

    m_bgndSide.width  = xdim ? newSize.x : newSize.x - wxs;
    m_bgndSide.height = xdim ? newSize.y - wys : newSize.y;
    m_bgndSide.x      = xdim ? 0 : wxs;
    m_bgndSide.y      = xdim ? wys : 0;

    // Power button
    m_PwrRect = scaleRect(m_NativePwrRect, m_PanelScale, m_pwrNativeScale * m_PanelScale);
    // Tapedrivr
    m_TapeDriveRect = scaleRect(m_NativeTapeDriveRect, m_PanelScale, m_TapeDriveNativeScale * m_PanelScale);
    // Disp panel
    m_dispPanelRect = scaleRect(m_NativeDispPanelRect, m_PanelScale, m_PanelScale);
    // Probe LEDS
    m_ProbeLEDHiRect = scaleRect(m_NativeProbeLEDHiRect, m_PanelScale, m_PanelScale);
    m_ProbeLEDLoRect = scaleRect(m_NativeProbeLEDLoRect, m_PanelScale, m_PanelScale);
}

void EmuPanel::OnResize(wxSizeEvent& event)
{
    // Recalculate scale
    wxSize sz = GetClientSize();
    //wxLogDebug("Scaling: %d  %d", sz.GetWidth(), sz.GetHeight());
    ResizeElements(sz);
    Refresh(true, NULL);
}

void EmuPanel::DrawBackGnd(wxAutoBufferedPaintDC &dc)
{
    if (m_bgndImage.IsOk())
    {
        // Scale?
        if(m_PanelSizeUpd)
        {
            //wxLogDebug("Scaling: %d  %d", sz.GetWidth(), sz.GetHeight());
           // ResizeElements(sz);
            int wxs = m_bgndImage.GetWidth()*m_PanelScale;
            int wys = m_bgndImage.GetHeight()*m_PanelScale;
            m_bgndPanel = new wxBitmap(m_bgndImage.Scale(wxs, wys, wxIMAGE_QUALITY_HIGH));
            m_PanelSizeUpd = false;
        }
        dc.DrawBitmap(*m_bgndPanel, 0, 0);
        dc.SetPen(*wxLIGHT_GREY_PEN);
        dc.SetBrush(*wxLIGHT_GREY_BRUSH);
        dc.DrawRectangle(m_bgndSide);

        //#define DRAW_KEYb_GRID
        #ifdef DRAW_KEYb_GRID
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(wxPen(wxColor(255,0,0), 1));
        wxRect kr;
        kr.x = KEYB_RECT.x*m_PanelScale;
        kr.y = KEYB_RECT.y*m_PanelScale;
        kr.width = KEYB_RECT.width*m_PanelScale;
        kr.height = KEYB_RECT.height*m_PanelScale;
        dc.DrawRectangle(kr);

        int ys = kr.y;
        int ye = (kr.y+kr.height);
        for(int c=0; c<COLNUM; c++ )
        {

            dc.DrawLine(m_keybCols[c][0]*m_PanelScale, ys, m_keybCols[c][0]*m_PanelScale, ye);
            dc.DrawLine(m_keybCols[c][1]*m_PanelScale, ys, m_keybCols[c][1]*m_PanelScale, ye);
            int xs = kr.x;
            int xe = (kr.x+kr.width);
            for(int r=0; r<ROWNUM; r++ )
            {
                dc.DrawLine(xs, m_keybRows[c][r][0]*m_PanelScale, xe, m_keybRows[c][r][0]*m_PanelScale);
                dc.DrawLine(xs, m_keybRows[c][r][1]*m_PanelScale, xe, m_keybRows[c][r][1]*m_PanelScale);
            }
        }

        #endif // DRAW_KEYb_GRID
    }
}

/*
void EmuPanel::OnEraseBackground(wxEraseEvent& event)
{
    if (m_bgndImage.IsOk())
    {
        // Scale?
        if(m_PanelSizeUpd)
        {
            //wxLogDebug("Scaling: %d  %d", sz.GetWidth(), sz.GetHeight());
           // ResizeElements(sz);
            int wxs = m_bgndImage.GetWidth()*m_PanelScale;
            int wys = m_bgndImage.GetHeight()*m_PanelScale;
            m_bgndPanel = new wxBitmap(m_bgndImage.Scale(wxs, wys, wxIMAGE_QUALITY_HIGH));
            m_PanelSizeUpd = false;
        }

        if (event.GetDC())
        {
            event.GetDC()->DrawBitmap(*m_bgndPanel, 0, 0);
            //event.GetDC()->SetPen(*wxRED_PEN);
            event.GetDC()->SetPen(*wxLIGHT_GREY_PEN);
            event.GetDC()->SetBrush(*wxLIGHT_GREY_BRUSH);
            event.GetDC()->DrawRectangle(m_bgndSide);
        }
        else
        {
            wxClientDC dc(this);
            dc.DrawBitmap(*m_bgndPanel, 0, 0);
            dc.SetPen(*wxLIGHT_GREY_PEN);
            dc.SetBrush(*wxLIGHT_GREY_BRUSH);
            dc.DrawRectangle(m_bgndSide);
        }
    }
    else
        event.Skip(); // The official way of doing it
}
*/

void EmuPanel::OnKeyEventDown(wxKeyEvent& event)
{
    int kf = KEY_NOKEY, k = event.GetKeyCode();
    // check ASCII codes
    if(k < ASCIIKEYMAPSIZE)
        kf = m_asciiKmap[k];
    // check other codes
    for(int i = 0; i < OTHERKEYMAPSIZE; i++)
    {
        if(k == m_otherKmap[i][0])
        {
            kf = m_otherKmap[i][1];
            break;
        }
    }

    if(kf != KEY_NOKEY)
    {
        m_snd.Play(1);
        // Handle toggle keys
        if(kf == KEY_HIGH_DOWN) // Key is always reported as DOWN
        {
            if(m_PulseHikeyDown) // If state is down, change key code
                kf = KEY_HIGH_UP;
            m_PulseHikeyDown = !m_PulseHikeyDown;
        }
        if(kf == KEY_LOW_DOWN) // Key is always reported as DOWN
        {
            if(m_PulseLokeyDown) // If state is down, change key code
                kf = KEY_LOW_UP;
            m_PulseLokeyDown = !m_PulseLokeyDown;
        }

        m_emuHw.m_dispKeyb.KeyPressed(kf);
        //notConsumed = false;
    }
    else
        event.Skip();

    int m = event.GetModifiers();
    if((m & wxMOD_ALT) > 0)
    {
        m_ShowKeys = true;
        //wxLogDebug("KEYD: ALT ");
    }
    wxLogDebug("KEYD: %d -> %d", k, kf );
}

void EmuPanel::OnKeyEventUp(wxKeyEvent& event)
{
    int m = event.GetModifiers();
    if((m & wxMOD_ALT) == 0)
    {
        m_ShowKeys = false;
        //wxLogDebug("KEYUP: ALT ");
    }
    //wxLogDebug("KEYUP: %d", m);
}

void EmuPanel::OnMouseEvent(wxMouseEvent& event)
{
    bool notConsumed = true;
    if (event.LeftDown())
    {
        //wxLogDebug("M DO: %d  %d\n", event.GetX(), event.GetY());
        // Power button
        if(m_PwrRect.Contains(event.GetX(), event.GetY()))
        {
            if(!m_PwrState)
            {
                m_emuHw.execCtrl(EMU_RESET);
                m_emuHw.execCtrl(EMU_RUN);
                m_PwrState = true;
                m_PwrStateLast = false;
                m_snd.Play(SND_ID_POWON1);
            }
            else
            {
                m_snd.Play(SND_ID_POWOFF1);
            }
            m_PwrButDown = true;
            Refresh(true, NULL); // TEST DO BETTER
            notConsumed = false;
        }
        else if(m_TapeDriveRect.Contains(event.GetX(), event.GetY()))
        {
            // Open cassette file dialog
            wxLogDebug("TAPE: insert cassette");
            TapeFileDialog();
            Refresh(true, NULL); // TEST DO BETTER
            notConsumed = false;
        }
        else if(m_keybRect.Contains(event.GetX()/m_PanelScale, event.GetY()/m_PanelScale))
        {
            wxRect keyRect;
            uint8_t key = GetKey(event.GetX()/m_PanelScale, event.GetY()/m_PanelScale, &keyRect);
            if(key != 255)
            {
                m_snd.Play(1);
                m_KeyRectDown = keyRect;
                if(key == KEY_HIGH_DOWN) // Key is always reported as DOWN
                {
                    if(m_PulseHikeyDown) // If state is down, change key code
                        key = KEY_HIGH_UP;
                    m_PulseHikeyDown = !m_PulseHikeyDown;
                }
                if(key == KEY_LOW_DOWN) // Key is always reported as DOWN
                {
                    if(m_PulseLokeyDown) // If state is down, change key code
                        key = KEY_LOW_UP;
                    m_PulseLokeyDown = !m_PulseLokeyDown;
                }
                else
                    m_keyDown = true;

                m_emuHw.m_dispKeyb.KeyPressed(key);

                notConsumed = false;
            }
        }
    }
    if (event.LeftUp())
    {
        //wxLogDebug("M UP: %d  %d\n", event.GetX(), event.GetY());
        // Power button
        if(m_PwrRect.Contains(event.GetX(), event.GetY()))
        {
            if(m_PwrState && !m_PwrStateLast)
            {
                m_snd.Play(SND_ID_POWON2);
                m_PwrStateLast = true;
            }
            else
            {
                m_PwrState = false;
                m_emuHw.execCtrl(EMU_PAUSE);
                m_snd.Play(SND_ID_POWOFF2);
            }
            Refresh(true, NULL); // TEST DO BETTER
            Refresh(false, &m_PwrRect);
            m_PwrButDown = false;
            wxLogDebug("PWR: %s\n", m_PwrState ? "on":"off");
            notConsumed = false;
        }
        else if(m_PwrButDown)
        {

        }

        m_keyDown = false;

    }

    if(notConsumed)
        event.Skip(); // The official way of doing it
}


// Element drawing functions
void EmuPanel::DrawDisplay(wxAutoBufferedPaintDC &dc)
{
    char* pdisp = m_emuHw.m_dispKeyb.GetDispChars();
    char buf[DISP_NUMCHAR + 1];
    memcpy(buf, pdisp, DISP_NUMCHAR);
    buf[DISP_NUMCHAR] = 0;
    m_dispStr = wxString(buf);

    if(m_PwrState)
    {
        m_dispFont->SetPixelSize(wxSize(m_dispRect.width/DISP_NUMCHAR*m_PanelScale, m_dispRect.height*m_PanelScale));
        dc.SetFont(*m_dispFont);
        dc.SetTextForeground (wxColour(DISPLAY_COLOR));
        wxSize ds = dc.GetTextExtent(m_dispStr);
        wxCoord dx = m_dispRect.x*m_PanelScale;
        wxCoord dy = (m_dispRect.y + m_dispRect.height/2)*m_PanelScale - ds.y/2;
        dc.DrawText(m_dispStr, dx, dy);
    }
}

void EmuPanel::DrawLEDS(wxAutoBufferedPaintDC &dc)
{
    if(m_PwrState)
    {
        dc.SetPen(*wxGREY_PEN);
        for(int lix = 0; lix < LEDS_NUM; lix++)
        {
            if(m_LEDState & m_LEDMap[lix])
                dc.SetBrush(*wxBLUE_BRUSH);
            else
                dc.SetBrush(*wxBLACK_BRUSH);
            dc.DrawRoundedRectangle(m_LEDPos[lix]*m_PanelScale, LED_SIZE*m_PanelScale, LED_CORNR*m_PanelScale);
        }
        // Probe LEDS
        if(m_ProbeLEDHiState)
            dc.SetBrush(*wxRED_BRUSH);
        else
            dc.SetBrush(*wxBLACK_BRUSH);
        dc.DrawRoundedRectangle(m_ProbeLEDHiRect, LED_CORNR*m_PanelScale);

        if(m_ProbeLEDLoState)
            dc.SetBrush(*wxGREEN_BRUSH);
        else
            dc.SetBrush(*wxBLACK_BRUSH);
        dc.DrawRoundedRectangle(m_ProbeLEDLoRect, LED_CORNR*m_PanelScale);
    }
}

void EmuPanel::DrawPwrButton(wxAutoBufferedPaintDC &dc)
{
    if(m_PwrState)
    {
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        int pw = 5*m_PanelScale < 1 ? 1 : 10*m_PanelScale;
        wxPen penOn = wxPen(POWER_BUT_ACT_COLOR, pw);
        dc.SetPen(penOn);
        dc.DrawRoundedRectangle(m_PwrRect, POWER_BUT_CORNR*m_PanelScale);
    }
}

void EmuPanel::DrawTapeDrive(wxAutoBufferedPaintDC &dc)
{
    if(m_tapeFileSet)
    {
        dc.SetBrush(*wxBLACK_BRUSH);
        int pw = 5*m_PanelScale < 1 ? 1 : 5*m_PanelScale;
        wxPen penOn = wxPen(wxColour(100, 100, 100), pw);
        dc.SetPen(penOn);
        wxRect rect2 = wxRect(m_TapeDriveRect.x + 0.15*m_TapeDriveRect.width,
                              m_TapeDriveRect.y - 0.1*m_TapeDriveRect.height,
                              m_TapeDriveRect.width*0.7,
                              m_TapeDriveRect.height*1.23);
        wxRect rectT = wxRect(m_TapeDriveRect.x + pw + 2,
                              m_TapeDriveRect.y + pw + 2,
                              m_TapeDriveRect.width - 2*(pw + 2),
                              m_TapeDriveRect.height - 2*(pw + 2));
        dc.DrawRoundedRectangle(rect2, TAPEDRIVE_CORNR*m_PanelScale);
        dc.DrawRoundedRectangle(m_TapeDriveRect, TAPEDRIVE_CORNR*m_PanelScale);

        dc.SetBrush(wxBrush(wxColour(200, 200, 210)));
        //penOn = wxPen(wxColour(200, 200, 210), pw);
        dc.SetPen(wxNullPen);
        dc.DrawRectangle(rectT);

        int ppch = rectT.height*m_PanelScale;
        if(ppch >= 5) // draw if font has reasonable size
        {
            int sl = m_tapeFileDispName.Len();
            int ppcw = rectT.width*m_PanelScale / (float)sl;
            // drop some letters ?
            sl = ppcw < 7 ? rectT.width*m_PanelScale / 7 : sl;

            m_tapeFont->SetPixelSize(wxSize(ppcw, ppch));
            dc.SetFont(*m_tapeFont);
            dc.SetTextForeground (wxColour(TAPETEXT_COLOR));
            dc.DrawLabel(m_tapeFileDispName.Mid(0, sl), rectT, wxALIGN_CENTER, -1);
        }
    }
}

void EmuPanel::DrawActiveKeys(wxAutoBufferedPaintDC &dc)
{
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    int pw = 5*m_PanelScale < 1 ? 1 : 10*m_PanelScale;
    if(m_keyDown)
    {
        wxRect srect = scaleRect(m_KeyRectDown, m_PanelScale, m_PanelScale);
        wxPen penOn = wxPen(KEY_ACT_COLOR, pw);
        dc.SetPen(penOn);
        dc.DrawRoundedRectangle(srect, POWER_BUT_CORNR*m_PanelScale);
    }
    // Pulse hi  key
    if(m_PulseHikeyDown)
    {
        wxRect srect = scaleRect(m_KeyRectHiPulse, m_PanelScale, m_PanelScale);
        dc.SetPen(wxPen(KEY_HIPUL_ACT_COLOR, pw));
        dc.DrawRoundedRectangle(srect, POWER_BUT_CORNR*m_PanelScale);
    }
    // Pulse Lo  key
    if(m_PulseLokeyDown)
    {
        wxRect srect = scaleRect(m_KeyRectLoPulse, m_PanelScale, m_PanelScale);
        dc.SetPen(wxPen(KEY_LOPUL_ACT_COLOR, pw));
        dc.DrawRoundedRectangle(srect, POWER_BUT_CORNR*m_PanelScale);
    }
}

void EmuPanel::DrawKeyOverlay(wxAutoBufferedPaintDC &dc)
{
    if(!m_ShowKeys)
        return;

    dc.SetBrush(*wxWHITE_BRUSH);
    int pw = 5*m_PanelScale < 1 ? 1 : 10*m_PanelScale;
    dc.SetPen(wxPen(wxColour(150, 150, 150), pw));
    dc.SetTextForeground (wxColour(TAPETEXT_COLOR));

    for(int cix = 0; cix < KEYCOLNUM; cix++)
    {
        for(int rix = 0; rix < KEYROWNUM; rix++)
        {
            int key = m_keyMap[cix][rix];
            if(key != KEY_NOKEY)
            {
                wxRect keyRect = {0,0,0,0};
                keyRect.x      = cix == 0 ? KEYBBEGX : m_keybCols[cix-1][1];
                keyRect.width  = m_keybCols[cix][0] - keyRect.x;
                keyRect.y      = m_keybRows[cix][rix][0];
                keyRect.height = m_keybRows[cix][rix][1] - m_keybRows[cix][rix][0];
                // Cosmetics
                keyRect.width += 5;
                keyRect.height += 5;
                wxRect srect = scaleRect(keyRect, m_PanelScale, m_PanelScale);
                dc.DrawRoundedRectangle(srect, POWER_BUT_CORNR*m_PanelScale);

                wxString ks = key < KEYS_TEXT_NUM ? m_KeyTextMap[key] : "";
                if(ks.length() > 0)
                {
                    int ppch = ks.Length() == 1 ? srect.width*0.9 : srect.width/ks.Length()*1.8;
                    m_tapeFont->SetPixelSize(wxSize(0, ppch));
                    dc.SetFont(*m_tapeFont);
                    dc.DrawLabel(ks, srect, wxALIGN_CENTER, -1);
                }
            }
        }
    }
}

// Keyboard
uint8_t EmuPanel::GetKey(int x, int y, wxRect* pKr)
{
    int col = -1;
    int row = -1;
    uint8_t key = KEY_NOKEY;
    // find column
    if((x >= KEYBBEGX) && (x <= KEYBENDX))
    {
        int bmax = (int)ceil(log2(COLTNUM));
        int ix = (1 << (bmax - 1)) - 1;
        int step = 1 << (bmax - 2);
        col = 8;
        for(int b = 0; b < bmax; b++)
        {
            //wxLogDebug("KEYB: %d S:%d  L:%d x:%d H:%d ", ix, step, m_keybCols[ix][0],x, m_keybCols[ix][1]);
            if(x < m_keybCols[ix][0])
            {
               ix -= step;
               col -= step > 0 ? step : 1;
            }
            else if(x > m_keybCols[ix][1])
            {
               ix += step;
               col += step;
            }
            else    // no  col
            {
                ix = -1;
                col = -1;
                break;
            }
            //wxLogDebug("KEYB: C:%d", col);
            step = step >> 1;
        }
        //wxLogDebug("KEYB COL: %d", col);
        if((col >= 0) && (col < KEYCOLNUM))
        {   // found a valid column
            for(int rix = 0; rix < KEYROWNUM; rix++)
            {
                if((y > m_keybRows[col][rix][0]) &&
                   (y < m_keybRows[col][rix][1]) )
                {
                    key = m_keyMap[col][rix];
                    wxLogDebug("KEYB COL: %d ROQ: %d -> K:%d", col, rix, key);
                    if(pKr)
                    {
                        pKr->x = col == 0 ? KEYBBEGX : m_keybCols[col-1][1];
                        pKr->width = m_keybCols[col][0] - pKr->x;
                        pKr->y = m_keybRows[col][rix][0];
                        pKr->height = m_keybRows[col][rix][1] - m_keybRows[col][rix][0];
                        // Cosmetics
                        pKr->width += 5;
                        pKr->height += 5;

                    }
                    //wxLogDebug("KEYB COL: %d ROQ: %d -> K:%d", col, rix, key);
                }
            }
        }
    }
    return key;
}

wxRect EmuPanel::GetKeyRect(uint8_t key)
{
    // search key in map
    bool keyFound = false;
    wxRect keyRect = {0,0,0,0};
    for(int cix = 0; cix < KEYCOLNUM; cix++)
    {
        for(int rix = 0; rix < KEYROWNUM; rix++)
        {
            keyFound = (key == m_keyMap[cix][rix]);
            if(keyFound)
            {
                keyRect.x      = cix == 0 ? KEYBBEGX : m_keybCols[cix-1][1];
                keyRect.width  = m_keybCols[cix][0] - keyRect.x;
                keyRect.y      = m_keybRows[cix][rix][0];
                keyRect.height = m_keybRows[cix][rix][1] - m_keybRows[cix][rix][0];
                // Cosmetics
                keyRect.width += 5;
                keyRect.height += 5;
                break;
            }
        }
        if(keyFound)
            break;
    }
    return keyRect;
}

void EmuPanel::TapeFileDialog(void)
{
    wxFileDialog openFileDialog(this, _("Open cassette file"), "", "DefaultName",
                   "Cassette files (*.txt)|*.txt|All files|*.*", wxFD_OPEN);

    int ret =  openFileDialog.ShowModal();
    if (ret != wxID_CANCEL)
    {
        m_tapeFileName = openFileDialog.GetPath();
        m_tapeFileDispName = m_tapeFileName.GetFullName();
        //m_tapeFileDispName = wxString::Format("%s.%s", m_tapeFileName.GetName(), m_tapeFileName.GetExt());
        bool fexists = m_tapeFileName.FileExists();
        bool isRead = m_tapeFileName.IsFileReadable();
        bool isWrite = m_tapeFileName.IsFileWritable();
        wxLogDebug("TAPE: cas file: %s / %s %c %c %c", openFileDialog.GetFilename(), m_tapeFileDispName, fexists ? 'E':'-', isRead ? 'R':'-', isWrite ? 'W':'-');
        m_emuHw.m_tapeUnit.setTapeFileName(m_tapeFileName.GetFullPath());
        ((FlukeEmuWxFrame*)GetParent())->GetStatusBar()->SetStatusText(_("Tape file: ") + m_tapeFileName.GetFullPath(), 0);
    }
    else if (ret == wxID_CANCEL)
    {   // remove cassette
        m_emuHw.m_tapeUnit.setTapeFileName("");
        ((FlukeEmuWxFrame*)GetParent())->GetStatusBar()->SetStatusText(_("Tape file: ") + "<none>", 0);
    }
}

void EmuPanel::SerPortDialog(void)
{
    wxString pName = wxGetTextFromUser(wxString("Port Name"), wxString("Serial Port"), m_serPortDispName);

    if(setSerialPort(pName, 9600) != 0)
    {
        //SetStatusText(_("Serial port: ") + (pName == "" ? "<none>" : pName) + " [8N1 9600]", 1);
        m_serPortDispName = pName;
    }
    else
    {
        wxMessageBox (wxString::Format("Failed to open port: %s", pName), wxString("Serial Port"));
        //SetStatusText(_("Serial port: ") + "<none>", 1);
        m_serPortDispName = "";
    }
    updateSerialStatus(); // update serial status
}

void EmuPanel::SerInFileDialog(void)
{
    wxFileDialog openFileDialog(this, _("Open serial input file"), "", "DefaultName",
                   "All files (*.txt)|*.*|All files|*.*", wxFD_OPEN);

    int ret =  openFileDialog.ShowModal();
    if (ret != wxID_CANCEL)
    {
        wxFileName fn = openFileDialog.GetPath();
        //bool isWrite = fn.IsFileWritable();
        //if(isWrite)
        {
            if(m_emuHw.m_serPort.openInFile(openFileDialog.GetPath()))
            {
                m_serInFileDispName = openFileDialog.GetPath();
            }
            else
            {
                m_serInFileDispName = "";
                wxLogDebug("SER OUT FILE: opening failed!");
            }
        }
        //else
        //    wxLogDebug("SER OUT FILE: %s write protected", openFileDialog.GetFilename() );
    }
    else if (ret == wxID_CANCEL)
    {   // close any open files
        m_emuHw.m_serPort.openInFile("");
    }
    //updateSerialStatus(); // update serial status
}

void EmuPanel::SerOutFileDialog(void)
{
    wxFileDialog openFileDialog(this, _("Open serial output file"), "", "DefaultName",
                   "All files (*.txt)|*.*|All files|*.*", wxFD_SAVE);

    int ret =  openFileDialog.ShowModal();
    if (ret != wxID_CANCEL)
    {
        wxFileName fn = openFileDialog.GetPath();
        //bool isWrite = fn.IsFileWritable();
        //if(isWrite)
        {
            if(m_emuHw.m_serPort.openOutFile(openFileDialog.GetPath()))
            {
                m_serOutFileDispName = openFileDialog.GetPath();
            }
            else
            {
                m_serOutFileDispName = "";
                wxLogDebug("SER OUT FILE: opening failed!");
            }
        }
        //else
        //    wxLogDebug("SER OUT FILE: %s write protected", openFileDialog.GetFilename() );
    }
    else if (ret == wxID_CANCEL)
    {   // close any open files
        m_emuHw.m_serPort.openOutFile("");
    }
    //updateSerialStatus(); // update serial status
}

void EmuPanel::updateSerialStatus(void)
{
    int sm = m_emuHw.m_serPort.getMode();
    if(sm & SER_UPD_FLAG)
    {
        wxString serStat;
        if((sm & SER_MODE_MASK) == 0) // port mode?
        {
            if(sm & SER_PORT_OPEN)
                serStat = wxString::Format("Serial port: %s : [8N1 9600]", m_serPortDispName);
            else
                serStat = wxString::Format("Serial port: <none>");
        }
        else
        {
            wxString istr = sm & SER_INFILE_OPEN ? m_serInFileDispName : "-";
            wxString ostr = sm & SER_OUTFILE_OPEN ? m_serOutFileDispName : "-";
            serStat = wxString::Format("Serial file: IN:%s / OUT: %s ", istr, ostr);
        }
        wxLogDebug("SERSTAT: %s", serStat);
        ((FlukeEmuWxFrame*)GetParent())->GetStatusBar()->SetStatusText(serStat, 1);
    }
}
