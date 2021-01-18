# How to add debug console on LPC802 EVK
在專案上按右鍵 -> Properties -> C/C++ Build -> Settings -> Tool Settings -> Preprocessor
檢查設定 SDK_DEBUGCONSOLE=1

# 加入程式
## 主程式
#include fsl_debug_console.h
main()
{
    BOARD_InitDebugConsole();
    PRINTF("TEST\r\n");
}
## pin_mux.c
    #include "fsl_swm_connections.h"
    CLOCK_EnableClock(kCLOCK_Swm);

    /* USART0_TXD connect to P0_4 */
    SWM_SetMovablePinSelect(SWM0, kSWM_USART0_TXD, kSWM_PortPin_P0_4);

    /* USART0_RXD connect to P0_0 */
    SWM_SetMovablePinSelect(SWM0, kSWM_USART0_RXD, kSWM_PortPin_P0_0);