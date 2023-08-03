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
void UART0_Init();
void UART1_Init();
void UART0_TEST_HANDLE();
void UART1_TEST_HANDLE();
void UART1_interrrupt(void);
void UART0_interrrupt(void);
void AutoFlow_FunctionTest();
//void print_page_2();
//void print_page_1();
//void print_page_0();
void print_page_setting_1();
void print_page_setting_2();
void print_page_weld_record();
void print_page_head_down();
extern uint8_t header[3];
extern uint8_t display_page;
void print_page_lock_freq();
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
//////////////////////////////////////////////////////////////////

/////////////////////READ SETTING///////////////////////////////////
unsigned int read_amplitude_set_display();
unsigned int read_mode_set() ;
//MODE 1 TIME
//MODE 2 DISTANCE COLLAPSE/RELATIVE
//MODE 3 DOSTANCE ABSOLUTE
//MODE 5 ENERGY

unsigned int read_freq_display();
unsigned int read_timeout_set_display();
unsigned int read_distance_absolute_set_display();
unsigned int read_distance_relative_set_display();
unsigned int read_energy_set_display();
unsigned int read_force_set_display();
unsigned int read_time_set_stage_one_display();
////////////////////////////////////////////////////////////////////

////////////////////HEAD DOWN//////////////////////////////////////
//unsigned int read_current_display(); not used
unsigned int read_force_display();
unsigned int read_distance_display();
unsigned int read_pressure_display();
////////////////////////////////////////////////////////////////////

//////////////////////HISTORY/////////////////////////////////////
unsigned int read_energy_display();


unsigned int read_freq_min();
unsigned int read_freq_max();
unsigned int read_freq_start();
unsigned int read_freq_end();
unsigned int read_F_start();
unsigned int read_F_max();
unsigned int read_P_max();
unsigned int read_distance_travelled();
unsigned int read_time_on();
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
