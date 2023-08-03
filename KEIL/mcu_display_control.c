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

uint8_t display_page_lock_freq [17] = {0x70,0x61,0x67,0x65,0x20,0x6C,0x6F,0x63,0x6B,0x5F,0x66,0x72,0x65,0x71,0xff,0xff,0xff};

void print_page_lock_freq(){
	UART_Write(UART1,display_page_lock_freq ,17);
}

void print_page_setting_1(){
		if (read_mode_set()==2 ){
			UART_Write(UART1,display_page_setting_1_dist_rel ,54);
			UART_Write(UART1,display_page_setting_1_trigger_val ,10);
			binary_to_bcd_array(read_distance_relative_set_display());
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		else if (read_mode_set()==3){
			UART_Write(UART1,display_page_setting_1_dist_abs ,54);
			UART_Write(UART1,display_page_setting_1_trigger_val ,10);
			binary_to_bcd_array(read_distance_absolute_set_display());
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		else if (read_mode_set()==5){
			UART_Write(UART1,display_page_setting_1_dist_energy,53);
			UART_Write(UART1,display_page_setting_1_trigger_val ,10);
			binary_to_bcd_array(read_energy_set_display()*100);
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		else {
			UART_Write(UART1,display_page_setting_1_time,53);
			UART_Write(UART1,display_page_setting_1_trigger_val ,10);
			binary_to_bcd_array(read_time_set_stage_one_display()/10);
			UART_Write(UART1,bcd_array,5);
			UART_Write(UART1,header,3);
		}
		
		UART_Write(UART1,display_page_setting_1_force_set,10);
		binary_to_bcd_array(read_force_set_display());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_setting_1_power_set,10);
		binary_to_bcd_array(read_amplitude_set_display());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
}
uint8_t display_page_setting_2_timeout [9] ={0x62,0x5B,0x36,0x5D,0x2E,0x76,0x61,0x6C,0x3D};//b[6].val=
void print_page_setting_2(){
		UART_Write(UART1,display_page_setting_2_timeout,9);
		//if(mode_set!=1)binary_to_bcd_array(time_set_display);
		//else 
		binary_to_bcd_array(read_timeout_set_display());
		UART_Write(UART1,bcd_array,2);
		UART_Write(UART1,header,3);
		
}

uint8_t display_page_head_down_pressure [9] ={0x62,0x5B,0x38,0x5D,0x2E,0x76,0x61,0x6C,0x3D};//b[8].val=
uint8_t display_page_head_down_force_read [9] ={0x62,0x5B,0x39,0x5D,0x2E,0x76,0x61,0x6C,0x3D};//b[9].val=
uint8_t display_page_head_down_distance_read [11] ={0x62,0x5B,0x31,0x33,0x5D,0x2E,0x74,0x78,0x74,0x3D,0x22};//b[13].txt="
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
	
		UART_Write(UART1,display_page_head_down_distance_read,11);
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
uint8_t display_page_2_time_on [7] 						= { 0x6E, 0x37,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_distance_travelled[7]	= { 0x6E, 0x38,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_F_start [7] 						= { 0x6E, 0x39,0x2E,0x76,0x61,0x6C,0x3D};
uint8_t display_page_2_F_max [8] 							= { 0x6E, 0x31,0x30,0x2E,0x76,0x61,0x6C,0x3D};


void print_page_weld_record(){
		UART_Write(UART1,display_page_2_freq_start,7);
		binary_to_bcd_array(read_freq_start());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
		UART_Write(UART1,display_page_2_freq_end,7);
		binary_to_bcd_array(read_freq_end());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
	
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
		
		UART_Write(UART1,display_page_2_time_on,7);
		binary_to_bcd_array(read_time_on());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_2_distance_travelled,7);
		binary_to_bcd_array(read_distance_travelled());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_2_F_start,7);
		binary_to_bcd_array(read_F_start());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
		
		UART_Write(UART1,display_page_2_F_max,8);
		binary_to_bcd_array(read_F_max());
		UART_Write(UART1,bcd_array,5);
		UART_Write(UART1,header,3);
}
//display_to_mcu()
int test,energy_set_temp,timer_mode_set;
//int temp_distance_relative,temp_distance_absolute, temp_energy;
void display_to_mcu(){//display HMI => MCU
				if(display_input_command[2]==0xFF || display_input_command[3]==0xFF){
					switch(display_input_command[0]) {
					case 0xAA:
						display_page = display_input_command[1];
						break;
					case 0xc0:
						write_amplitude_set(display_input_command[1]);
						break;
					case 0xc2:
						write_mode_set(display_input_command[1]);
					/*
						if(mode_set==1){
								distance_relative_set= 0;
								distance_absolute_set= 0;
								energy_set 				=0;}
					*/
						
						break;
					case 0xc3:
						write_time_set_stage_one_set(((display_input_command[2]<<8)|(display_input_command[1]))*10);
						
					/*
					
						distance_relative_set= 0;
						distance_absolute_set= 0;
						energy_set 				=0;
					
					
						if(mode_set==1){
								distance_relative_set= 0;
								distance_absolute_set= 0;
								energy_set 				=0;}*/
						break;
					case 0xc4:
						//temp_distance_relative=(display_input_command[2]<<8)|(display_input_command[1]);
						write_distance_relative_set((display_input_command[2]<<8)|(display_input_command[1]));
						/*
						distance_absolute_set=0;
						energy_set=0;
						time_set_stage_one_set = 0;
					*/
						break;
					case 0xc5:
						//distance_relative_set= 0;
						write_distance_absolute_set((display_input_command[2]<<8)|(display_input_command[1]));
						//energy_set=0;
						//time_set_stage_one_set =0;
						break;
					case 0xca:
						write_force_set((display_input_command[2]<<8)|(display_input_command[1]));
						break;
					case 0xc7:
						//distance_relative_set= 0;
						//distance_absolute_set=0;
						energy_set_temp=(display_input_command[2]<<8)|(display_input_command[1]);
						write_energy_set(energy_set_temp/100);
						//time_set_stage_one_set = 0;
						break;
					case 0xcd://timeout
						write_timeout_set(((display_input_command[2]<<8)|(display_input_command[1]))*1000);
						//time_set=((display_input_command[2]<<8)|(display_input_command[1]))*1000;
						//timeout_set=time_set;
						break;
					default:
						test=0;
						break;
				}
					//if((distance_absolute_set!=0 || distance_relative_set!=0 || energy_set!=0)&& time_set==0) time_set=2000;
					///*
					//if(mode_set==1) time_set=timer_mode_set;
					//else time_set=timeout_set;
					
				}
				
				/*
					printf ("page %d\n",display_page);
					printf ("power setting %d\n",amplitude_set);
					printf ("trigger %d\n",mode_set);
					printf ("time set %d\n",time_set);
					printf ("absolute %d\n",distance_absolute_set);
					printf ("relative %d\n",distance_relative_set);
					printf ("energy %d\n",energy_set);*/
}