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
void print_page_2();
void print_page_1();
void print_page_0();
extern uint8_t header[3];
extern uint8_t display_page;
#endif
