#include <stdio.h>
#include "dependencies.h"
#include "NuMicro.h"
#define RXBUFSIZE 256


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
uint8_t display_page_setting_1_time [60] 			= {0x63,0X62,0X4D,0X6F,0X64,0X65,0X53,0X65,0X6C,0X65,0X63,0X74,0x2E,0x76,0x61,0x6C,0x3D,0x30,0xff,0xff,0xff//set drop down to time
																								,0x74,0x4D,0x6F,0x64,0x65,0x2E,0x74,0x78,0x74,0x3D,0x22,0xAE,0xC9,0xB6,0xA1,0x22,0xff,0xff,0xff//print shijian
																								,0x74,0x4D,0x6F,0x64,0x65,0x55,0x6E,0x69,0x74,0x2E,0x74,0x78,0x74,0x3D,0x22,0x53,0x22,0xff,0xff,0xff};//print "S"

uint8_t display_page_setting_1_dist_rel [61] = {0x63,0X62,0X4D,0X6F,0X64,0X65,0X53,0X65,0X6C,0X65,0X63,0X74,0x2E,0x76,0x61,0x6C,0x3D,0x31,0xff,0xff,0xff//set drop down to relative
																										,0x74,0x4D,0x6F,0x64,0x65,0x2E,0x74,0x78,0x74,0x3D,0x22,0xB6,0x5A,0xC2,0xF7,0x22,0xff,0xff,0xff//print jvli
																										,0x74,0x4D,0x6F,0x64,0x65,0x55,0x6E,0x69,0x74,0x2E,0x74,0x78,0x74,0x3D,0x22,0x6D,0x6D,0x22,0xff,0xff,0xff};//print "mm"																								

uint8_t display_page_setting_1_dist_abs [61] = {0x63,0X62,0X4D,0X6F,0X64,0X65,0X53,0X65,0X6C,0X65,0X63,0X74,0x2E,0x76,0x61,0x6C,0x3D,0x32,0xff,0xff,0xff//set drop down to absolute
																										,0x74,0x4D,0x6F,0x64,0x65,0x2E,0x74,0x78,0x74,0x3D,0x22,0xB6,0x5A,0xC2,0xF7,0x22,0xff,0xff,0xff//print jvli
																										,0x74,0x4D,0x6F,0x64,0x65,0x55,0x6E,0x69,0x74,0x2E,0x74,0x78,0x74,0x3D,0x22,0x6D,0x6D,0x22,0xff,0xff,0xff};//print "mm"

uint8_t display_page_setting_1_power[81] = {0x63,0X62,0X4D,0X6F,0X64,0X65,0X53,0X65,0X6C,0X65,0X63,0X74,0x2E,0x76,0x61,0x6C,0x3D,0x33,0xff,0xff,0xff//set drop down to energy
																										,0x74,0X4D,0X6F,0X64,0X65,0X2E,0X74,0X78,0X74,0X3D,0X22,0XA5,0X5C,0XB2,0X76,0X22,0xff,0xff,0xff//print gong lv
																										,0X6E,0X43,0X75,0X72,0X72,0X65,0X6E,0X74,0X4D,0X6F,0X64,0X65,0X2E,0X76,0X61,0X6C,0X3D,0X34,0xff,0xff,0xff//nCurrentMode.val=4
																										,0X74,0X4D,0X6F,0X64,0X65,0X55,0X6E,0X69,0X74,0X2E,0X74,0X78,0X74,0X3D,0X22,0X25,0X22,0xff,0xff,0xff};//print "%"
	
uint8_t display_page_setting_1_dist_energy[60] = {0x63,0X62,0X4D,0X6F,0X64,0X65,0X53,0X65,0X6C,0X65,0X63,0X74,0x2E,0x76,0x61,0x6C,0x3D,0x34,0xff,0xff,0xff//set drop down to energy
																										,0x74,0x4D,0x6F,0x64,0x65,0x2E,0x74,0x78,0x74,0x3D,0x22,0xB5,0x4A,0xA6,0xD5,0x22,0xff,0xff,0xff//print jiaoer
																										,0x74,0x4D,0x6F,0x64,0x65,0x55,0x6E,0x69,0x74,0x2E,0x74,0x78,0x74,0x3D,0x22,0x4A,0x22,0xff,0xff,0xff};//print "J"
	
uint8_t display_page_setting_1_trigger_val [10] ={0x78,0X4D,0X6F,0X64,0X65,0x2E,0x76,0x61,0x6C,0x3D};//b[22].val=

uint8_t display_page_setting_1_force_set [18] ={0x6E,0X54,0X72,0X69,0X67,0X50,0X72,0X65,0X73,0X73,0X75,0X72,0X65,0x2E,0x76,0x61,0x6C,0x3D};//b[11].val=

uint8_t display_page_setting_1_power_set [12] ={0x6E,0X56,0X69,0X62,0X41,0X6D,0X70,0x2E,0x76,0x61,0x6C,0x3D};//b[31].val=

uint8_t display_page_setting_1_hold_time_set [14] = {0x78,0X48,0X6F,0X6C,0X64,0X54,0X69,0X6D,0X65,0x2E,0x76,0x61,0x6C,0x3D};

uint8_t display_page_lock_freq [17] = {0x70,0x61,0x67,0x65,0x20,0x6C,0x6F,0x63,0x6B,0x5F,0x66,0x72,0x65,0x71,0xff,0xff,0xff};

uint8_t display_page_setting_1_two_stages[57]={0x62,0x53,0x65,0x63,0x74,0x69,0x6F,0x6E,0x56,0x69,0x62,0x2E,0x74,0x78,0x74,0x3D,0x22,0xA4,0xC0,0xB6,0xA5,0x22,0xff,0xff,0xff
																							,0x76,0x69,0x73,0x20,0x6E,0x56,0x69,0x62,0x41,0x6D,0x70,0x2C,0x30,0xff,0xff,0xff
																							,0x76,0x69,0x73,0x20,0x62,0x56,0x69,0x62,0x41,0x6D,0x70,0x2C,0x31,0xff,0xff,0xff};//bSectionVib.txt="fen jie" 
//vis nVibAmp,1
//vis bVibAmp,0

uint8_t display_page_setting_1_one_stage[57]={0x62,0x53,0x65,0x63,0x74,0x69,0x6F,0x6E,0x56,0x69,0x62,0x2E,0x74,0x78,0x74,0x3D,0x22,0xA9,0x54,0xA9,0x77,0x22,0xff,0xff,0xff 
																							,0x76,0x69,0x73,0x20,0x6E,0x56,0x69,0x62,0x41,0x6D,0x70,0x2C,0x31,0xff,0xff,0xff
																							,0x76,0x69,0x73,0x20,0x62,0x56,0x69,0x62,0x41,0x6D,0x70,0x2C,0x30,0xff,0xff,0xff};//bSectionVib.txt="gu ding"

uint8_t display_page_setting_1_early_on[21]={0x62,0x50,0x72,0x65,0x56,0x69,0x62,0x2E,0x74,0x78,0x74,0x3D,0x22,0xB6,0x7D,0xB1,0xD2,0x22,0xff,0xff,0xff};//bPreVib.txt="kai qi"
uint8_t display_page_setting_1_early_off[21]={0x62,0x50,0x72,0x65,0x56,0x69,0x62,0x2E,0x74,0x78,0x74,0x3D,0x22,0xC3,0xF6,0xB3,0xAC,0x22,0xff,0xff,0xff};//bPreVib.txt="guan bi"
uint8_t display_page_setting_1_after_on[23]={0x62,0x41,0x66,0x74,0x65,0x72,0x56,0x69,0x62,0x2E,0x74,0x78,0x74,0x3D,0x22,0xB6,0x7D,0xB1,0xD2,0x22,0xff,0xff,0xff};//bAfterVib.txt="kai qi"
uint8_t display_page_setting_1_after_off[23]={0x62,0x41,0x66,0x74,0x65,0x72,0x56,0x69,0x62,0x2E,0x74,0x78,0x74,0x3D,0x22,0xC3,0xF6,0xB3,0xAC,0x22,0xff,0xff,0xff};//bAfterVib.txt="guan bi"
																							
void print_page_lock_freq(){
	UART_Write(UART1,display_page_lock_freq ,17);
}

void print_page_setting_1(){
		if (read_mode_set()==2 ){
			UART_Write(UART1,display_page_setting_1_dist_rel ,61);
			UART_Write(UART1,display_page_setting_1_trigger_val ,10);
			binary_to_bcd_array(read_distance_relative_set_display());
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		else if (read_mode_set()==3){
			UART_Write(UART1,display_page_setting_1_dist_abs ,61);
			UART_Write(UART1,display_page_setting_1_trigger_val ,10);
			binary_to_bcd_array(read_distance_absolute_set_display());
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		else if (read_mode_set()==5){
			UART_Write(UART1,display_page_setting_1_dist_energy,60);
			UART_Write(UART1,display_page_setting_1_trigger_val ,10);
			binary_to_bcd_array(read_energy_set_display());
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		else if (read_mode_set()==4){
			UART_Write(UART1,display_page_setting_1_power,81);
			UART_Write(UART1,display_page_setting_1_trigger_val ,10);
			binary_to_bcd_array(read_power_stage_one_display());
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		else {
			UART_Write(UART1,display_page_setting_1_time,60);
			UART_Write(UART1,display_page_setting_1_trigger_val ,10);
			binary_to_bcd_array(read_time_set_stage_one_display()/10);
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		
		if (read_stage2_mode_address_set()!=0){
			UART_Write(UART1,display_page_setting_1_two_stages,57);
		}
		else UART_Write(UART1,display_page_setting_1_one_stage,57);
		
		if(read_mode_early_stage_display()!=0)UART_Write(UART1,display_page_setting_1_early_on,21);
		else UART_Write(UART1,display_page_setting_1_early_off,21);
		
		if(read_mode_after_stage_display()!=0)UART_Write(UART1,display_page_setting_1_after_on,23);
		else UART_Write(UART1,display_page_setting_1_after_off,23);
		
		UART_Write(UART1,display_page_setting_1_force_set,18);
		binary_to_bcd_array(read_force_set_display());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_setting_1_power_set,12);
		binary_to_bcd_array(read_amplitude_set_display());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_setting_1_hold_time_set,14);
		binary_to_bcd_array(read_hold_time_display()/10);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
}
uint8_t display_page_setting_2_timeout [15] ={0x6E,0X41,0X62,0X6F,0X72,0X74,0X54,0X69,0X6D,0X65,0x2E,0x76,0x61,0x6C,0x3D};//b[6].val=
void print_page_setting_2(){
		UART_Write(UART1,display_page_setting_2_timeout,15);
		//if(mode_set!=1)binary_to_bcd_array(time_set_display);
		//else 
		binary_to_bcd_array(read_timeout_set_display());
		UART_Write(UART1,bcd_array,2);
		UART_Write(UART1,header,3);
		
}

uint8_t display_page_SectionVib_time_stage2 [10] ={0x78,0x54,0x69,0x6D,0x65,0x2E,0x76,0x61,0x6C,0x3D};//xTime.val=
uint8_t display_page_SectionVib_dist_stage2 [18] ={0x78,0x43,0x6F,0x6C,0x6C,0x61,0x70,0x73,0x65,0x44,0x69,0x73,0x74,0x2E,0x76,0x61,0x6C,0x3D};//xCollapseDist.val=
uint8_t display_page_SectionVib_power_stage2 [11] ={0x6E,0x50,0x6F,0x77,0x65,0x72,0x2E,0x76,0x61,0x6C,0x3D};//nPower.val=
uint8_t display_page_SectionVib_energy_stage2 [12] ={0x6E,0x45,0x6E,0x65,0x72,0x67,0x79,0x2E,0x76,0x61,0x6C,0x3D};//nEnergy.val=
uint8_t display_page_SectionVib_amplitudeA_stage2 [13] ={0x6E,0x56,0x69,0x62,0x41,0x6D,0x70,0x41,0x2E,0x76,0x61,0x6C,0x3D};//nVibAmpA.val=
uint8_t display_page_SectionVib_amplitudeB_stage2 [13] ={0x6E,0x56,0x69,0x62,0x41,0x6D,0x70,0x42,0x2E,0x76,0x61,0x6C,0x3D};//nVibAmpB.val=
void print_page_SectionVib(){
	  
			UART_Write(UART1,display_page_SectionVib_amplitudeA_stage2 ,13);
			binary_to_bcd_array(read_amplitudeA_set_stage2_display());
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
			UART_Write(UART1,display_page_SectionVib_amplitudeB_stage2 ,13);
			binary_to_bcd_array(read_amplitudeB_set_stage2_display());
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		
//		else if (stage_mode!=0 ){  
//			UART_Write(UART1,display_page_SectionVib_amplitudeB_stage2 ,10);
//			binary_to_bcd_array(read_amplitudeB_set_stage2_display());
//			UART_Write(UART1,bcd_array,5);
//			UART_Write(UART1,header,3);
//		}
	  if (read_stage2_mode_address_display()==3 ){  // stage_mode==1
			UART_Write(UART1,display_page_SectionVib_time_stage2 ,10);
			binary_to_bcd_array(read_time_set_stage2_display()/10);
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		else if (read_stage2_mode_address_display()==4){  // stage_mode==2
			UART_Write(UART1,display_page_SectionVib_dist_stage2 ,18);
			binary_to_bcd_array(read_distance_set_stage2_display());
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		
		else if (read_stage2_mode_address_display()==4){  // stage_mode==3
			UART_Write(UART1,display_page_SectionVib_power_stage2 ,11);
			binary_to_bcd_array(read_power_set_stage2_display());
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		
		
		else if (read_stage2_mode_address_display()==6 ){  // stage_mode==4)
			UART_Write(UART1,display_page_SectionVib_energy_stage2 ,12);
			binary_to_bcd_array(read_energy_set_stage2_display());
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
	}

uint8_t display_page_head_down_pressure [9] ={0x62,0x5B,0x33,0x5D,0x2E,0x76,0x61,0x6C,0x3D};//b[8].val=
uint8_t display_page_head_down_force_read [9] ={0x62,0x5B,0x34,0x5D,0x2E,0x76,0x61,0x6C,0x3D};//b[9].val=
uint8_t display_page_head_down_distance_read [14] ={0x6E,0X41,0X42,0X53,0X50,0X6F,0X73,0X69,0x2E,0x74,0x78,0x74,0x3D,0x22};//b[13].txt="
uint8_t one_array_temp[1]={0};
void print_page_head_down(){
		UART_Write(UART1,display_page_head_down_pressure,9);
		binary_to_bcd_array(read_pressure_display());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_head_down_force_read,9);
		binary_to_bcd_array( read_force_display() );
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
		UART_Write(UART1,display_page_head_down_distance_read,14);
		binary_to_bcd_array(read_distance_display());
	
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

//unsigned int freq_min,freq_max,freq_start,freq_end,F_start,F_max,P_max,distance_travelled,time_on;
uint8_t display_page_2_freq_start [7] 				= { 0x6E, 0x31,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_freq_end [7] 					= { 0x6E, 0x32,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_freq_max[7] 						= { 0x6E, 0x33,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_freq_min [7] 					= { 0x6E, 0x34,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_P_max [7] 							= { 0x6E, 0x35,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_energy [7] 						= { 0x6E, 0x36,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_time_on [9] 						= {0x74,0X33,0X34,0x2E,0x74,0x78,0x74,0x3D,0x22};
uint8_t display_page_2_distance_travelled[9]	= {0x74,0X32,0X34,0x2E,0x74,0x78,0x74,0x3D,0x22};//b[13].txt="
uint8_t display_page_2_F_start [7] 						= { 0x6E, 0x39,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_F_max [8] 							= { 0x6E, 0x31,0x30,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_F_set [7] 						= { 0x6E, 0x38,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_timeout_occured [7] 						= { 0x6E, 0x30,0x2E,0x76,0x61,0x6C,0x3D};

uint8_t display_page_2_distance_absolute [9] ={0x74,0X32,0X35,0x2E,0x74,0x78,0x74,0x3D,0x22};//b[13].txt="
uint8_t display_page_2_absolute_hold [9] ={0x74,0X33,0X33,0x2E,0x74,0x78,0x74,0x3D,0x22};//b[13].txt="
uint8_t display_page_2_collapse_hold [9] ={0x74,0X33,0X30,0x2E,0x74,0x78,0x74,0x3D,0x22};//b[13].txt="
uint8_t display_page_2_freq_delta [9] ={0x74,0X33,0X38,0x2E,0x74,0x78,0x74,0x3D,0x22};//b[13].txt="
// + is 2B
// - is 2D
int freq_delta,freq_delta_abs;
void print_page_weld_record(){
		UART_Write(UART1,display_page_2_freq_start,7);
		binary_to_bcd_array(read_freq_start());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
		UART_Write(UART1,display_page_2_freq_end,7);
		binary_to_bcd_array(read_freq_end());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		freq_delta=read_freq_end()-read_freq_start();
		UART_Write(UART1,display_page_2_freq_delta,9);
		if(freq_delta<0){
			one_array_temp[0] = 0x2D;//minus
			freq_delta_abs=-1*freq_delta;}
		else {
			one_array_temp[0] = 0x2B;//plus
			freq_delta_abs=freq_delta;}
		UART_Write(UART1,one_array_temp,1);
		
		if(freq_delta_abs<1000){
		binary_to_bcd_array(freq_delta_abs);
		if(freq_delta_abs>99){
			one_array_temp[0] = bcd_array[2];
			UART_Write(UART1,one_array_temp,1);}
		
		if(freq_delta_abs>9){
			one_array_temp[0] = bcd_array[3];
			UART_Write(UART1,one_array_temp,1);}
		
		one_array_temp[0] = bcd_array[4];
		UART_Write(UART1,one_array_temp,1);
		}
		else {
			one_array_temp[0] = 0x30;
			UART_Write(UART1,one_array_temp,1);
		}
		
		one_array_temp[0] = 0x22;
		UART_Write(UART1,one_array_temp,1);//second decimal place
		
		UART_Write(UART1,header,3);
	
		//binary_to_bcd_array(read_time_on());
		//UART_Write(UART1,bcd_array,2);
		
		UART_Write(UART1,display_page_2_freq_max,7);
		binary_to_bcd_array(read_freq_max());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
		UART_Write(UART1,display_page_2_freq_min,7);
		binary_to_bcd_array(read_freq_min());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
		UART_Write(UART1,display_page_2_P_max,7);
		binary_to_bcd_array(read_P_max());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
		UART_Write(UART1,display_page_2_energy,7);
		binary_to_bcd_array(read_energy_display());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_2_timeout_occured,7);
		binary_to_bcd_array(read_timeout_occured());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_2_time_on,9);
		binary_to_bcd_array(read_time_on());
		
		UART_Write(UART1,bcd_array,2);
		one_array_temp[0] = 0x2E;
		UART_Write(UART1,one_array_temp,1);//decimal point
		one_array_temp[0] = bcd_array[2];
		UART_Write(UART1,one_array_temp,1);//first decimal place
		one_array_temp[0] = bcd_array[3];
		UART_Write(UART1,one_array_temp,1);//second decimal place
		one_array_temp[0] = bcd_array[4];
		UART_Write(UART1,one_array_temp,1);//third decimal place
		one_array_temp[0] = 0x22;
		UART_Write(UART1,one_array_temp,1);//second decimal place
		
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_2_distance_travelled,9);
		binary_to_bcd_array(read_distance_travelled());
		
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
		
		
		UART_Write(UART1,display_page_2_distance_absolute,9);
		binary_to_bcd_array(read_distance_reached());
		
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
		
		UART_Write(UART1,display_page_2_F_start,7);
		binary_to_bcd_array(read_F_start());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_2_F_set,7);
		binary_to_bcd_array(read_force_set_display());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_2_F_max,8);
		binary_to_bcd_array(read_F_max());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		
		UART_Write(UART1,display_page_2_collapse_hold,9);
		binary_to_bcd_array(read_collapse_hold());
		
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
		
		UART_Write(UART1,display_page_2_absolute_hold,9);
		binary_to_bcd_array(read_absolute_hold());
		
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

uint8_t display_page_head_test_power_set [12] ={0x6E,0X56,0X69,0X62,0X41,0X6D,0X70,0x2E,0x76,0x61,0x6C,0x3D};//b[31].val=
uint8_t display_page_head_test_power_read [7] = { 0x6E, 0x35,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_head_test_freq_read [7] = { 0x6E, 0x31,0x2E,0x76,0x61,0x6C,0x3D};

void print_page_head_test(){
		UART_Write(UART1,display_page_head_test_power_set,12);
		binary_to_bcd_array(read_amplitude_set_display());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
		UART_Write(UART1,display_page_head_test_freq_read,7);
		binary_to_bcd_array(read_freq_display());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
		UART_Write(UART1,display_page_head_test_power_read,7);
		binary_to_bcd_array(read_power_read_display());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
}
uint8_t display_page_early_after_mode[17]={0x63,0X62,0X4D,0X6F,0X64,0X65,0X53,0X65,0X6C,0X65,0X63,0X74,0X2E,0x76,0x61,0x6C,0x3D};//cbModeSelect.val
uint8_t display_page_early_after_value[10]={0x78,0x4D,0x6F,0x64,0x65,0x2E,0x76,0x61,0x6C,0x3D};//xMode.val=
uint8_t display_page_early_after_ampli[12]={0x6E,0X56,0X69,0X62,0X41,0X6D,0X70,0x2E,0x76,0x61,0x6C,0x3D};//nVibAmp.val=
uint8_t display_page_after_time_on[18]={0x78,0X41,0X66,0X74,0X65,0X72,0X56,0X69,0X62,0X54,0X69,0X6D,0X65,0x2E,0x76,0x61,0x6C,0x3D};//xAfterVibTime.val=
void print_page_early_after_trigger(){
	if(display_page==5){//early
		UART_Write(UART1,display_page_early_after_mode,17);
		if(read_mode_early_stage_display()==2)one_array_temp[0] = 0x31;
		else one_array_temp[0] = 0x30;
		UART_Write(UART1,one_array_temp,1);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_early_after_value,10);
		if(read_mode_early_stage_display()==2)binary_to_bcd_array(read_distance_early_trigger_display());
		else binary_to_bcd_array(read_time_early_trigger_display()/10);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_early_after_ampli,12);
		binary_to_bcd_array(read_power_early_stage_display());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	}
	else {
		UART_Write(UART1,display_page_early_after_mode,17);
		if(read_mode_after_stage_display()==5)one_array_temp[0] = 0x31;
		else one_array_temp[0] = 0x30;
		UART_Write(UART1,one_array_temp,1);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_early_after_value,10);
		if(read_mode_after_stage_display()==5)binary_to_bcd_array(read_distance_after_trigger_display());
		else binary_to_bcd_array(read_time_after_trigger_display()/10);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_early_after_ampli,12);
		binary_to_bcd_array(read_power_after_stage_display());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_after_time_on,18);
		binary_to_bcd_array(read_time_on_after_stage_display()/10);
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	}
}


//display_to_mcu()
int test,energy_set_temp,timer_mode_set,energy_set_stage2_temp;
//int temp_distance_relative,temp_distance_absolute, temp_energy;
void display_to_mcu(){//display HMI => MCU
				if(display_input_command[2]==0xFF || display_input_command[3]==0xFF){
					switch(display_input_command[0]) {
					case 0xAA:display_page 										=display_input_command[1];break;
					case 0xc0:write_amplitude_set							(display_input_command[1]);break;
					
					//first stage
					case 0xc2:write_mode_set									(display_input_command[1]);break;
					case 0xc3:write_time_set_stage_one_set		(((display_input_command[2]<<8)|(display_input_command[1]))*10);break;
					case 0xc4:write_distance_relative_set			((display_input_command[2]<<8)|(display_input_command[1]));break;
					case 0xc5:write_distance_absolute_set			((display_input_command[2]<<8)|(display_input_command[1]));break;
					case 0xc6:write_power_stage_one_set				((display_input_command[2]<<8)|(display_input_command[1]));break;
					case 0xc7:write_energy_set								((display_input_command[2]<<8)|(display_input_command[1]));break;
					case 0xc9:write_power_stage_one_set				(display_input_command[1]);break;
						
					//main setting
					case 0xca:write_force_set									((display_input_command[2]<<8)|(display_input_command[1]));break;
					case 0xc8:write_hold_time_set							((display_input_command[2]<<8)|(display_input_command[1])*10);break;
					case 0xcd:write_timeout_set								(((display_input_command[2]<<8)|(display_input_command[1]))*1000);break;
					
					//second stage
					case 0xd0:if (display_input_command[1]==0)	write_stage2_mode_address_set(0);break;
					case 0xd1:write_amplitudeA_set_stage2			(display_input_command[1]);break;
					case 0xd2:write_amplitudeB_set_stage2			(display_input_command[1]);break;
					case 0xd3:write_time_set_stage2						(((display_input_command[2]<<8)|(display_input_command[1]))*10);
										write_stage2_mode_address_set		(3);break;
					case 0xd4:write_distance_set_stage2				((display_input_command[2]<<8)|(display_input_command[1]));
										write_stage2_mode_address_set		(4);break;
					case 0xd5:write_power_set_stage2					(display_input_command[1]);
										write_stage2_mode_address_set		(5);break;
					case 0xd6:write_energy_set_stage2					((display_input_command[2]<<8)|(display_input_command[1]));
										write_stage2_mode_address_set		(6);break;
					
					//EARLY STAGE
					case 0xe1:write_time_early_trigger_set		(((display_input_command[2]<<8)|(display_input_command[1]))*10);
										write_mode_early_stage_set			(1);break;
					case 0xe2:write_distance_early_trigger_set((display_input_command[2]<<8)|(display_input_command[1]));
										write_mode_early_stage_set			(2);break;
					case 0xe3:write_power_early_stage_set			(display_input_command[1]);break;
					
					//AFTER STAGE
					case 0xe4:write_time_after_trigger_set		(((display_input_command[2]<<8)|(display_input_command[1]))*10);
										write_mode_after_stage_set			(4);break;
					case 0xe5:write_distance_after_trigger_set((display_input_command[2]<<8)|(display_input_command[1]));
										write_mode_after_stage_set			(5);break;
					case 0xe6:write_power_after_stage_set			(display_input_command[1]);break;
					case 0xe7:write_time_on_after_stage_set		(((display_input_command[2]<<8)|(display_input_command[1]))*10);break;
										
					case 0xe0:
								if		 (display_input_command[1]==1)write_mode_early_stage_set(0);
								else if(display_input_command[1]==2)write_mode_early_stage_set(1);
								else if(display_input_command[1]==3)write_mode_early_stage_set(2);
								else if(display_input_command[1]==4)write_mode_after_stage_set(0);
								else if(display_input_command[1]==5)write_mode_after_stage_set(4);
								else if(display_input_command[1]==6)write_mode_after_stage_set(5);break;
					
					default:test=0;break;
				}}
				
				display_input_command[0]=0;
				display_input_command[1]=0;
				display_input_command[2]=0;
				display_input_command[3]=0;
}

