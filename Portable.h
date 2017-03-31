/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  GRAPH                            1
#define  GRAPH_RB_CURRENT                 2       /* control type: radioButton, callback function: OnCursor */
#define  GRAPH_RB_FORCE                   3       /* control type: radioButton, callback function: OnCursor */
#define  GRAPH_GRAPH_OLD                  4       /* control type: graph, callback function: OnGraph */
#define  GRAPH_TEXTMSG_3                  5       /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_4                  6       /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_5                  7       /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_6                  8       /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_7                  9       /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_9                  10      /* control type: textMsg, callback function: (none) */
#define  GRAPH_FORCE                      11      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TIME_CURRENT               12      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_8                  13      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_10                 14      /* control type: textMsg, callback function: (none) */
#define  GRAPH_CURRENT                    15      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TIME_FORCE                 16      /* control type: textMsg, callback function: (none) */
#define  GRAPH_DECORATION_2               17      /* control type: deco, callback function: (none) */
#define  GRAPH_DECORATION                 18      /* control type: deco, callback function: (none) */
#define  GRAPH_GRAPH                      19      /* control type: graph, callback function: OnGraph */
#define  GRAPH_TEXTMSG                    20      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_11                 21      /* control type: textMsg, callback function: (none) */

#define  MAIN                             2
#define  MAIN_STEP                        2       /* control type: textMsg, callback function: (none) */
#define  MAIN_STEP_TEXT                   3       /* control type: textMsg, callback function: (none) */
#define  MAIN_TYPENAME                    4       /* control type: string, callback function: (none) */
#define  MAIN_STEPMSG_3                   5       /* control type: textMsg, callback function: (none) */
#define  MAIN_STEPMSG_2                   6       /* control type: textMsg, callback function: (none) */
#define  MAIN_STEPMSG                     7       /* control type: textMsg, callback function: (none) */
#define  MAIN_LED_ERROR                   8       /* control type: LED, callback function: (none) */
#define  MAIN_LED_RUNING_2                9       /* control type: LED, callback function: (none) */
#define  MAIN_LED_RUNING_8                10      /* control type: LED, callback function: (none) */
#define  MAIN_LED_SAVEAS                  11      /* control type: LED, callback function: (none) */
#define  MAIN_LED_SYSTEM                  12      /* control type: LED, callback function: (none) */
#define  MAIN_LED_PRINT                   13      /* control type: LED, callback function: (none) */
#define  MAIN_LED_OPEN                    14      /* control type: LED, callback function: (none) */
#define  MAIN_LED_SAVE                    15      /* control type: LED, callback function: (none) */
#define  MAIN_LED_RUNING                  16      /* control type: LED, callback function: (none) */
#define  MAIN_TM_TIME                     17      /* control type: textMsg, callback function: (none) */
#define  MAIN_DECORATION_2                18      /* control type: deco, callback function: (none) */
#define  MAIN_LED_AUTO                    19      /* control type: LED, callback function: (none) */
#define  MAIN_TM_TITLE_2                  20      /* control type: textMsg, callback function: (none) */
#define  MAIN_NUM_VREF                    21      /* control type: numeric, callback function: (none) */
#define  MAIN_NUM_FORCE                   22      /* control type: numeric, callback function: (none) */
#define  MAIN_NUM_I                       23      /* control type: numeric, callback function: (none) */
#define  MAIN_NUM_V                       24      /* control type: numeric, callback function: (none) */
#define  MAIN_PIC_SYSTEM                  25      /* control type: picture, callback function: OnSystem */
#define  MAIN_DECORATION                  26      /* control type: deco, callback function: (none) */
#define  MAIN_PIC_SAVEAS                  27      /* control type: picture, callback function: (none) */
#define  MAIN_PIC_OPEN                    28      /* control type: picture, callback function: OnOpenData */
#define  MAIN_TEXTMSG                     29      /* control type: textMsg, callback function: (none) */
#define  MAIN_TEXTMSG_2                   30      /* control type: textMsg, callback function: (none) */
#define  MAIN_TEXTMSG_4                   31      /* control type: textMsg, callback function: (none) */
#define  MAIN_TIMER_MEASURE               32      /* control type: timer, callback function: OnTimer_Measure */
#define  MAIN_PIC_SAVE                    33      /* control type: picture, callback function: OnSaveData */
#define  MAIN_PIC_PRINT                   34      /* control type: picture, callback function: OnPrint */
#define  MAIN_TIMER_WATCH                 35      /* control type: timer, callback function: OnTimer_Watch */
#define  MAIN_DECORATION_3                36      /* control type: deco, callback function: (none) */
#define  MAIN_PICTURE_6                   37      /* control type: picture, callback function: OnExitPrograme */
#define  MAIN_TEXTMSG_3                   38      /* control type: textMsg, callback function: (none) */
#define  MAIN_PIC_MEASURE                 39      /* control type: picture, callback function: OnStartMeasure */

#define  MAIN_DESK                        3
#define  MAIN_DESK_STEPMSG_3              2       /* control type: textMsg, callback function: (none) */
#define  MAIN_DESK_STEPMSG_2              3       /* control type: textMsg, callback function: (none) */
#define  MAIN_DESK_LED_RUNING_8           4       /* control type: LED, callback function: (none) */
#define  MAIN_DESK_LED_PRINT              5       /* control type: LED, callback function: (none) */
#define  MAIN_DESK_LED_OPEN               6       /* control type: LED, callback function: (none) */
#define  MAIN_DESK_TM_TIME                7       /* control type: textMsg, callback function: (none) */
#define  MAIN_DESK_DECORATION_2           8       /* control type: deco, callback function: (none) */
#define  MAIN_DESK_TM_TITLE_2             9       /* control type: textMsg, callback function: (none) */
#define  MAIN_DESK_DECORATION             10      /* control type: deco, callback function: (none) */
#define  MAIN_DESK_PIC_OPEN               11      /* control type: picture, callback function: OnOpenData */
#define  MAIN_DESK_PIC_PRINT              12      /* control type: picture, callback function: OnPrint */
#define  MAIN_DESK_TIMER_WATCH            13      /* control type: timer, callback function: OnTimer_Watch */
#define  MAIN_DESK_DECORATION_3           14      /* control type: deco, callback function: (none) */
#define  MAIN_DESK_PICTURE_6              15      /* control type: picture, callback function: OnExitPrograme */

#define  MEASURE                          4
#define  MEASURE_DECORATION               2       /* control type: deco, callback function: (none) */
#define  MEASURE_PIC_MANUAL_MEASURE       3       /* control type: picture, callback function: OnMeasureStart */
#define  MEASURE_PIC_AUTO_MEASURE         4       /* control type: picture, callback function: OnMeasureStart */
#define  MEASURE_TEST_TIME                5       /* control type: string, callback function: (none) */
#define  MEASURE_TEST_PERSON              6       /* control type: string, callback function: (none) */
#define  MEASURE_TEST_ADDR                7       /* control type: string, callback function: (none) */
#define  MEASURE_FORCE_SENSOR_TYPE        8       /* control type: string, callback function: (none) */
#define  MEASURE_SWITCH_NUM               9       /* control type: string, callback function: (none) */
#define  MEASURE_RADIOBUTTON              10      /* control type: radioButton, callback function: (none) */
#define  MEASURE_SWITCH_MODEL             11      /* control type: ring, callback function: (none) */
#define  MEASURE_PIC_CANCEL               12      /* control type: picture, callback function: OnMeasureCancel */

#define  P_PRINT                          5
#define  P_PRINT_TEXTMSG                  2       /* control type: textMsg, callback function: (none) */
#define  P_PRINT_GRAPH                    3       /* control type: graph, callback function: (none) */
#define  P_PRINT_TABLE                    4       /* control type: table, callback function: (none) */
#define  P_PRINT_SPLITTER_2               5       /* control type: splitter, callback function: (none) */
#define  P_PRINT_SPLITTER_4               6       /* control type: splitter, callback function: (none) */
#define  P_PRINT_SPLITTER_3               7       /* control type: splitter, callback function: (none) */
#define  P_PRINT_SPLITTER                 8       /* control type: splitter, callback function: (none) */
#define  P_PRINT_SPLITTER_7               9       /* control type: splitter, callback function: (none) */
#define  P_PRINT_SPLITTER_5               10      /* control type: splitter, callback function: (none) */
#define  P_PRINT_SPLITTER_6               11      /* control type: splitter, callback function: (none) */

#define  PANEL                            6
#define  PANEL_GRAPH                      2       /* control type: graph, callback function: (none) */

#define  SYSTEM                           7
#define  SYSTEM_DECORATION                2       /* control type: deco, callback function: (none) */
#define  SYSTEM_PICTURE                   3       /* control type: picture, callback function: OnSystemSet */
#define  SYSTEM_LED_ERROR_2               4       /* control type: LED, callback function: (none) */
#define  SYSTEM_STRING_3                  5       /* control type: string, callback function: (none) */
#define  SYSTEM_LED_USB                   6       /* control type: LED, callback function: (none) */
#define  SYSTEM_LED_ControlOn             7       /* control type: LED, callback function: (none) */
#define  SYSTEM_LED_RUNING                8       /* control type: LED, callback function: (none) */
#define  SYSTEM_LED_AUTO                  9       /* control type: LED, callback function: (none) */
#define  SYSTEM_STRING_6                  10      /* control type: string, callback function: (none) */
#define  SYSTEM_STRING_2                  11      /* control type: string, callback function: (none) */
#define  SYSTEM_TEXTMSG                   12      /* control type: textMsg, callback function: (none) */
#define  SYSTEM_TEXTMSG_2                 13      /* control type: textMsg, callback function: (none) */
#define  SYSTEM_TEXTMSG_3                 14      /* control type: textMsg, callback function: (none) */

#define  USER_DATA                        8
#define  USER_DATA_DECORATION             2       /* control type: deco, callback function: (none) */
#define  USER_DATA_PIC_CANCEL             3       /* control type: picture, callback function: OnCancelSaveUserData */
#define  USER_DATA_PIC_SAVE               4       /* control type: picture, callback function: OnSaveUserData */
#define  USER_DATA_SWITCH_NUM             5       /* control type: string, callback function: ONSwitchNumChange */
#define  USER_DATA_TEST_ADDR              6       /* control type: string, callback function: (none) */
#define  USER_DATA_TEST_PERSON            7       /* control type: string, callback function: (none) */
#define  USER_DATA_FILENAME               8       /* control type: string, callback function: (none) */
#define  USER_DATA_TEST_TIME              9       /* control type: string, callback function: (none) */
#define  USER_DATA_FORCE_SENSOR_TYPE      10      /* control type: string, callback function: (none) */
#define  USER_DATA_RADIOBUTTON            11      /* control type: radioButton, callback function: (none) */
#define  USER_DATA_SWITCH_MODEL           12      /* control type: ring, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnCancelSaveUserData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnCursor(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnExitPrograme(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnGraph(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMeasureCancel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMeasureStart(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnOpenData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPrint(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSaveData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSaveUserData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnStartMeasure(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ONSwitchNumChange(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSystem(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSystemSet(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer_Measure(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer_Watch(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
