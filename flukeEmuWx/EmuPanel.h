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

#ifndef EMUFRAME_H
#define EMUFRAME_H

#include <wx/sound.h>
#include <wx/filename.h>
#include "EmuPanelDef.h"
#include "FlukeEmuHw.h"
#include "wxSndWrap.h"
#include <wx/dcbuffer.h>

#define RUNEMU_TIMER_ID     0x123

class EmuPanel: public wxWindow
{
    public:
        EmuPanel(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size);
        virtual ~EmuPanel();

        void OnPaint( wxPaintEvent &event );
        //void OnEraseBackground(wxEraseEvent& event);
        void OnMouseEvent(wxMouseEvent& event);
        void OnResize(wxSizeEvent& event);
        void OnRunEmu(wxTimerEvent& event);

        wxSize GetNativeSize();
        void ResizeElements(wxSize newSize);
        void DrawBackGnd(wxAutoBufferedPaintDC &dc);
        // Element drawing functions
        void DrawDisplay(wxAutoBufferedPaintDC &dc);
        void DrawLEDS(wxAutoBufferedPaintDC &dc);
        void DrawPwrButton(wxAutoBufferedPaintDC &dc);
        void DrawTapeDrive(wxAutoBufferedPaintDC &dc);
        void DrawActiveKeys(wxAutoBufferedPaintDC &dc);
        void DrawKeyOverlay(wxAutoBufferedPaintDC &dc);

        uint8_t GetKey(int x, int y, wxRect* pKr);
        wxRect GetKeyRect(uint8_t key);
        wxRect GetDispPanelRect(void) { return m_dispPanelRect; };
        wxRect GetKeyEmuSettingsRect(void) { return m_KeyRectEmuSettings; };
        void OnKeyEventDown(wxKeyEvent& event);
        void OnKeyEventUp(wxKeyEvent& event);

        void TapeFileDialog(void);
        void SerPortDialog(void);
        void SerInFileDialog(void);
        void SerOutFileDialog(void);
        void updateSerialStatus(void);

        int setSerialPort(wxString pname, int speed)
        { return m_emuHw.m_serPort.setSerPort(pname, speed); };

        wxRect scaleRect(wxRect rect, float ps, float ss);
    protected:


    private:
    flukeEmuHw  m_emuHw;
    wxTimer*    m_timer;
    //
    wxImage     m_bgndImage;
    wxBitmap*   m_bgndPanel;
    wxRect      m_bgndSide;
    wxSize      m_PanelSize;
    bool        m_PanelSizeUpd;
    float       m_PanelScale;

    // Keyboard
    int         m_keyMap[KEYCOLNUM][KEYROWNUM] = KEYMAP;
    wxRect      m_keybRect = KEYB_RECT;
    int         m_keybCols[COLTNUM][2] = COLS;
    int         m_keybRows[KEYCOLNUM][KEYROWNUM][2] = ROWS;
    const char* m_KeyTextMap[APLHA_KEYS_TEXT_NUM] = APLPHA_KEYS_TEXT;
    const char* m_KeyKeybMap[KEYBMAP_NUM] = KEYBMAP_TEXT;

    int         m_asciiKmap[ASCIIKEYMAPSIZE] = {ASCIIKEYMAP};
    int         m_otherKmap[OTHERKEYMAPSIZE][2] = {OTHERKEYMAP};
    wxRect      m_KeyRectDown;
    bool        m_keyDown;
    wxRect      m_KeyRectHiPulse;
    bool        m_PulseHikeyDown;
    wxRect      m_KeyRectLoPulse;
    bool        m_PulseLokeyDown;

    bool        m_ShowKeys;
    bool        m_ShowAlphaKeys;
    wxRect      m_KeyRectAplhaKeys;
    wxRect      m_KeyRectEmuSettings;
    wxRect      m_NativeKeyRectEmuSettings;

    // Font display
    wxRect      m_dispRect = DISPLAY_RECT;

    // Display + LED panel (used to show exit fullscreen menu)
    wxRect      m_dispPanelRect;
    wxRect      m_NativeDispPanelRect = DISPPANEL_RECT;

    // Char display
    wxFont*     m_dispFont;
    wxString    m_dispStr;
    wxBitmap*   m_charMap;
    wxImage     m_charOff;
    int         m_charMapWidth;
    int         m_charMapHeight;
    int         m_charWidth;
    int         m_charHeight;
    uint8_t     m_CharLUT[256];

    // LEDs
    int         m_LEDState;
    wxPoint     m_LEDPos[LEDS_NUM] = LEDSPOS;
    int         m_LEDMap[LEDS_NUM] = LED_MAP;
    // Probe LEDS
    bool        m_ProbeLEDHiState;
    bool        m_ProbeLEDLoState;
    wxRect      m_NativeProbeLEDHiRect;
    wxRect      m_NativeProbeLEDLoRect;
    wxRect      m_ProbeLEDHiRect;
    wxRect      m_ProbeLEDLoRect;
    // Power button
    wxRect      m_NativePwrRect;
    wxImage     m_pwrOff;
    wxImage     m_pwrOnt;
    wxImage     m_pwrOn;
    float       m_pwrNativeScale;
    wxRect      m_PwrRect;
    bool        m_PwrState;
    bool        m_PwrStateLast;
    bool        m_PwrButDown;

    // tape drive
    wxFont*     m_tapeFont;
    wxRect      m_NativeTapeDriveRect;
    float       m_TapeDriveNativeScale;
    wxRect      m_TapeDriveRect;
    wxFileName  m_tapeFileName;
    wxString    m_tapeFileDispName;
    bool        m_tapeFileSet;

    // Serial port file
    wxString    m_serPortDispName;
    wxString    m_serInFileDispName;
    wxString    m_serOutFileDispName;

    wxSndWrap   m_snd;

    wxDECLARE_ABSTRACT_CLASS(EmuPanel);
    wxDECLARE_EVENT_TABLE();
};


#endif // EMUFRAME_H
