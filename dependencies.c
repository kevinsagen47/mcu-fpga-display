#include "dependencies.h"
#include <stdio.h>
#include "NuMicro.h"


#define RXBUFSIZE 256

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

uint8_t display_page_1_power [7] 		= { 0x6E, 0x37,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_1_freq [7] 		= { 0x6E, 0x34,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_1_force_set [7] 		= { 0x6E, 0x38,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_1_time [7] 		= { 0x6E, 0x33,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_1_trigger [7] 	= { 0x6E, 0x35,0x2E,0x76,0x61,0x6C,0x3D};




//uint8_t header [3]={0xFF,0xFF,0xFF};	
unsigned int power_set=20,freq,display_trigger=1,time_set, 
		distance_absolute_set,distance_relative_set, energy_set, force_set=200,timeout_set;
unsigned int power_set_display,freq_display,time_set_display, 
		distance_absolute_set_display,distance_relative_set_display, energy_set_display, force_set_display,
		current_display,power_read_display,force_display,distance_display, energy_display,pressure_display, overload_display;
//history
unsigned int freq_min,freq_max,freq_start,freq_end,F_start,F_max,P_max,distance_travelled,time_on;

unsigned int temp_time_picker;
int i=0;
uint8_t bcd_array[5]={0};
void binary_to_bcd_array(int variable){
	bcd_array[0]=0x30+((variable/10000)%10);
	bcd_array[1]=0x30+((variable/1000)%10);
	bcd_array[2]=0x30+((variable/100)%10);
	bcd_array[3]=0x30+((variable/10)%10);
	bcd_array[4]=0x30+((variable/1)%10);
}
//62 5B 33 5D 2E 74 78 74 3D 22 B5 4A A6 D5 22 ff ff ff b[3] joule text
//62 5B 33 5D 2E 74 78 74 3D 22 AE C9 B6 A1 22 ff ff ff b[3] time
//62 5B 33 5D 2E 74 78 74 3D 22 B6 5A C2 F7 22 ff ff ff b[3] distance 
//62 5B 34 5D 2E 74 78 74 3D 22 4A 22 ff ff ff b[4].txt="J"
//62 5B 34 5D 2E 74 78 74 3D 22 53 22 ff ff ff b[4].txt="S"
//62 5B 34 5D 2E 74 78 74 3D 22 6D 6D 22 ff ff ff b[4].txt="mm"
//62 5B 33 35 5D 2E 76 61 6C 3D 32 ff ff ff b[35].val=2
//62 5B 32 38 5D 2E 76 61 6C 3D b[28].val=
uint8_t display_page_setting_1_time [53] 			= {0x62,0x5B,0x33,0x35,0x5D,0x2E,0x76,0x61,0x6C,0x3D,0x30,0xff,0xff,0xff//set drop down to time
																								,0x74,0x4D,0x6F,0x64,0x65,0x2E,0x74,0x78,0x74,0x3D,0x22,0xAE,0xC9,0xB6,0xA1,0x22,0xff,0xff,0xff//print shijian
																								,0x74,0x4D,0x6F,0x64,0x65,0x55,0x6E,0x69,0x74,0x2E,0x74,0x78,0x74,0x3D,0x22,0x53,0x22,0xff,0xff,0xff};//print "S"

uint8_t display_page_setting_1_dist_rel [54] = {0x62,0x5B,0x33,0x35,0x5D,0x2E,0x76,0x61,0x6C,0x3D,0x31,0xff,0xff,0xff//set drop down to relative
																										,0x74,0x4D,0x6F,0x64,0x65,0x2E,0x74,0x78,0x74,0x3D,0x22,0xB6,0x5A,0xC2,0xF7,0x22,0xff,0xff,0xff//print jvli
																										,0x74,0x4D,0x6F,0x64,0x65,0x55,0x6E,0x69,0x74,0x2E,0x74,0x78,0x74,0x3D,0x22,0x6D,0x6D,0x22,0xff,0xff,0xff};//print "mm"																								

uint8_t display_page_setting_1_dist_abs [54] = {0x62,0x5B,0x33,0x35,0x5D,0x2E,0x76,0x61,0x6C,0x3D,0x32,0xff,0xff,0xff//set drop down to absolute
																										,0x74,0x4D,0x6F,0x64,0x65,0x2E,0x74,0x78,0x74,0x3D,0x22,0xB6,0x5A,0xC2,0xF7,0x22,0xff,0xff,0xff//print jvli
																										,0x74,0x4D,0x6F,0x64,0x65,0x55,0x6E,0x69,0x74,0x2E,0x74,0x78,0x74,0x3D,0x22,0x6D,0x6D,0x22,0xff,0xff,0xff};//print "mm"

uint8_t display_page_setting_1_dist_energy[53] = {0x62,0x5B,0x33,0x35,0x5D,0x2E,0x76,0x61,0x6C,0x3D,0x34,0xff,0xff,0xff//set drop down to energy
																										,0x74,0x4D,0x6F,0x64,0x65,0x2E,0x74,0x78,0x74,0x3D,0x22,0xB5,0x4A,0xA6,0xD5,0x22,0xff,0xff,0xff//print jiaoer
																										,0x74,0x4D,0x6F,0x64,0x65,0x55,0x6E,0x69,0x74,0x2E,0x74,0x78,0x74,0x3D,0x22,0x4A,0x22,0xff,0xff,0xff};//print "J"
uint8_t display_page_setting_1_trigger_val [10] ={0x62,0x5B,0x32,0x38,0x5D,0x2E,0x76,0x61,0x6C,0x3D};//b[28].val=

uint8_t display_page_setting_1_force_set [10] ={0x62,0x5B,0x31,0x31,0x5D,0x2E,0x76,0x61,0x6C,0x3D};//b[11].val=

uint8_t display_page_setting_1_power_set [10] ={0x62,0x5B,0x33,0x38,0x5D,0x2E,0x76,0x61,0x6C,0x3D};//b[38].val=
																										
void print_page_setting_1(){
		if (display_trigger==2 ){
			UART_Write(UART1,display_page_setting_1_dist_rel ,54);
			UART_Write(UART1,display_page_setting_1_trigger_val ,10);
			binary_to_bcd_array(distance_relative_set_display);
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		else if (display_trigger==3){
			UART_Write(UART1,display_page_setting_1_dist_abs ,54);
			UART_Write(UART1,display_page_setting_1_trigger_val ,10);
			binary_to_bcd_array(distance_absolute_set_display);
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		else if (display_trigger==5){
			UART_Write(UART1,display_page_setting_1_dist_energy,53);
			UART_Write(UART1,display_page_setting_1_trigger_val ,10);
			binary_to_bcd_array(energy_set_display*100);
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		else {
			UART_Write(UART1,display_page_setting_1_time,53);
			UART_Write(UART1,display_page_setting_1_trigger_val ,10);
			binary_to_bcd_array(time_set_display/10);
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		
		UART_Write(UART1,display_page_setting_1_force_set,10);
		binary_to_bcd_array(force_set_display);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_setting_1_power_set,10);
		binary_to_bcd_array(power_set_display);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
}
uint8_t display_page_setting_2_timeout [9] ={0x62,0x5B,0x36,0x5D,0x2E,0x76,0x61,0x6C,0x3D};//b[6].val=
void print_page_setting_2(){
		UART_Write(UART1,display_page_setting_2_timeout,9);
		if(display_trigger!=1)binary_to_bcd_array(time_set_display);
		else binary_to_bcd_array(timeout_set);
		UART_Write(UART1,bcd_array,2);
		UART_Write(UART1,header,3);
		
}

uint8_t display_page_head_down_pressure [9] ={0x62,0x5B,0x38,0x5D,0x2E,0x76,0x61,0x6C,0x3D};//b[8].val=
uint8_t display_page_head_down_force_read [9] ={0x62,0x5B,0x39,0x5D,0x2E,0x76,0x61,0x6C,0x3D};//b[9].val=
uint8_t display_page_head_down_distance_read [11] ={0x62,0x5B,0x31,0x33,0x5D,0x2E,0x74,0x78,0x74,0x3D,0x22};//b[13].txt="
uint8_t one_array_temp[1]={0};
void print_page_head_down(){
		UART_Write(UART1,display_page_head_down_pressure,9);
		binary_to_bcd_array(pressure_display);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_head_down_force_read,9);
		binary_to_bcd_array(force_display);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
		UART_Write(UART1,display_page_head_down_distance_read,11);
		binary_to_bcd_array(distance_display);
	
		UART_Write(UART1,bcd_array,3);
		one_array_temp[0] = 0x2E;
		UART_Write(UART1,one_array_temp,1);//decimal point
		one_array_temp[0] = bcd_array[3];
		UART_Write(UART1,one_array_temp,1);//first decimal place
		one_array_temp[0] = bcd_array[4];
		UART_Write(UART1,one_array_temp,1);//second decimal place
		one_array_temp[0] = 0x22;
		UART_Write(UART1,one_array_temp,1);//second decimal place
		UART_Write(UART1,header,3);
}

unsigned int freq_min,freq_max,freq_start,freq_end,F_start,F_max,P_max,distance_travelled,time_on;
uint8_t display_page_2_freq_start [7] 				= { 0x6E, 0x31,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_freq_end [7] 					= { 0x6E, 0x32,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_freq_max[7] 						= { 0x6E, 0x33,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_freq_min [7] 					= { 0x6E, 0x34,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_P_max [7] 							= { 0x6E, 0x35,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_energy [7] 						= { 0x6E, 0x36,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_time_on [7] 						= { 0x6E, 0x37,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_distance_travelled[7]	= { 0x6E, 0x38,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_F_start [7] 						= { 0x6E, 0x39,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_F_max [8] 							= { 0x6E, 0x31,0x30,0x2E,0x76,0x61,0x6C,0x3D};


void print_page_weld_record(){
		UART_Write(UART1,display_page_2_freq_start,7);
		binary_to_bcd_array(freq_start);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
		UART_Write(UART1,display_page_2_freq_end,7);
		binary_to_bcd_array(freq_end);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
		UART_Write(UART1,display_page_2_freq_max,7);
		binary_to_bcd_array(freq_max);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
		UART_Write(UART1,display_page_2_freq_min,7);
		binary_to_bcd_array(freq_min);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
		UART_Write(UART1,display_page_2_P_max,7);
		binary_to_bcd_array(P_max);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
		UART_Write(UART1,display_page_2_energy,7);
		binary_to_bcd_array(energy_display);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_2_time_on,7);
		binary_to_bcd_array(time_on);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_2_distance_travelled,7);
		binary_to_bcd_array(distance_travelled);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_2_F_start,7);
		binary_to_bcd_array(F_start);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_2_F_max,8);
		binary_to_bcd_array(F_max);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
}

int test,energy_set_temp,timer_mode_set;
void update_variable(){//display HMI => MCU
				if(display_input_command[2]==0xFF || display_input_command[3]==0xFF){
					switch(display_input_command[0]) {
					case 0xAA:
						display_page = display_input_command[1];
						break;
					case 0xc0:
						power_set= display_input_command[1];
						break;
					case 0xc2:
						display_trigger= display_input_command[1];
						if(display_trigger==1){
								distance_relative_set= 0;
								distance_absolute_set= 0;
								energy_set 				=0;}
						
						break;
					case 0xc3:
						timer_mode_set= ((display_input_command[2]<<8)|(display_input_command[1]))*10;
						if(display_trigger==1){
								distance_relative_set= 0;
								distance_absolute_set= 0;
								energy_set 				=0;}
						break;
					case 0xc4:
						distance_relative_set= (display_input_command[2]<<8)|(display_input_command[1]);
						distance_absolute_set=0;
						energy_set=0;
						break;
					case 0xc5:
						distance_relative_set= 0;
						distance_absolute_set=(display_input_command[2]<<8)|(display_input_command[1]);
						energy_set=0;
						break;
					case 0xca:
						force_set=(display_input_command[2]<<8)|(display_input_command[1]);
						break;
					case 0xc7:
						distance_relative_set= 0;
						distance_absolute_set=0;
						energy_set_temp=(display_input_command[2]<<8)|(display_input_command[1]);
						energy_set=energy_set_temp/100;
						break;
					case 0xcd://timeout
						timeout_set=((display_input_command[2]<<8)|(display_input_command[1]))*1000;
						//time_set=((display_input_command[2]<<8)|(display_input_command[1]))*1000;
						//timeout_set=time_set;
						break;
					default:
						test=0;
						break;
				}
					if((distance_absolute_set!=0 || distance_relative_set!=0 || energy_set!=0)&& time_set==0) time_set=2000;
					///*
					if(display_trigger==1) time_set=timer_mode_set;
					else time_set=timeout_set;
					if (display_page==9)time_set=0;
				}
				
				/*
					printf ("page %d\n",display_page);
					printf ("power setting %d\n",power_set);
					printf ("trigger %d\n",display_trigger);
					printf ("time set %d\n",time_set);
					printf ("absolute %d\n",distance_absolute_set);
					printf ("relative %d\n",distance_relative_set);
					printf ("energy %d\n",energy_set);*/
}



volatile int32_t g_i32pointer_0 = 0;
uint16_t FPGA_length=48;
uint8_t FPGA_input[48] = {0};
uint16_t FPGA_address=0;


void update_display_variable(){//FPGA => MCU

	if(FPGA_input[0]==0xFF && FPGA_input[1]==0xFF && FPGA_input[2]==0xFF && FPGA_input[3]==0xFF){
		power_set_display							=FPGA_input[4];
		time_set_display 							=(FPGA_input[5]<<8)|(FPGA_input[6]);
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
		overload_display							=FPGA_input[28];
		
		freq_min											=(FPGA_input[29]<<8)|(FPGA_input[30]);
		freq_max											=(FPGA_input[31]<<8)|(FPGA_input[32]);
		freq_start										=(FPGA_input[33]<<8)|(FPGA_input[34]);
		freq_end											=(FPGA_input[35]<<8)|(FPGA_input[36]);
		F_start												=(FPGA_input[37]<<8)|(FPGA_input[38]);
		F_max													=(FPGA_input[39]<<8)|(FPGA_input[40]);
		P_max													=(FPGA_input[41]<<8)|(FPGA_input[42]);
		time_on												=(FPGA_input[43]<<8)|(FPGA_input[44]);
		distance_travelled						= (FPGA_input[45]<<8)|(FPGA_input[46]);//(FPGA_input[45]<<8)|(FPGA_input[46]);
	}
}
int time_set_zero=0;
void update_different_variables(){//MCU => FPGA
	if(FPGA_input[0]==0xFF && FPGA_input[1]==0xFF && FPGA_input[2]==0xFF && FPGA_input[3]==0xFF){
		if(power_set_display!=power_set){
			UART_WRITE(UART0,0xC0);
			UART_WRITE(UART0,power_set);
			UART_WRITE(UART0,0xFF);
			PC6 = 1;
		}
		//display input temp time*10
		//else if (temp_time_picker!=time_set_display){
		
		else if (time_set!=time_set_display){
			UART_WRITE(UART0,0xC3);
			//UART_WRITE(UART0,temp_time_picker& ~(~0U << 8));
			//UART_WRITE(UART0,temp_time_picker>>8);
			UART_WRITE(UART0,time_set& ~(~0U << 8));
			UART_WRITE(UART0,time_set>>8);
			UART_WRITE(UART0,0xFF);
			PC6 = 1;
	}
		
		else if (distance_relative_set!=distance_relative_set_display){
			UART_WRITE(UART0,0xC4);
			UART_WRITE(UART0,distance_relative_set& ~(~0U << 8));
			UART_WRITE(UART0,distance_relative_set>>8);
			UART_WRITE(UART0,0xFF);
			PC6 = 1;
	}	
		else if (distance_absolute_set_display!=distance_absolute_set){
			UART_WRITE(UART0,0xC5);
			UART_WRITE(UART0,distance_absolute_set& ~(~0U << 8));
			UART_WRITE(UART0,distance_absolute_set>>8);
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
		else if (energy_set!=energy_set_display){
			UART_WRITE(UART0,0xC7);
			UART_WRITE(UART0,energy_set& ~(~0U << 8));
			UART_WRITE(UART0,energy_set>>8);
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
//28 overload
//end header
void UART0_TEST_HANDLE()
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
          }
				update_display_variable();
					
				update_different_variables();
				//printf("UART0  ");
				//j=0;
				//for(j = 0; j< FPGA_length; j++){printf("%x ", FPGA_input[j]); }
				//printf("\n");
		}
}

int time_wait;

void UART1_TEST_HANDLE()
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
				update_variable();}
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

void UART0_Init()
{
    UART_Open(UART0, 115200);
}

void UART1_Init()
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
