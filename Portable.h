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
#define  GRAPH_TEXTMSG_15                 8       /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_6                  9       /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_7                  10      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_9                  11      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TIME_DELTA                 12      /* control type: textMsg, callback function: (none) */
#define  GRAPH_FORCE                      13      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_16                 14      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TIME_CURRENT               15      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_8                  16      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_10                 17      /* control type: textMsg, callback function: (none) */
#define  GRAPH_CURRENT                    18      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TIME_FORCE                 19      /* control type: textMsg, callback function: (none) */
#define  GRAPH_DECORATION_3               20      /* control type: deco, callback function: (none) */
#define  GRAPH_DECORATION_2               21      /* control type: deco, callback function: (none) */
#define  GRAPH_DECORATION                 22      /* control type: deco, callback function: (none) */
#define  GRAPH_GRAPH                      23      /* control type: graph, callback function: OnGraph */
#define  GRAPH_TEXTMSG_14                 24      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_13                 25      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG                    26      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_12                 27      /* control type: textMsg, callback function: (none) */
#define  GRAPH_TEXTMSG_11                 28      /* control type: textMsg, callback function: (none) */

#define  MAIN                             2
#define  MAIN_STEPMSG_3                   2       /* control type: textMsg, callback function: (none) */
#define  MAIN_STEPMSG_2                   3       /* control type: textMsg, callback function: (none) */
#define  MAIN_LED_RUNING_8                4       /* control type: LED, callback function: (none) */
#define  MAIN_LED_SAVEAS                  5       /* control type: LED, callback function: (none) */
#define  MAIN_LED_SYSTEM                  6       /* control type: LED, callback function: (none) */
#define  MAIN_LED_PRINT                   7       /* control type: LED, callback function: (none) */
#define  MAIN_LED_OPEN                    8       /* control type: LED, callback function: (none) */
#define  MAIN_LED_SAVE                    9       /* control type: LED, callback function: (none) */
#define  MAIN_LED_RUNING                  10      /* control type: LED, callback function: (none) */
#define  MAIN_TM_TIME                     11      /* control type: textMsg, callback function: (none) */
#define  MAIN_DECORATION_2                12      /* control type: deco, callback function: (none) */
#define  MAIN_TM_TITLE_2                  13      /* control type: textMsg, callback function: (none) */
#define  MAIN_NUM_VREF                    14      /* control type: numeric, callback function: (none) */
#define  MAIN_NUM_FORCE                   15      /* control type: numeric, callback function: (none) */
#define  MAIN_NUM_I                       16      /* control type: numeric, callback function: (none) */
#define  MAIN_NUM_V                       17      /* control type: numeric, callback function: (none) */
#define  MAIN_PIC_SYSTEM                  18      /* control type: picture, callback function: OnSystem */
#define  MAIN_DECORATION                  19      /* control type: deco, callback function: (none) */
#define  MAIN_PIC_SAVEAS                  20      /* control type: picture, callback function: (none) */
#define  MAIN_PIC_OPEN                    21      /* control type: picture, callback function: OnOpenData */
#define  MAIN_UNIT_V                      22      /* control type: textMsg, callback function: (none) */
#define  MAIN_UNIT_I                      23      /* control type: textMsg, callback function: (none) */
#define  MAIN_UNIT_VREF                   24      /* control type: textMsg, callback function: (none) */
#define  MAIN_TIMER_MEASURE               25      /* control type: timer, callback function: OnTimer_Measure */
#define  MAIN_PIC_SAVE                    26      /* control type: picture, callback function: OnSaveData */
#define  MAIN_PIC_PRINT                   27      /* control type: picture, callback function: OnPrint */
#define  MAIN_TIMER_WATCH                 28      /* control type: timer, callback function: OnTimer_Watch */
#define  MAIN_DECORATION_3                29      /* control type: deco, callback function: (none) */
#define  MAIN_PICTURE_6                   30      /* control type: picture, callback function: OnExitPrograme */
#define  MAIN_UNIT_FORCE                  31      /* control type: textMsg, callback function: (none) */
#define  MAIN_PIC_MEASURE                 32      /* control type: picture, callback function: OnStartMeasure */

#define  MAIN_DESK                        3
#define  MAIN_DESK_MSG_COMPANY_TEL        2       /* control type: textMsg, callback function: (none) */
#define  MAIN_DESK_MSG_COMPANY_NAME       3       /* control type: textMsg, callback function: (none) */
#define  MAIN_DESK_LED_RUNING_8           4       /* control type: LED, callback function: (none) */
#define  MAIN_DESK_LED_PRINT_2            5       /* control type: LED, callback function: (none) */
#define  MAIN_DESK_LED_PRINT              6       /* control type: LED, callback function: (none) */
#define  MAIN_DESK_LED_OPEN               7       /* control type: LED, callback function: (none) */
#define  MAIN_DESK_TM_TIME                8       /* control type: textMsg, callback function: (none) */
#define  MAIN_DESK_DECORATION_2           9       /* control type: deco, callback function: (none) */
#define  MAIN_DESK_TM_TITLE_2             10      /* control type: textMsg, callback function: (none) */
#define  MAIN_DESK_DECORATION             11      /* control type: deco, callback function: (none) */
#define  MAIN_DESK_PIC_PRINT_2            12      /* control type: picture, callback function: OnPrint */
#define  MAIN_DESK_PIC_OPEN               13      /* control type: picture, callback function: OnOpenData */
#define  MAIN_DESK_PIC_WAVE_DATA          14      /* control type: picture, callback function: OnSwitchDataAndWave */
#define  MAIN_DESK_TIMER_WATCH            15      /* control type: timer, callback function: OnTimer_Watch */
#define  MAIN_DESK_DECORATION_3           16      /* control type: deco, callback function: (none) */
#define  MAIN_DESK_PICTURE_6              17      /* control type: picture, callback function: OnExitPrograme */

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

#define  USER_CMD                         8
#define  USER_CMD_PIC_SAVE                2       /* control type: picture, callback function: OnSaveUserData */
#define  USER_CMD_PIC_CANCEL              3       /* control type: picture, callback function: OnCancelSaveUserData */

#define  USER_DATA                        9
#define  USER_DATA_SWITCH_NUM             2       /* control type: string, callback function: ONSwitchNumChange */
#define  USER_DATA_TEST_ADDR              3       /* control type: string, callback function: (none) */
#define  USER_DATA_TEST_PERSON            4       /* control type: string, callback function: (none) */
#define  USER_DATA_FILENAME               5       /* control type: string, callback function: (none) */
#define  USER_DATA_TEST_TIME              6       /* control type: string, callback function: (none) */
#define  USER_DATA_FORCE_SENSOR_TYPE      7       /* control type: string, callback function: (none) */
#define  USER_DATA_RADIOBUTTON            8       /* control type: radioButton, callback function: (none) */
#define  USER_DATA_SWITCH_MODEL           9       /* control type: ring, callback function: (none) */


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
int  CVICALLBACK OnSwitchDataAndWave(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ONSwitchNumChange(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSystem(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSystemSet(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer_Measure(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer_Watch(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
