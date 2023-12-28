/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Transmit and receive data using auto flow control.
 *
 * @copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/

#include <stdio.h>
#include "dependencies.h"
#include "NuMicro.h"

#define __ENABLE_EMAC_FUNCTION_
#define RXBUFSIZE 256
//uint8_t display_page_0_frequency [12] = { 0x6E, 0x34,0x2E,0x76,0x61,0x6C,0x3D,0x33,0x35,0x33,0x30,0x33};
//uint8_t display_page_1_power [7] = { 0x6E, 0x37,0x2E,0x76,0x61,0x6C,0x3D};

uint8_t header [3]={0xFF,0xFF,0xFF};

#ifdef __ENABLE_EMAC_FUNCTION_
// external function declaration
void webserver_init(void);
void webserver_loop(void);

// timer variable
extern uint32_t volatile curTime;
#endif

int timeout_sent=0,broken_sent=0;
int broken_count=0;
char led_test = 0;
int prev_overload,prev2_overload,prev3_overload;
///*
void TMR0_IRQHandler(void)
{
#ifdef __ENABLE_EMAC_FUNCTION_
    curTime++;
#endif	
	if (read_broken_transducer()==1 && broken_sent==0){
		if(read_entered_main_page()==1)broken_count++;
		if(broken_count>10 && read_entered_main_page()==1)broken_sent=1;
		print_page_broken_transducer();
	}
	else if (read_overload_display()==1&&display_page!=7) print_page_overload();
	else if (timeout_sent==0 && read_timeout_occured()){
		timeout_sent=1;
		if (display_page!=10 && display_page!=7 && display_page!=9)print_page_timeout();
	}
	else if (read_two_hand_alarm()==1)print_page_two_hand_alarm();
	else if (display_page==2){
		if (Freq_init==1)print_page_setting_1();
		else print_page_lock_freq();
	}
	else if (display_page==3)print_page_setting_2();
	else if (display_page==4)print_page_SectionVib();
	else if (display_page==5 || display_page==6)print_page_early_after_trigger();
	else if (display_page==7)print_page_head_diagnosis();
	else if (display_page==8)print_page_weld_record();
	else if (display_page==9)print_page_head_down();
	else if (display_page==10)print_page_head_test();
	else TIMER_Delay(TIMER1, 5);//print_page_0();
	
	if(read_timeout_occured()==0)timeout_sent=0;
	if(read_broken_transducer()==0){broken_sent=0;broken_count=0;}
	if(display_page!=10 && read_button_test_display()==1) write_button_test(0);
	
	prev3_overload=prev2_overload;
	prev2_overload=prev_overload;
	prev_overload=read_overload_display();
	if(prev3_overload==1 && prev2_overload==1 && prev_overload==1 && read_overload_display()==0)print_back_to_main_page();
	if(prev3_overload==1 && prev2_overload==1 && prev_overload==0 && read_overload_display()==0)print_back_to_main_page();
	
  TIMER_ClearIntFlag(TIMER0);
	//printf("+--------------------------+\n");
	/*
	if (led_test == 0)
	{
		PC6 = 1;
		led_test = 1;
	}
	else
	{
		PC6 = 0;
		led_test = 0;
	}
	*/
	//print_page_1();
	
}
//*/
int main(void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init UART0 for printf */
    UART0_Init();

    /* Init UART1 */
    UART1_Init();
		
		SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC6MFP_Msk);
    GPIO_SetMode(PC, BIT6, GPIO_MODE_OUTPUT);
    PC6 = 0;
		
		//*
		TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 10);
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
    TIMER_Start(TIMER0);
	//*/
		//while(1){
	//8'h6E,8'h34,8'h2E,8'h76,8'h61,8'h6C,8'h3D,8'h33,8'h35,8'h33,8'h30,8'h33,8'hFF,8'hFF,8'hFF
		//TIMER_Delay(TIMER0, 1000000);
		

		//UART_Write(UART1,display_page_0_frequency,12);
		//UART_Write(UART1,header,3);
		//UART_WRITE(UART1,0xFF);
		//}
		/*
    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
		
    printf("+--------------------------+\n");
    printf("|  Auto-Flow function test  |\n");
    printf("+--------------------------+\n");
		*/
    //AutoFlow_FunctionTest();
		UART1_interrrupt();
		UART0_interrrupt();

#ifdef __ENABLE_EMAC_FUNCTION_
    webserver_init();
#endif

    while (1)
    {
#ifdef __ENABLE_EMAC_FUNCTION_
		webserver_loop();
#else
		;
#endif        
    }
}



