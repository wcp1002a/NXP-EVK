/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <sensors/tmf8801.h>
#include <stdio.h>
#include "LCD1602_PCF8574_board.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "math.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "custom_func.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define I2C_BAUDRATE (100000U)
#define HWTIMER_PERIOD 10000U
#define SLAVE_ADDR_TOF_SENSOR 0x41
#define SLAVE_ADDR_PCF8574T   0x27
#define SLAVE_ADDR_PCF8574AT  0x3F
#define SLAVE_ADDR_LCD1602_PCF8574 SLAVE_ADDR_PCF8574T

#define TMF8801_ENABLE  GPIO_PinWrite(BOARD_USER_LED_GPIO, 20, 1U)
#define TMF8801_DISABLE GPIO_PinWrite(BOARD_USER_LED_GPIO, 20, 0U)
#define TMF8801_MEASURE_TIMES 1

// #define DISTANCE_DETECTION_DEMO     // Enable distance detection
#ifdef DISTANCE_DETECTION_DEMO
#define LED_ON   GPIO_PinWrite(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN, 1U)
#define LED_OFF  GPIO_PinWrite(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN, 0U) 
#define LED_BLINK_PERIOD_MS 250
uint16_t g_distance_threshold = 2000;   // distance <= 2m led on, distance > 2m led blinking
bool    g_blLedOn = false;
#endif /* DISTANCE_DETECTION_DEMO */
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Hardware timer initialize
 *
 */
static void HW_Timer_init(void);
void tmf8801_measure_once(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern uint8_t __g_cali_buf[14];
volatile uint16_t SampleEventFlag;
tmf8801_handle_t g_tmf8801Handle;
lcd_handle_t g_lcdHandle;
//uint8_t tof_sensor_address[] = {0x41U};	//20200526 paul: tmf8801 slave address

//Timer
volatile uint32_t g_systickCounter;
volatile uint32_t g_systick = 0;

uint8_t g_lcdDeteced;

app0_init_config_t app_config = {0};
result_info_t      g_tof_result = {0};
double             g_calibration_factor = 1.0;     /* ?????? */

/*******************************************************************************
 * Code
 ******************************************************************************/
static void HW_Timer_init(void)
{
    /* Configure the SysTick timer */
    SysTick_Config(SystemCoreClock / HWTIMER_PERIOD);
}

void SysTick_Handler(void)
{
    SampleEventFlag = 1;
    if (g_systickCounter != 0U)
    {
        g_systickCounter--;
    }
    g_systick++;
#ifdef DISTANCE_DETECTION_DEMO
    if ( g_systick % (LED_BLINK_PERIOD_MS*10) == 0){
        g_blLedOn = !g_blLedOn;
    }
#endif
}

void SysTick_DelayTicks(uint32_t n)
{
    g_systickCounter = n;
    while (g_systickCounter != 0U)
    {
    }
}

void show_usage(void)
{
	PRINTF("\r\n");
	PRINTF("+--------------------------------------+\r\n");
	PRINTF("| AMS TMF8801 Tof module demonstration |\r\n");
	PRINTF("+--------------------------------------+\r\n");
    PRINTF("Usage:\r\n");
    PRINTF("  Use SW4 to control the board.\r\n");
    PRINTF("  Single Click - Switch to the next function.\r\n");
    PRINTF("  Double Click - Execute current function.\r\n");
    PRINTF("  Long press & release - Back to function selection.\r\n\r\n");
}

int main(void)
{
    tmf8801_config_t config = {0};
    status_t result;
    uint8_t ret;
    uint8_t cur_idx = 0;
    uint8_t i;

    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /*Clock setting for LPI2C*/
    CLOCK_SetMux(kCLOCK_Lpi2cMux, BOARD_ACCEL_I2C_CLOCK_SOURCE_SELECT);
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, BOARD_ACCEL_I2C_CLOCK_SOURCE_DIVIDER);

    HW_Timer_init();

    BOARD_I2C_Init();

    //+----------------------------------+
    //| LCD_PCF8574__board_configuration |
    //+----------------------------------+
    PRINTF("LCD Init: ");
    g_lcdHandle.I2C_SendFunc    = BOARD_I2C_Send;
    g_lcdHandle.I2C_ReceiveFunc = BOARD_I2C_Receive;
    /* Set Slave Address. */
    g_lcdHandle.slaveAddress = SLAVE_ADDR_LCD1602_PCF8574;

    result = LCD_initialize(&g_lcdHandle, 16, 2);
    if (result == kStatus_Success)
    {
        PRINTF("LCD module Detected\r\n");
        g_lcdDeteced = true;
    }else{
        PRINTF("LCD module can't be detected\r\n");
        g_lcdDeteced = false;
    }
    LCD_write_big_string("E D O M ", 0, 0);
    SysTick_DelayTicks(10000);
    LCD_println("EDOM Technology", 0);
    LCD_println("AMS TMF8801 Demo", 1);

    //+------------------------+
    //| TMF8801 Initialization |
    //+------------------------+
    /* Configure the I2C function */
    config.I2C_SendFunc    = BOARD_I2C_Send;
    config.I2C_ReceiveFunc = BOARD_I2C_Receive;
    config.slaveAddress = SLAVE_ADDR_TOF_SENSOR;

    PRINTF("Read TMF8801 Device ID: ");

    /* Enable TMF8801 */
    TMF8801_ENABLE;         //GPIO_AD_06 set to High
    SysTick_DelayTicks(80); //Delay 8ms: Enable high to ready for measurement 

    /* Read ID */
    result = TMF8801_ReadID(&g_tmf8801Handle, &config); // Check uf E3 Reg. = 0x07

    if (result != kStatus_Success){
        if(result == kStatus_OutOfRange){
    	    PRINTF(" ==> not matched\r\n");
        }else{
            PRINTF(" Fail!");
        }
        PRINTF("System halt!\r\n");
//        while(1);
        if(g_lcdDeteced){
    	    LCD_println("TMF8801 Module ", 0);
    	    LCD_println("Initial Error  ", 1);
    	    while ((ret = check_button(6000)) == butNoClick);
        }
    }else{
    	PRINTF(" ==> TMF8801 detected!\r\n");
    }

    PRINTF("READ APPID: ");
    result = TMF8801_ReadAppId(&g_tmf8801Handle);
    if (result != kStatus_Success)
    {
        PRINTF("Fail\r\n");
    }

    /* Initialize AMS TMF8801 Tof sensor */
    PRINTF("TMF8801 initialization\r\n");
	if (TMF8801_Init(&g_tmf8801Handle)!= kStatus_Success)
    {
        PRINTF("tmf8801_init err!\r\n");
    }

    PRINTF("TMF8801 app0 start\r\n");
    tmf8801_app0_get_default_config(&app_config);

    if (tmf8801_app0_start(&g_tmf8801Handle, &app_config) != kStatus_Success) {
        PRINTF("  tmf8801_app0_start err!\r\n");
    }

#ifdef DISTANCE_DETECTION_DEMO
    while (true)
    {
        ret = select_threshold(cur_idx);
        cur_idx = ret;
        switch(ret){
            case 0:     g_distance_threshold =  500;    break;
            case 1:     g_distance_threshold = 1000;    break;
            case 2:     g_distance_threshold = 1500;    break;
            case 3:     g_distance_threshold = 2000;    break;
            default:    g_distance_threshold = 2000;    break;
        }
        while ((ret = check_button(6000)) != butLongPress)
        {
            tmf8801_measure_once();
            // g_blLedOn = !g_blLedOn;             // toggle LED On/Off
            if(g_tof_result.distance_peak > g_distance_threshold && !g_blLedOn){
                LED_OFF;
            }else{
                LED_ON;
            }
        }
    }
#endif /* DISTANCE_DETECTION_DEMO */

    show_usage();
    while(1){
    	ret = select_function(cur_idx);
    	cur_idx = ret;
    	PRINTF("==>Exec. item:%d\r\n", cur_idx+1);
        if(cur_idx == 0)
        {
            if(g_lcdDeteced){
        	    LCD_println("1.Calibration:  ", 0);
        	    LCD_println("  Start", 1);
            }
        	if (tmf8801_app0_stop(&g_tmf8801Handle) != kStatus_Success){
        		PRINTF("tmf8801_app_stop err!\r\n");
                if(g_lcdDeteced){
                    LCD_println(" App stop error ", 1);
                    SysTick_DelayTicks(2000);
                }
            }
			if (tmf8801_factory_calibration(&g_tmf8801Handle) != kStatus_Success){
				PRINTF("Factory Calibration Fail\r\n");
                if(g_lcdDeteced){
                    LCD_println(" Calibration err", 1);
                    SysTick_DelayTicks(2000);
                }
//				break;
			}
			app_config.calibration_state_mask = FACTORY_CALIBRATION_PROVIDE;
			if (tmf8801_app0_start(&g_tmf8801Handle, &app_config) == kStatus_Fail)
			{
				PRINTF("tmf8801_app0_start err!\r\n");
                if(g_lcdDeteced){
                    LCD_println(" App start err  ", 1);
                    SysTick_DelayTicks(2000);
                }
			}
            if(g_lcdDeteced){
                i = 0;
                while(1){
                    LCD_setcursor(0,1);
                    if(i == 12){
                        LCD_printf(" [%02d-%02d]%02X%02X%", i, i+1, __g_cali_buf[i+0], __g_cali_buf[i+1]);
                        LCD_printf("    ");
                    }else{
                        LCD_printf(" [%02d-%02d]%02X%02X%02X%02X", i, i+3, __g_cali_buf[i+0], __g_cali_buf[i+1], __g_cali_buf[i+2], __g_cali_buf[i+3]);
                    }
    	        	while ((ret = check_button(6000)) == butNoClick);
    	    
                	if(ret == butLongPress)
    	        	{
    	        		break;
    	        	}else if(ret == butClick1){
                        i += 4;
                        if(i >= 14) i = 0;
                    }
                }
                LCD_println("  Finish!", 1);
                SysTick_DelayTicks(5000);
           }
        }
        if (cur_idx == 1)
		{
            if(g_lcdDeteced){
        	    LCD_println("1.No Calibration:", 0);
        	    LCD_println("  app stop", 1);
            }
        	if (tmf8801_app0_stop(&g_tmf8801Handle) != kStatus_Success){
        		PRINTF("tmf8801_app_stop err!\r\n");
        	}
			app_config.calibration_state_mask = NO_DATA_PROVIDE;
			while (tmf8801_app0_start(&g_tmf8801Handle, &app_config) == kStatus_Fail)
			{
				PRINTF("tmf8801_app0_start err!\r\n");
			}
            if(g_lcdDeteced){
                LCD_println("  Finish!", 1);
                SysTick_DelayTicks(5000);
            }
		}
		if (cur_idx == 2)
		{
            if(g_lcdDeteced){
                LCD_println("3.Single Measure", 0);
            }
    		tmf8801_measure_once();
	        while (1)
	        {
	        	while ((ret = check_button(6000)) == butNoClick);
	        	if(ret == butLongPress)
	        	{
	        		break;
	        	}
	        	tmf8801_measure_once();
	        }
		}
		if (cur_idx == 3)
		{
            if(g_lcdDeteced){
                LCD_println("4.Cont.  Measure", 0);
            }
	        while ((ret = check_button(6000)) != butLongPress)
	        {
	        	tmf8801_measure_once();
	        	DelayMs(200);
	        }
		}

    }
    TMF8801_DISABLE;

    /* Infinite loops */
    for (;;)
    {
    } /* End infinite loops */
}

void tmf8801_measure_once(void)
{
	uint16_t result_sum=0;
	uint8_t  nCnt;
    uint32_t tm1;

    for(nCnt = 0; nCnt < TMF8801_MEASURE_TIMES; nCnt++){
        g_systickCounter = 1000;
        TimerStart(&tm1);
		while (1)
		{
			if (tmf8801_result_get(&g_tmf8801Handle, &g_tof_result) == kStatus_Success)
			{
				break;
			}
			if (g_systickCounter==0)
			{
				PRINTF("tmf8801_result_get time over!\r\n");
				break;
			}
		}
		//tmf8801_result_display(g_tof_result);
	    PRINTF("量測時間：%d\r\n", TimerPass(&tm1));
	    g_tof_result.distance_peak /= g_calibration_factor;

	    PRINTF("num:%3d 測量距離(mm):%4d 可信度:%2d 校準距離(mm):%4d 校準因子:%.3f\r\n",
	    g_tof_result.result_num,
	    g_tof_result.distance_peak,
	    g_tof_result.reliability,
	    (uint16_t)(g_tof_result.distance_peak / g_calibration_factor),
	    g_calibration_factor);
	    result_sum += g_tof_result.distance_peak;
    }
    if(g_lcdDeteced){
        LCD_setcursor(0, 1);
        LCD_printf("  Dist:%4dmm %2d", g_tof_result.distance_peak, g_tof_result.reliability);
    }
    PRINTF("平均距離(mm): %4d\r\n", result_sum/TMF8801_MEASURE_TIMES);
}
