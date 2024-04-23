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

#ifndef EMUPANELDEF_H_INCLUDED
#define EMUPANELDEF_H_INCLUDED


// Key definitions (Source FlukeEm by AAdam Courchesne (@onecircuit))
#define KEY_0              0
#define KEY_1              1
#define KEY_2              2
#define KEY_3              3
#define KEY_4              4
#define KEY_5              5
#define KEY_6              6
#define KEY_7              7
#define KEY_8              8
#define KEY_9              9
#define KEY_A             10
#define KEY_B             11
#define KEY_C             12
#define KEY_D             13
#define KEY_E             14
#define KEY_F             15
#define KEY_LEARN         16    // (
#define KEY_IO_VIEW       18    // %
#define KEY_RAM_VIEW      17    // *
#define KEY_ROM_VIEW      19    // #
#define KEY_AUTO          20    // backslash
#define KEY_BUS_TEST      21    // "
#define KEY_ROM_TEST      22    // $
#define KEY_RAMLONG_TEST  23    // /
#define KEY_RAMSHORT_TEST 24
#define KEY_IO_TEST       25
#define KEY_PRIOR         26
#define KEY_MORE          27
#define KEY_ENTER_YES     28
#define KEY_CLEAR_NO      29
#define KEY_STS_CTL       30    //' '
#define KEY_READ_TS       31    // G
#define KEY_WRITE_TS      32    // H
#define KEY_RAMP          33    // L
#define KEY_WALK          34    // M
#define KEY_TOGGL_ADDR    35    // Q
#define KEY_TOGGL_DATA    36    // R
#define KEY_CONT          37    // V
#define KEY_REPEAT        38    // ;
#define KEY_LOOP          39    // ,
#define KEY_STOP          40    // W
#define KEY_RUN_UUT       41    // @
#define KEY_PROGRAM       42    // I
#define KEY_LABEL         43    // T
#define KEY_GOTO          44    // U
#define KEY_IF            45    // N
#define KEY_GR_THAN       46    // O
#define KEY_EQUALS        47    // P
#define KEY_AND           48    // X
#define KEY_OR            49    // =
#define KEY_SHIFT_LEFT    50    // Y
#define KEY_SHIFT_RIGHT   51    // <
#define KEY_INCR          52    // Z
#define KEY_DECR          53    // >
#define KEY_COMPL         54    // -
#define KEY_EXEC          55    // J
#define KEY_REG           56    // +
#define KEY_READ_PROBE    57    // ?
#define KEY_READ_TAPE     58
#define KEY_WRITE_TAPE    59
#define KEY_SYNC          60
#define KEY_SETUP         61    // .
#define KEY_DISPL         62    // S
#define KEY_AUX_IF        63    // K

#define KEY_HIGH_DOWN     64
#define KEY_HIGH_UP       65

#define KEY_LOW_DOWN      66
#define KEY_LOW_UP        67

#define KEY_NOKEY         255

#define ROW00KEYS   {KEY_LEARN,    KEY_RAM_VIEW,   KEY_NOKEY,      KEY_AUTO,         KEY_BUS_TEST,      KEY_ROM_TEST}
#define ROW01KEYS   {KEY_IO_VIEW,  KEY_ROM_VIEW,   KEY_NOKEY,      KEY_RAMLONG_TEST, KEY_RAMSHORT_TEST, KEY_IO_TEST}
#define ROW02KEYS   {KEY_NOKEY,    KEY_C,          KEY_8,          KEY_4,            KEY_0,             KEY_ENTER_YES}
#define ROW03KEYS   {KEY_NOKEY,    KEY_D,          KEY_9,          KEY_5,            KEY_1,             KEY_CLEAR_NO}
#define ROW04KEYS   {KEY_NOKEY,    KEY_E,          KEY_A,          KEY_6,            KEY_2,             KEY_PRIOR}
#define ROW05KEYS   {KEY_STS_CTL,  KEY_F,          KEY_B,          KEY_7,            KEY_3,             KEY_MORE}
#define ROW06KEYS   {KEY_READ_TS,  KEY_RAMP,       KEY_TOGGL_ADDR, KEY_CONT,         KEY_REPEAT,        KEY_LOOP}
#define ROW07KEYS   {KEY_WRITE_TS, KEY_WALK,       KEY_TOGGL_DATA, KEY_STOP,         KEY_RUN_UUT,       KEY_SETUP}
#define ROW08KEYS   {KEY_PROGRAM,  KEY_IF,         KEY_DISPL,      KEY_AND,          KEY_OR,            KEY_READ_PROBE}
#define ROW09KEYS   {KEY_EXEC,     KEY_GR_THAN,    KEY_LABEL,      KEY_SHIFT_LEFT,   KEY_SHIFT_RIGHT,   KEY_REG}
#define ROW10KEYS   {KEY_AUX_IF,   KEY_EQUALS,     KEY_GOTO,       KEY_INCR,         KEY_DECR,          KEY_COMPL}
#define ROW11KEYS   {KEY_READ_TAPE,KEY_WRITE_TAPE, KEY_SYNC,       KEY_HIGH_DOWN,    KEY_LOW_DOWN,      KEY_NOKEY}

#define KEYMAP  {ROW00KEYS,ROW01KEYS,ROW02KEYS,ROW03KEYS,ROW04KEYS,ROW05KEYS,ROW06KEYS,ROW07KEYS,ROW08KEYS,ROW09KEYS,ROW10KEYS,ROW11KEYS}

#define KEYCOLNUM   12
#define KEYROWNUM   6

#define LEDS_NUM      5
#define LED_PROGRAMMING 0x02
#define LED_STOPPED     0x04
#define LED_LOOPING     0x08
#define LED_MORE        0x10
#define LED_EXECUTING   0x01    //???

#define LED_MAP {LED_MORE, LED_LOOPING, LED_STOPPED, LED_PROGRAMMING, LED_EXECUTING}

// Text for keys
#define KEYS_TEXT {\
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",   /*  0 -  9*/ \
"A", "B", "C", "D", "E", "F", "(", "*", "%", "#",   /* 10 - 19*/ \
"\\","\"","$", "/", "F2","F4","Left", "Right", "Enter", "ESC",    /* 20 - 29*/ \
"' '", "G", "H", "L", "M", "Q", "R", "V", ";", ",",   /* 30 - 39*/ \
"W", "@", "I", "T", "U", "N", "O", "P", "X", "=",     /* 40 - 49*/ \
"Y", "<", "Z", ">", "-", "J", "+", "?", "INS","DEL",  /* 50 - 59*/ \
"HOME", ".", "S", "K", "PG\nUP",  "",  "PG\nDOWN",  "",  "",  ""}    /* 60 - 69*/
#define KEYS_TEXT_NUM   70

// Defines for EmuPanel

// New look
#define F9010PANEL_BMP  "media/Fluke9010_front2.bmp"

#define KEYBBEGX 91
#define KEYBENDX 1831
#define KEYBBEGY 285
#define KEYBENDY 1015
#define KEYB_RECT wxRect(KEYBBEGX, KEYBBEGY, KEYBENDX-KEYBBEGX, KEYBENDY-KEYBBEGY)

// Separation between columns
#define COL1    {171, 221}
#define COL2    {301, 409}
#define COL3    {489, 539}
#define COL4    {619, 669}
#define COL5    {749, 799}
#define COL6    {879, 987}
#define COL7    {1067, 1116}
#define COL8    {1196, 1303}
#define COL9    {1383, 1433}
#define COL10   {1513, 1563}
#define COL11   {1643, 1751}
#define COL12   {KEYBENDX, KEYBENDX}
#define COL13   {KEYBENDX, KEYBENDX}
#define COL14   {KEYBENDX, KEYBENDX}
#define COL15   {KEYBENDX, KEYBENDX}
#define COLTNUM 15
#define COLNUM  12

#define COLS {COL1, COL2, COL3, COL4, COL5, COL6, COL7, COL8, COL9, COL10, COL11, COL12, COL13, COL14, COL15}

// Key ROW coordinates
#define ROW00POS    {{287, 367}, {410, 490}, {531,611}, {690, 770}, {812, 892}, {935, 1015}}
#define ROW02POS    {{287, 367}, {452, 532}, {574,654}, {690, 770}, {812, 892}, {935, 1015}}
#define ROW11POS    {{287, 367}, {410, 490}, {574,654}, {754, 834}, {935, 1015}, {1015, 1015}}
#define ROWNUM  6

#define ROWS {ROW00POS, ROW00POS, ROW02POS, ROW02POS, ROW02POS, ROW02POS, ROW00POS, ROW00POS, ROW00POS, ROW00POS, ROW00POS, ROW11POS}

// Key active colors
#define KEY_ACT_COLOR wxColour(250, 250, 250)
#define KEY_HIPUL_ACT_COLOR wxColour(250, 50, 50)
#define KEY_LOPUL_ACT_COLOR wxColour( 50, 250, 50)

// Power button
#define POWER_BUT_RECT      wxRect(1683, 139, 217, 60)
#define POWER_BUT_CORNR     10
#define POWER_BUT_ACT_COLOR wxColour(24, 84, 250)

// Tape drive
#define TAPEDRIVE_RECT wxRect(1689, 43, 202, 45)
#define TAPEDRIVE_CORNR 2
#define TAPETEXT_COLOR 0,0,0
// LEDS
#define LED_SIZE  wxSize(32, 16)
#define LED_CORNR 3
#define LEDSPOS   {wxPoint(1587, 52), wxPoint(1587, 81), wxPoint(1587, 110), wxPoint(1587, 139), wxPoint(1587, 168)}

// Display
#define DISPLAY_RECT    wxRect(76, 50, 1290, 110)
#define DISPLAY_COLOR   232,162,2

// Display + LED panel rectangle
#define DISPPANEL_RECT  wxRect(0, 0, 1600, 200)

#define DISP_NUMCHAR           32

// Keyboard mappings PC keyboard to Fluke key

#define ASCIIKEYMAP \
 KEY_NOKEY, KEY_NOKEY, KEY_NOKEY, KEY_NOKEY,    /* ASCII  0..3 */\
 KEY_NOKEY, KEY_NOKEY, KEY_NOKEY, KEY_NOKEY,    /* ASCII  4..7 */\
 KEY_CLEAR_NO, KEY_NOKEY, KEY_NOKEY, KEY_NOKEY, /* ASCII  8..11 */\
 KEY_NOKEY, KEY_ENTER_YES, KEY_NOKEY, KEY_NOKEY,/* ASCII 12..15 */\
 KEY_NOKEY, KEY_NOKEY, KEY_NOKEY, KEY_NOKEY,    /* ASCII 16..19 */\
 KEY_NOKEY, KEY_NOKEY, KEY_NOKEY, KEY_NOKEY,    /* ASCII 20..23 */\
 KEY_NOKEY, KEY_NOKEY, KEY_NOKEY, KEY_STOP,     /* ASCII 24..27 */\
 KEY_NOKEY, KEY_NOKEY, KEY_NOKEY, KEY_NOKEY,    /* ASCII 28..31 */\
 KEY_STS_CTL, KEY_NOKEY, KEY_AUTO, KEY_ROM_VIEW,/* ASCII 32..35 */\
 KEY_ROM_TEST, KEY_IO_VIEW, KEY_NOKEY, KEY_LEARN,/* ASCII 36..39 */\
 KEY_NOKEY, KEY_NOKEY, KEY_RAM_VIEW, KEY_REG,   /* ASCII 40..43 */\
 KEY_LOOP, KEY_COMPL, KEY_SETUP, KEY_RAMLONG_TEST,/* ASCII 44..47 */\
 KEY_0,     KEY_1,      KEY_2,      KEY_3,      /* ASCII 48..51 */\
 KEY_4,     KEY_5,      KEY_6,      KEY_7,      /* ASCII 52..55 */\
 KEY_8,     KEY_9,      KEY_NOKEY,  KEY_REPEAT, /* ASCII 56..59 */\
 KEY_SHIFT_RIGHT, KEY_OR,  KEY_DECR,  KEY_READ_PROBE,/* ASCII 60..63 */\
 KEY_RUN_UUT,KEY_A,     KEY_B,      KEY_C,      /* ASCII 64..67 @ABC */\
 KEY_D,     KEY_E,      KEY_F,      KEY_READ_TS,/* ASCII 68..71 DEFG */\
 KEY_WRITE_TS,KEY_PROGRAM,KEY_EXEC, KEY_AUX_IF, /* ASCII 72..75 HIJK */\
 KEY_RAMP,  KEY_WALK,   KEY_IF,     KEY_GR_THAN,/* ASCII 76..79 LMNO */\
 KEY_EQUALS,KEY_TOGGL_ADDR,KEY_TOGGL_DATA,KEY_DISPL,/* ASCII 80..83 PQRS */\
 KEY_LABEL, KEY_GOTO,   KEY_CONT,   KEY_STOP,   /* ASCII 84..87 TUVW */\
 KEY_AND,   KEY_SHIFT_LEFT,KEY_INCR,KEY_NOKEY   /* ASCII 88..91 XYZ */

#define ASCIIKEYMAPSIZE 92

#define OTHERKEYMAP \
 {WXK_NUMPAD0, KEY_0}, 	/*0*/ \
 {WXK_NUMPAD1, KEY_1}, 	/*1*/ \
 {WXK_NUMPAD2, KEY_2}, 	/*2*/ \
 {WXK_NUMPAD3, KEY_3}, 	/*3*/ \
 {WXK_NUMPAD4, KEY_4}, 	/*4*/ \
 {WXK_NUMPAD5, KEY_5}, 	/*5*/ \
 {WXK_NUMPAD6, KEY_6}, 	/*6*/ \
 {WXK_NUMPAD7, KEY_7}, 	/*7*/ \
 {WXK_NUMPAD8, KEY_8}, 	/*8*/ \
 {WXK_NUMPAD9, KEY_9}, 	/*9*/ \
 {WXK_LEFT, KEY_PRIOR}, /*10*/ \
 {WXK_RIGHT, KEY_MORE}, /*11*/ \
 {WXK_PAGEUP, KEY_HIGH_DOWN}, /*12*/ \
 {WXK_PAGEDOWN, KEY_LOW_DOWN}, /*13*/ \
 {WXK_HOME, KEY_SYNC},      /*14*/ \
 {WXK_INSERT, KEY_READ_TAPE}, /*15*/ \
 {WXK_DELETE, KEY_WRITE_TAPE}, /*16*/ \
 {WXK_F1, KEY_BUS_TEST},        /*17*/ \
 {WXK_F2, KEY_RAMSHORT_TEST}, /*18*/ \
 {WXK_F3, KEY_ROM_TEST}, /*19*/ \
 {WXK_F4, KEY_IO_TEST}, /*20*/ \
 {WXK_F7, KEY_RAMLONG_TEST}, /*21*/ \
 {WXK_F8, KEY_AUTO}, /*22*/ \
 {WXK_F9, KEY_LEARN}, /*23*/ \
 {WXK_F10, KEY_RAM_VIEW}, /*24*/ \
 {WXK_F11, KEY_ROM_VIEW}, /*25*/ \
 {WXK_F12, KEY_IO_VIEW}, /*26*/ \
 {WXK_NUMPAD_ENTER, KEY_ENTER_YES}, 	/*27*/ \

#define OTHERKEYMAPSIZE 28


#endif // EMUPANELDEF_H_INCLUDED
