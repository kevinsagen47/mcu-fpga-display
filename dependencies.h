/**************************************************************************//**
 * @file     dependencies.h
 * @version  V1.00
 * @brief    NuMicro peripheral access layer header file.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __DEPENDENCIES_H__
#define __DEPENDENCIES_H__
#include <stdint.h>

void UART_TEST_HANDLE(void);
void AutoFlow_FunctionTest(void);
void SYS_Init(void);
void UART0_Init(void);
void UART1_Init(void);
void UART0_TEST_HANDLE(void);
void UART1_TEST_HANDLE(void);
void UART1_interrrupt(void);
void UART0_interrrupt(void);
void AutoFlow_FunctionTest(void);
void display_to_mcu(void);
//void print_page_2();
//void print_page_1();
//void print_page_0();
void print_page_setting_1(void);
void print_page_setting_2(void);
void print_page_weld_record(void);
void print_page_head_down(void);
void print_page_SectionVib(void);
extern uint8_t header[3];
extern uint8_t display_input_command[4];
extern uint8_t display_page;
void print_page_lock_freq(void);
extern uint8_t Freq_init;

////////////////////////////////////////////////////////////////////API///////////////////////////////////////////////////////////////
//////////////////////WRITE SETTING////////////////////////////
void write_amplitude_set (unsigned int arg);
void write_mode_set (unsigned int arg);
void write_timeout_set (unsigned int arg);
void write_distance_absolute_set (unsigned int arg);
void write_distance_relative_set (unsigned int arg);
void write_energy_set (unsigned int arg);
void write_force_set (unsigned int arg);
void write_time_set_stage_one_set (unsigned int arg);


void write_amplitudeA_set_stage2 (unsigned int arg);
void write_amplitudeB_set_stage2 (unsigned int arg);
void write_stage_mode (unsigned int arg);
void write_distance_set_stage2 (unsigned int arg);
void write_power_set_stage2 (unsigned int arg);
void write_energy_set_stage2 (unsigned int arg);
void write_time_set_stage2 (unsigned int arg);
void write_stage2_mode_address_set (unsigned int arg);
void write_hold_time_set(unsigned int arg);

//////////////////////////////////////////////////////////////////

/////////////////////READ SETTING///////////////////////////////////
unsigned int read_amplitude_set_display(void);
unsigned int read_mode_set(void) ;
//MODE 1 TIME
//MODE 2 DISTANCE COLLAPSE/RELATIVE
//MODE 3 DOSTANCE ABSOLUTE
//MODE 5 ENERGY

unsigned int read_freq_display(void);
unsigned int read_timeout_set_display(void);
unsigned int read_distance_absolute_set_display(void);
unsigned int read_distance_relative_set_display(void);
unsigned int read_energy_set_display(void);
unsigned int read_force_set_display(void);
unsigned int read_time_set_stage_one_display(void);
unsigned int read_distance_reached(void);
unsigned int read_hold_time_display(void);
////////////////////////////////////////////////////////////////////

////////////////////HEAD DOWN//////////////////////////////////////
//unsigned int read_current_display(); not used
unsigned int read_force_display(void);
unsigned int read_distance_display(void);
unsigned int read_pressure_display(void);
////////////////////////////////////////////////////////////////////

//////////////////////HISTORY/////////////////////////////////////
unsigned int read_energy_display(void);


unsigned int read_freq_min(void);
unsigned int read_freq_max(void);
unsigned int read_freq_start(void);
unsigned int read_freq_end(void);
unsigned int read_F_start(void);
unsigned int read_F_max(void);
unsigned int read_P_max(void);
unsigned int read_distance_travelled(void);
unsigned int read_time_on(void);
///////////////////////////////////////////////////////////////////////

//////////////////////////stages////////////////////////////
unsigned int read_amplitudeA_set_stage2_display(void);
unsigned int read_amplitudeB_set_stage2_display(void);
unsigned int read_stage2_mode_address_display(void);
unsigned int read_time_set_stage2_display(void);
unsigned int read_distance_set_stage2_display(void);
//unsigned int read_power_set_stage2_display(){return power_set_stage2_display;}
unsigned int read_energy_set_stage2_display(void);
unsigned int read_stage2_mode_address_set(void);
//////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
