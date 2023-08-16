#include "dependencies.h"
#include <stdio.h>
#include "NuMicro.h"


#define RXBUFSIZE 256
int i=0;
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8SendData[12] = {0};
uint8_t g_u8RecData[RXBUFSIZE]  = {0};

volatile uint32_t g_u32comRbytes = 0;
volatile uint32_t g_u32comRhead  = 0;
volatile uint32_t g_u32comRtail  = 0;
volatile int32_t g_bWait         = TRUE;
volatile int32_t g_i32pointer = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void UART_TEST_HANDLE(void);
void AutoFlow_FunctionTest(void);

uint8_t display_input_command[4] = {0};
uint8_t input_address=0;
uint8_t display_page=0;
uint8_t Freq_init=0;

uint8_t display_page_1_power [7] 		= { 0x6E, 0x37,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_1_freq [7] 		= { 0x6E, 0x34,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_1_force_set [7] 		= { 0x6E, 0x38,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_1_time [7] 		= { 0x6E, 0x33,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_1_trigger [7] 	= { 0x6E, 0x35,0x2E,0x76,0x61,0x6C,0x3D};




//uint8_t header [3]={0xFF,0xFF,0xFF};	
unsigned int amplitude_set=20,freq,mode_set=1,time_set, 
		distance_absolute_set,distance_relative_set=15, energy_set=300, force_set=22,time_set_stage_one_set=500,timeout_set=5000;
unsigned int amplitude_set_display,freq_display,timeout_set_display, 
		distance_absolute_set_display,distance_relative_set_display, energy_set_display, force_set_display,
		current_display,power_read_display,force_display,distance_display, energy_display,pressure_display, overload_display,
		time_set_stage_one_display,standby,hold_time_display,hold_time_set;
//history
unsigned int freq_min,freq_max,freq_start,freq_end,F_start,F_max,P_max,distance_travelled,time_on,distance_reached,distance_hold,timeout_occured;

void write_amplitude_set (unsigned int arg){amplitude_set=arg;}
void write_mode_set (unsigned int arg){mode_set=arg;}
void write_timeout_set (unsigned int arg){timeout_set=arg;}
void write_distance_absolute_set (unsigned int arg){distance_absolute_set=arg;}
void write_distance_relative_set (unsigned int arg){distance_relative_set=arg;}
void write_energy_set (unsigned int arg){energy_set=arg;}
void write_force_set (unsigned int arg){force_set=arg;}
void write_time_set_stage_one_set (unsigned int arg){time_set_stage_one_set=arg;}
void write_hold_time_set(unsigned int arg){hold_time_set=arg;}

//stage 2
unsigned int stage_mode_on_off=0, time_set_stage2, distance_set_stage2, energy_set_stage2, power_set_stage2, amplitudeA_set_stage2, amplitudeB_set_stage2=30;
unsigned int time_set_stage2_display, distance_set_stage2_display, energy_set_stage2_display, power_set_stage2_display,
	  amplitudeA_set_stage2_display, amplitudeB_set_stage2_display;
unsigned int stage2_mode_address_set,stage2_mode_value_display,stage2_mode_address_display;
void write_amplitudeA_set_stage2 (unsigned int arg){amplitude_set=arg;}
void write_amplitudeB_set_stage2 (unsigned int arg){amplitudeB_set_stage2=arg;}
//void write_stage_mode (unsigned int arg){stage_mode_on_off=arg;}
void write_distance_set_stage2 (unsigned int arg){distance_set_stage2=arg;}
void write_power_set_stage2 (unsigned int arg){power_set_stage2=arg;}
void write_energy_set_stage2 (unsigned int arg){energy_set_stage2=arg;}
void write_time_set_stage2 (unsigned int arg){time_set_stage2=arg;}
void write_stage2_mode_address_set (unsigned int arg){stage2_mode_address_set=arg;}
	
//void write_ (unsigned int arg){=arg;}

unsigned int read_amplitude_set_display(void){return amplitude_set_display;}
unsigned int read_mode_set(void) {return mode_set;}
unsigned int read_freq_display(void){return freq_display;}
unsigned int read_timeout_set_display(void){return timeout_set_display;}
unsigned int read_distance_absolute_set_display(void){return distance_absolute_set_display;}
unsigned int read_distance_relative_set_display(void){return distance_relative_set_display;}
unsigned int read_energy_set_display(void){return energy_set_display;}
unsigned int read_force_set_display(void){return force_set_display;}
unsigned int read_current_display(void){return current_display;}
unsigned int read_force_display(void){return force_display;}
unsigned int read_distance_display(void){return distance_display;}
unsigned int read_energy_display(void){return energy_display;}
unsigned int read_pressure_display(void){return pressure_display;}
unsigned int read_time_set_stage_one_display(void){return time_set_stage_one_display;}

unsigned int read_power_read_display() {return power_read_display;}
unsigned int read_freq_min(){return freq_min;}
unsigned int read_freq_max(){return freq_max;}
unsigned int read_freq_start(){return freq_start;}
unsigned int read_freq_end(){return freq_end;}
unsigned int read_F_start(){return F_start;}
unsigned int read_F_max(){return F_max;}
unsigned int read_P_max(){return P_max;}
unsigned int read_distance_travelled(){return distance_travelled;}
unsigned int read_distance_reached(){return distance_reached;}
unsigned int read_time_on(){return time_on;}
unsigned int read_timeout_occured() {return timeout_occured;}

unsigned int read_distance_hold() {return distance_hold;}
unsigned int read_collapse_hold() {return (distance_travelled+distance_hold);}
unsigned int read_absolute_hold() {return (distance_reached+distance_hold);}


unsigned int read_amplitudeA_set_stage2_display(){return amplitude_set_display;}
unsigned int read_amplitudeB_set_stage2_display(){return amplitudeB_set_stage2_display;}
unsigned int read_stage2_mode_address_display() {return stage2_mode_address_display;}
unsigned int read_time_set_stage2_display(){if(stage2_mode_address_display==3)return stage2_mode_value_display;else return 0;}
unsigned int read_distance_set_stage2_display(){if(stage2_mode_address_display==4)return stage2_mode_value_display;else return 0;}
//unsigned int read_power_set_stage2_display(){return power_set_stage2_display;}
unsigned int read_energy_set_stage2_display(){if(stage2_mode_address_display==6)return stage2_mode_value_display;else return 0;}
unsigned int read_stage2_mode_address_set() {return stage2_mode_address_set;}
unsigned int read_hold_time_display() {return hold_time_display;}


//unsigned int read_(){return ;}



unsigned int temp_time_picker;



volatile int32_t g_i32pointer_0 = 0;
uint16_t FPGA_length=59;
uint8_t FPGA_input[59] = {0};
uint16_t FPGA_address=0;

//fpga_to_mcu
void fpga_to_mcu(void){//FPGA => MCU

	if(FPGA_input[0]==0xFF && FPGA_input[1]==0xFF && FPGA_input[2]==0xFF && FPGA_input[3]==0xFF){
		amplitude_set_display							=FPGA_input[4];
		timeout_set_display 							=(FPGA_input[5]<<8)|(FPGA_input[6]);
		distance_relative_set_display	=(FPGA_input[7]<<8)|(FPGA_input[8]);
		distance_absolute_set_display	=(FPGA_input[9]<<8)|(FPGA_input[10]);
		force_set_display							=(FPGA_input[11]<<8)|(FPGA_input[12]);
		energy_set_display						=(FPGA_input[13]<<8)|(FPGA_input[14]);
		freq_display									=(FPGA_input[15]<<8)|(FPGA_input[16]);
		current_display								=(FPGA_input[17]<<8)|(FPGA_input[18]);
		power_read_display						=(FPGA_input[19]<<8)|(FPGA_input[20]);
		force_display									=(FPGA_input[21]<<8)|(FPGA_input[22]);
		distance_display							=(FPGA_input[23]<<8)|(FPGA_input[24]);
		energy_display								=(FPGA_input[25]<<8)|(FPGA_input[26]);
		pressure_display							=FPGA_input[27];
		
		timeout_occured								=(FPGA_input[28]>>4)& 1;
		overload_display							=(FPGA_input[28]>>5)& 1;
		standby												=(FPGA_input[28]>>6)& 1;
		Freq_init											=(FPGA_input[28]>>7)& 1;
		
		freq_min											=(FPGA_input[29]<<8)|(FPGA_input[30]);
		freq_max											=(FPGA_input[31]<<8)|(FPGA_input[32]);
		freq_start										=(FPGA_input[33]<<8)|(FPGA_input[34]);
		freq_end											=(FPGA_input[35]<<8)|(FPGA_input[36]);
		F_start												=(FPGA_input[37]<<8)|(FPGA_input[38]);
		F_max													=(FPGA_input[39]<<8)|(FPGA_input[40]);
		P_max													=(FPGA_input[41]<<8)|(FPGA_input[42]);
		time_on												=(FPGA_input[43]<<8)|(FPGA_input[44]);
		distance_travelled						= (FPGA_input[45]<<8)|(FPGA_input[46]);//(FPGA_input[45]<<8)|(FPGA_input[46]);
		distance_reached						= (FPGA_input[47]<<8)|(FPGA_input[48]);
		
		time_set_stage_one_display		= (FPGA_input[49]<<8)|(FPGA_input[50]);
		
		amplitudeB_set_stage2_display	= FPGA_input[51];
		stage2_mode_address_display	= FPGA_input[52];
		stage2_mode_value_display = (FPGA_input[53]<<8)|(FPGA_input[54]);
		hold_time_display = (FPGA_input[55]<<8)|(FPGA_input[56]);
		
		distance_hold= FPGA_input[57];
	}
}
int time_set_zero=0;
unsigned int temp_distance_relative_set,temp_distance_absolute_set, temp_energy_set, temp_time_set_stage_one_set,temp_timeout_set;
unsigned int temp_value_mode_stage2;
unsigned int temp_stage2_address;
void mcu_to_fpga(void){//MCU => FPGA
	
	if (display_page==9)temp_timeout_set=0;
	else if (display_page==10)temp_timeout_set=2000;
	else temp_timeout_set=timeout_set;
	
	if (mode_set==2){
		temp_distance_relative_set=distance_relative_set;
		temp_distance_absolute_set=0;
		temp_energy_set=0;;
		temp_time_set_stage_one_set=0;
	}
	else if (mode_set==3){
		temp_distance_relative_set=0;
		temp_distance_absolute_set=distance_absolute_set;
		temp_energy_set=0;;
		temp_time_set_stage_one_set=0;
	}
	else if (mode_set==5){
		temp_distance_relative_set=0;
		temp_distance_absolute_set=0;
		temp_energy_set=energy_set;
		temp_time_set_stage_one_set=0;
	}
	else {
		temp_time_set_stage_one_set=time_set_stage_one_set;
		temp_distance_relative_set=0;
		temp_distance_absolute_set=0;
		temp_energy_set=0;
	}
		
	if(stage2_mode_address_set==3){
		temp_value_mode_stage2=time_set_stage2;
		temp_stage2_address=0xD3;
	}
	else if (stage2_mode_address_set==4){
		temp_value_mode_stage2=distance_set_stage2;
		temp_stage2_address=0xD4;
	}
	else if (stage2_mode_address_set==6){
		temp_value_mode_stage2=energy_set_stage2;	
		temp_stage2_address=0xD6;
	}
	else temp_value_mode_stage2=0;	
	
	if(Freq_init==1 && standby==1 && FPGA_input[0]==0xFF && FPGA_input[1]==0xFF && FPGA_input[2]==0xFF && FPGA_input[3]==0xFF && FPGA_input[58]==0x68){
		if(amplitude_set_display!=amplitude_set){
			UART_WRITE(UART0,0xC0);
			UART_WRITE(UART0,amplitude_set);
			UART_WRITE(UART0,0xFF);
			PC6 = 1;
		}
		//display input temp time*10
		//else if (temp_time_picker!=time_set_display){
		else if (temp_timeout_set!=timeout_set_display){
			UART_WRITE(UART0,0xCD);
			//UART_WRITE(UART0,temp_time_picker& ~(~0U << 8));
			//UART_WRITE(UART0,temp_time_picker>>8);
			UART_WRITE(UART0,temp_timeout_set& ~(~0U << 8));
			UART_WRITE(UART0,temp_timeout_set>>8);
			UART_WRITE(UART0,0xFF);
			PC6 = 1;
	}
		
		else if (temp_time_set_stage_one_set!=time_set_stage_one_display){
			UART_WRITE(UART0,0xC3);
			//UART_WRITE(UART0,temp_time_picker& ~(~0U << 8));
			//UART_WRITE(UART0,temp_time_picker>>8);
			UART_WRITE(UART0,temp_time_set_stage_one_set& ~(~0U << 8));
			UART_WRITE(UART0,temp_time_set_stage_one_set>>8);
			UART_WRITE(UART0,0xFF);
			PC6 = 1;
	}
		
		else if (temp_distance_relative_set!=distance_relative_set_display){
			UART_WRITE(UART0,0xC4);
			UART_WRITE(UART0,temp_distance_relative_set& ~(~0U << 8));
			UART_WRITE(UART0,temp_distance_relative_set>>8);
			UART_WRITE(UART0,0xFF);
			
			PC6 = 1;
	}	
		else if (distance_absolute_set_display!=temp_distance_absolute_set){
			UART_WRITE(UART0,0xC5);
			UART_WRITE(UART0,temp_distance_absolute_set& ~(~0U << 8));
			UART_WRITE(UART0,temp_distance_absolute_set>>8);
			UART_WRITE(UART0,0xFF);
			PC6 = 1;
	}
		else if (force_set!=force_set_display){
			UART_WRITE(UART0,0xC6);
			UART_WRITE(UART0,force_set& ~(~0U << 8));
			UART_WRITE(UART0,force_set>>8);
			UART_WRITE(UART0,0xFF);
			PC6 = 1;
	}
		else if (temp_energy_set!=energy_set_display){
			UART_WRITE(UART0,0xC7);
			UART_WRITE(UART0,temp_energy_set& ~(~0U << 8));
			UART_WRITE(UART0,temp_energy_set>>8);
			UART_WRITE(UART0,0xFF);
			PC6 = 1;
	}
		else if (hold_time_set!=hold_time_display){
			UART_WRITE(UART0,0xC8);
			UART_WRITE(UART0,hold_time_set& ~(~0U << 8));
			UART_WRITE(UART0,hold_time_set>>8);
			UART_WRITE(UART0,0xFF);
			PC6 = 1;
	}
		else if (amplitudeB_set_stage2!=amplitudeB_set_stage2_display){
			UART_WRITE(UART0,0xD2);
			UART_WRITE(UART0,amplitudeB_set_stage2);
			UART_WRITE(UART0,0xFF);
			PC6 = 1;
	}
		//stage 2/////////////////////////////////////////////////////////////////////////
		else if (stage2_mode_address_set==0 && stage2_mode_address_display!=0){
			UART_WRITE(UART0,0xD0);
			UART_WRITE(UART0,0x00);
			UART_WRITE(UART0,0xFF);
			PC6 = 1;
	}	
		
		else if (stage2_mode_address_set!=0 && (stage2_mode_address_set!=stage2_mode_address_display || temp_value_mode_stage2!=stage2_mode_value_display)){
			UART_WRITE(UART0,temp_stage2_address);
			UART_WRITE(UART0,temp_value_mode_stage2& ~(~0U << 8));
			UART_WRITE(UART0,temp_value_mode_stage2>>8);
			//UART_WRITE(UART0,);
			UART_WRITE(UART0,0xFF);
			PC6 = 1;
	}		
		else PC6 = 0;
		
}}
//0-3 header
//4 	 power set
//5  6  time set
//7  8  distance relative set
//9  10 distance absolute set
//11 12 force set
//13 14 energy set 
//15 16 freq  	
//17 18 current      
//19 20 power
//21 22 force N
//23 24 distance read
//25 26 energy read
//27 pressure
//28 overloadq
//end header
int time_wait_uart0;
void UART0_TEST_HANDLE(void)
{		
	uint8_t u8InChar_0=0xFF;
  uint32_t u32IntSts_0= UART0->INTSTS;
	int j;
	//TIMER_ClearIntFlag(TIMER0);
    if(u32IntSts_0 & UART_INTSTS_RDAINT_Msk)
    {
				FPGA_address=0;
				j=0;
				for(j = 0; j < FPGA_length; j++){FPGA_input[j]=0; }
        while( (!UART_GET_RX_EMPTY(UART0)) )
        {	TIMER_Delay(TIMER1, 5);
					u8InChar_0 = UART_READ(UART0);    /* Rx trigger level is 1 byte*/
          g_i32pointer_0++;
					FPGA_input[FPGA_address] = u8InChar_0;
					//printf("RAW: %x ", FPGA_input[FPGA_address]);
					FPGA_address++;
					/*
					while(UART_GET_RX_EMPTY(UART0) && time_wait_uart0<5){
						TIMER_Delay(TIMER1, 10);
						time_wait_uart0++;
					}
					time_wait_uart0=0;*/
          }
				fpga_to_mcu();
					
				mcu_to_fpga();
				/*	
				printf("UART0  ");
				j=0;
				for(j = 0; j< FPGA_length; j++){printf("%x ", FPGA_input[j]); }
				printf("\n");
				*/	
				
		}
}

int time_wait;

void UART1_TEST_HANDLE(void)
{		
	uint8_t u8InChar=0xFF;
  uint32_t u32IntSts= UART1->INTSTS;
    if(u32IntSts & UART_INTSTS_RDAINT_Msk)
    {
        //printf("\nInput:");
				input_address=0;
				i=0;
				for(i = 0; i < 4; i++){display_input_command[i]=0; }
        while( (!UART_GET_RX_EMPTY(UART1)) )
        {
					//TIMER_Delay(TIMER1, 80);
					u8InChar = UART_READ(UART1);    /* Rx trigger level is 1 byte*/
          //g_i32pointer++;
          //printf("%x ", u8InChar);
					if(input_address<4)display_input_command[input_address] = u8InChar;
					input_address++;
					while(UART_GET_RX_EMPTY(UART1) && time_wait<5){
						TIMER_Delay(TIMER1, 30);
						time_wait++;
					}
					time_wait=0;
				}
				/*
				printf("UART1  ");
				i=0;
				for(i = 0; i< 4; i++){printf("%x ", display_input_command[i]); }
				printf("\n");*/
				display_to_mcu();}
}

void UART1_interrrupt(void){
	UART1->MODEM |= UART_MODEM_RTSACTLV_Msk; // Set RTS high level trigger
  UART1->MODEMSTS |= UART_MODEMSTS_CTSACTLV_Msk; // Set CTS high level trigger
  UART1->FIFO = (UART1->FIFO &~ UART_FIFO_RTSTRGLV_Msk) | UART_FIFO_RTSTRGLV_14BYTES;
	//UART1->INTEN |= UART_INTEN_ATORTSEN_Msk | UART_INTEN_ATOCTSEN_Msk;
	UART_ENABLE_INT(UART1, (UART_INTEN_RDAIEN_Msk | UART_INTEN_RLSIEN_Msk | UART_INTEN_RXTOIEN_Msk));
	UART_SetTimeoutCnt(UART1,0xFF);//0x3E);

  NVIC_EnableIRQ(UART1_IRQn);
	//printf("Starting UART1\n");
}

void UART0_interrrupt(void){
	UART0->MODEM |= UART_MODEM_RTSACTLV_Msk; // Set RTS high level trigger
  UART0->MODEMSTS |= UART_MODEMSTS_CTSACTLV_Msk; // Set CTS high level trigger
  UART0->FIFO = (UART0->FIFO &~ UART_FIFO_RTSTRGLV_Msk) | UART_FIFO_RTSTRGLV_14BYTES;
	//UART1->INTEN |= UART_INTEN_ATORTSEN_Msk | UART_INTEN_ATOCTSEN_Msk;
	UART_ENABLE_INT(UART0, (UART_INTEN_RDAIEN_Msk | UART_INTEN_RLSIEN_Msk | UART_INTEN_RXTOIEN_Msk));
	UART_SetTimeoutCnt(UART0,0xFF);//0x3E);

  NVIC_EnableIRQ(UART0_IRQn);
	//printf("Starting UART0\n");
}






















void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    /* Enable HXT*/
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk; // XTAL12M (HXT) Enabled

    /* Waiting clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(192000000);
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2); // PCLK divider set 2

    /* Enable IP clock */
    CLK->APBCLK0 |= CLK_APBCLK0_UART0CKEN_Msk; // UART0 Clock Enable
    CLK->APBCLK0 |= CLK_APBCLK0_UART1CKEN_Msk; // UART1 Clock Enable

    /* Select IP clock source */
    /* Select UART0 clock source is HXT */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_UART0SEL_Msk) | (0x0 << CLK_CLKSEL1_UART0SEL_Pos);
    /* Select UART1 clock source is HXT */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_UART1SEL_Msk) | (0x0 << CLK_CLKSEL1_UART1SEL_Pos);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /* Set GPB multi-function pins for UART0 RXD and TXD */
		/*
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
		*/
		SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA6MFP_Msk | SYS_GPA_MFPL_PA7MFP_Msk);
    SYS->GPA_MFPL |= (SYS_GPA_MFPL_PA6MFP_UART0_RXD | SYS_GPA_MFPL_PA7MFP_UART0_TXD);
		
    /* Set PA multi-function pins for UART1 TXD, RXD, CTS and RTS */
    SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA0MFP_Msk | SYS_GPA_MFPL_PA1MFP_Msk |
                       SYS_GPA_MFPL_PA2MFP_Msk | SYS_GPA_MFPL_PA3MFP_Msk);
    SYS->GPA_MFPL |= (0x8 << SYS_GPA_MFPL_PA0MFP_Pos) | (0x8 << SYS_GPA_MFPL_PA1MFP_Pos) |
                     (0x8 << SYS_GPA_MFPL_PA2MFP_Pos) | (0x8 << SYS_GPA_MFPL_PA3MFP_Pos);

    /* Lock protected registers */
    SYS_LockReg();
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
		
		CLK_EnableModuleClock(TMR1_MODULE);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HXT, 0);
			

}

void UART0_Init(void)
{
    UART_Open(UART0, 115200);
}

void UART1_Init(void)
{
    UART_Open(UART1, 115200);
}


/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UART1_IRQHandler(void)
{
    UART1_TEST_HANDLE();
}

void UART0_IRQHandler(void)
{
    UART0_TEST_HANDLE();
}

/*---------------------------------------------------------------------------------------------------------*/
/* UART Callback function                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*  UART Function Test                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

void AutoFlow_FunctionTest()
{
    uint8_t u8Item;
    uint32_t u32i;
    printf("+-----------------------------------------------------------+\n");
    printf("|     Pin Configure                                         |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|  _______                                      _______     |\n");
    printf("| |       |                                    |       |    |\n");
    printf("| |Master |---TXD1    <============>    RXD1---| Slave |    |\n");
    printf("| |       |---RTS1    <============>    CTS1---|       |    |\n");
    printf("| |_______|---CTS1    <============>    RTS1---|_______|    |\n");
    printf("|                                                           |\n");
    printf("+-----------------------------------------------------------+\n\n");

    /* Set Trigger Level */
    UART1->MODEM |= UART_MODEM_RTSACTLV_Msk; // Set RTS high level trigger
    UART1->MODEMSTS |= UART_MODEMSTS_CTSACTLV_Msk; // Set CTS high level trigger
    UART1->FIFO = (UART1->FIFO &~ UART_FIFO_RTSTRGLV_Msk) | UART_FIFO_RTSTRGLV_14BYTES;

    /* Enable RTS and CTS auto flow control */
    UART1->INTEN |= UART_INTEN_ATORTSEN_Msk | UART_INTEN_ATOCTSEN_Msk;

    printf("+-----------------------------------------------------------+\n");
    printf("|       AutoFlow Function Test                              |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|  Description :                                            |\n");
    printf("|    The sample code needs two boards. One is Master and    |\n");
    printf("|    the other is slave. Master will send 1k bytes data     |\n");
    printf("|    to slave.Slave will check if received data is correct  |\n");
    printf("|    after getting 1k bytes data.                           |\n");
    printf("|  Please select Master or Slave test                       |\n");
    printf("|  [0] Master    [1] Slave                                  |\n");
    printf("+-----------------------------------------------------------+\n\n");
    u8Item = getchar();


    if(u8Item=='0')
    {
        for(u32i=0; u32i<(RXBUFSIZE-1); u32i++)
        {
            UART_WRITE(UART1,((u32i+1)&0xFF));

            while(!(UART1->FIFOSTS & UART_FIFOSTS_TXEMPTYF_Msk));
        }
        printf("\n Transmit Done\n");
    }
    else
    {
        g_i32pointer = 0;

        /* Enable RDA\RLS\RTO Interrupt  */
        UART_ENABLE_INT(UART1, (UART_INTEN_RDAIEN_Msk | UART_INTEN_RLSIEN_Msk | UART_INTEN_RXTOIEN_Msk));

        /* Set RX Trigger Level = 8 */
        UART1->FIFO = (UART1->FIFO &~ UART_FIFO_RFITL_Msk) | UART_FIFO_RFITL_8BYTES;

        /* Set Timeout time 0x3E bit-time */
        UART_SetTimeoutCnt(UART1,0x3E);

        NVIC_EnableIRQ(UART1_IRQn);

        printf("Starting to receive %d bytes data...\n", RXBUFSIZE);

        while(g_i32pointer<(RXBUFSIZE-1))
        {
            //printf("%d\r",g_i32pointer);
        }

        /* Compare Data */
        for(u32i=0; u32i!=(RXBUFSIZE-1); u32i++)
        {
            printf("%x2", (int)g_u8RecData[u32i]);
        }
        printf("\n Receive OK & Check OK\n");
    }

    NVIC_DisableIRQ(UART1_IRQn);

    UART_DISABLE_INT(UART1, (UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk | UART_INTEN_RXTOIEN_Msk));

}
