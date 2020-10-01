#ifndef VT100_VT100WIN10_INCLUDED
#define VT100_VT100WIN10_INCLUDED
/* (c) 2019 by dbj.org   -- CC BY-SA 4.0 -- https://creativecommons.org/licenses/by-sa/4.0/ */

/*
VT100 ESC codes for coloring the WIN10 PRO console output
full explanation:
https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences?redirectedfrom=MSDN
*/
// #define VT100_ESC "\x1b"
#define VT100_CSI "\x1b["

#define VT100_CLS VT100_CSI "2J"
#define VT100_RESET VT100_CSI "0m"
#define VT100_BOLD_FG VT100_CSI "1m"
#define VT100_ULINE VT100_CSI "4m"
#define VT100_ULINE_OFF VT100_CSI "24m"
#define VT100_COLOR_SWAP VT100_CSI "7m"

#define VT100_FG_RED VT100_CSI "31m"
#define VT100_FG_GREEN VT100_CSI "32m"
#define VT100_FG_YELLOW VT100_CSI "33m"
#define VT100_FG_BLUE VT100_CSI "34m"
#define VT100_FG_CYAN VT100_CSI "36m"
#define VT100_FG_WHITE VT100_CSI "37m"

#define VT100_FG_RED_BOLD VT100_BOLD_FG VT100_FG_RED
#define VT100_FG_GREEN_BOLD VT100_BOLD_FG VT100_FG_GREEN
#define VT100_FG_YELLOW_BOLD VT100_BOLD_FG VT100_FG_YELLOW
#define VT100_FG_BLUE_BOLD VT100_BOLD_FG VT100_FG_BLUE
#define VT100_FG_CYAN_BOLD VT100_BOLD_FG VT100_FG_CYAN
#define VT100_FG_WHITE_BOLD VT100_BOLD_FG VT100_FG_WHITE

#define VT100_BG_BLACK VT100_CSI "40m"
#define VT100_BG_RED VT100_CSI "41m"
#define VT100_BG_GREEN VT100_CSI "42m"
#define VT100_BG_YELLOW VT100_CSI "43m"
#define VT100_BG_BLUE VT100_CSI "44m"

#endif // VT100_VT100WIN10_INCLUDED