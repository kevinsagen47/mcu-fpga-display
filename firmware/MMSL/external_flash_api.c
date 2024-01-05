/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * @brief    Access SPI flash through SPI interface.
 *
 *
 * @copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#define USE_4_BYTES_MODE            0            /* W25Q20 does not support 4-bytes address mode. */

#define FLASH_BLOCK_SIZE            (64*1024)    /* Flash block size. Depend on the physical flash. */
#define TEST_BLOCK_ADDR             0x10000      /* Test block address on SPI flash. */
#define BUFFER_SIZE                 36864

#ifdef __ICCARM__
#pragma data_alignment=4
uint8_t  g_buff[BUFFER_SIZE];
#else
static uint8_t  g_buff[BUFFER_SIZE] __attribute__((aligned(4)));
#endif

void SPIM_Init_MMSL(void);
void SPIM_EraseBlock_MMSL(uint32_t u32Addr);  // Block size is 64KB
void SPIM_Write_MMSL(uint32_t u32Addr, uint32_t u32ByteCount, uint8_t pu8Buf[]);   // u32Addr is 4-byte alignment
void SPIM_Read_MMSL(uint32_t u32Addr, uint32_t u32ByteCount, uint8_t pu8Buf[]);   // u32Addr is 4-byte alignment
//void external_flash_init(void);  // Read flash history data
void save_welding_record(void);
void read_welding_record(void);  // Read flash history data

//extern int history_record_array[255][18];
extern int history_record_array[511][18];
extern unsigned int history_point_set;
extern unsigned int history_point_fpga;
extern unsigned int history_point_display;
extern int prev_history_point;
unsigned int history_point_flash;
const uint32_t pseudo_welding_record[10][18] = {
  {12, 19900, 20000, 20900, 19510, 81, 60, 31, 500, 50, 210, 451, 155, 900, 900, 5, 5, 1500},
  {21, 19910, 20100, 20800, 19520, 82, 61, 32, 499, 50, 220, 452, 160, 850, 850, 4, 4, 1499},
  {32, 19920, 20200, 20700, 19530, 83, 62, 33, 498, 50, 230, 453, 170, 860, 860, 3, 3, 1498},
  {45, 19930, 20300, 20600, 19540, 84, 63, 34, 499, 50, 240, 454, 165, 850, 850, 2, 2, 1497},
  {51, 19940, 20400, 20500, 19550, 85, 64, 35, 498, 50, 250, 455, 170, 850, 850, 3, 3, 1496},
  {46, 19950, 20500, 20600, 19560, 86, 65, 36, 500, 50, 240, 456, 165, 860, 860, 4, 4, 1495},
  {37, 19960, 20600, 21700, 19570, 87, 66, 37, 499, 50, 230, 457, 160, 860, 850, 5, 5, 1496},
  {28, 19970, 20700, 20800, 19580, 88, 67, 38, 498, 50, 220, 458, 165, 850, 850, 4, 4, 1497},
  {15, 19980, 20800, 20900, 19590, 89, 68, 39, 499, 50, 210, 459, 165, 860, 860, 3, 3, 1498},
  {21, 19990, 20900, 21000, 19600, 90, 69, 40, 500, 50, 200, 460, 165, 850, 850, 2, 2, 1499},	
};
uint32_t welding_record_flash[255][18] = {0};

void SYS_Init_External_Flash(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    /* Enable HXT clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);

    /* Wait for HXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to HXT */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT,CLK_CLKDIV0_HCLK(1));

    /* Set core clock (HCLK) as 160MHz from PLL.            */
    /* The maximum frequency of W25Q20 SPI flash is 80 MHz. */
    /* SPIM clock source is HCLK. This sample configue SPIM clock as HCLK divided by 2. */
    CLK_SetCoreClock(FREQ_192MHZ);

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    CLK->PCLKDIV = CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2;

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));

    /* Enable SPIM module clock */
    CLK_EnableModuleClock(SPIM_MODULE);

    /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Set GPB multi-function pins for UART0 RXD and TXD */
	// NuMaker-PFM-M487:
    // SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    // SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
	// MMSL-BLE41006:
	SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA6MFP_Msk | SYS_GPA_MFPL_PA7MFP_Msk);
    SYS->GPA_MFPL |= (SYS_GPA_MFPL_PA6MFP_UART0_RXD | SYS_GPA_MFPL_PA7MFP_UART0_TXD);
	
    /* Init SPIM multi-function pins, MOSI(PC.0), MISO(PC.1), CLK(PC.2), SS(PC.3), D3(PC.4), and D2(PC.5) */
    SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC0MFP_Msk | SYS_GPC_MFPL_PC1MFP_Msk | SYS_GPC_MFPL_PC2MFP_Msk |
                       SYS_GPC_MFPL_PC3MFP_Msk | SYS_GPC_MFPL_PC4MFP_Msk | SYS_GPC_MFPL_PC5MFP_Msk);
    SYS->GPC_MFPL |= SYS_GPC_MFPL_PC0MFP_SPIM_MOSI | SYS_GPC_MFPL_PC1MFP_SPIM_MISO |
                     SYS_GPC_MFPL_PC2MFP_SPIM_CLK | SYS_GPC_MFPL_PC3MFP_SPIM_SS |
                     SYS_GPC_MFPL_PC4MFP_SPIM_D3 | SYS_GPC_MFPL_PC5MFP_SPIM_D2;
    PC->SMTEN |= GPIO_SMTEN_SMTEN2_Msk;

    /* Set SPIM I/O pins as high slew rate up to 80 MHz. */
    PC->SLEWCTL = (PC->SLEWCTL & 0xFFFFF000) |
                  (0x1<<GPIO_SLEWCTL_HSREN0_Pos) | (0x1<<GPIO_SLEWCTL_HSREN1_Pos) |
                  (0x1<<GPIO_SLEWCTL_HSREN2_Pos) | (0x1<<GPIO_SLEWCTL_HSREN3_Pos) |
                  (0x1<<GPIO_SLEWCTL_HSREN4_Pos) | (0x1<<GPIO_SLEWCTL_HSREN5_Pos);

    /* Lock protected registers */
    SYS_LockReg();
}

void SPIM_Init_MMSL(void)
{
	uint8_t idBuf[3];
	
	SPIM_SET_CLOCK_DIVIDER(1);        /* Set SPIM clock as HCLK divided by 2 */

    SPIM_SET_RXCLKDLY_RDDLYSEL(0);    /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */
    SPIM_SET_RXCLKDLY_RDEDGE();       /* Use SPI input clock rising edge to sample received data. */

    SPIM_SET_DCNUM(8);                /* Set 8 dummy cycle. */

    if (SPIM_InitFlash(1) != 0)        /* Initialized SPI flash */
    {
        return;
    }

    SPIM_ReadJedecId(idBuf, sizeof (idBuf), 1);
    
    SPIM_WinbondUnlock(1);

	if (USE_4_BYTES_MODE)
	{
		if (SPIM_Enable_4Bytes_Mode(USE_4_BYTES_MODE, 1) != 0)
		{
			return;
		}
	}
}

void SPIM_EraseBlock_MMSL(uint32_t u32Addr)
{
	SPIM_EraseBlock(u32Addr, USE_4_BYTES_MODE, OPCODE_BE_64K, 1, 1);
}

void SPIM_Write_MMSL(uint32_t u32Addr, uint32_t u32ByteCount, uint8_t pu8Buf[])
{
	SPIM_IO_Write(u32Addr, USE_4_BYTES_MODE, u32ByteCount, pu8Buf, OPCODE_PP, 1, 1, 1);
}

void SPIM_Read_MMSL(uint32_t u32Addr, uint32_t u32ByteCount, uint8_t pu8Buf[])
{
	SPIM_IO_Read(u32Addr, USE_4_BYTES_MODE, u32ByteCount, pu8Buf, OPCODE_FAST_READ, 1, 1, 1, 1);
}

//void external_flash_init(void)
void read_welding_record(void)
{
    uint32_t    i, j, offset;
    uint32_t    *pData;
    //uint8_t     idBuf[3];
	int test_loop = 3;
	uint32_t test_addr = 0;

//    SYS_Init_External_Flash();

//    SYS_UnlockReg();

//	SPIM_Init_MMSL();

	test_addr = TEST_BLOCK_ADDR*test_loop;

	//SPIM_EraseBlock_MMSL(test_addr);

    // Read flash data
//    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
//    {
        memset(g_buff, 0, BUFFER_SIZE);
        //SPIM_IO_Read(test_addr+offset, USE_4_BYTES_MODE, BUFFER_SIZE, g_buff, OPCODE_FAST_READ, 1, 1, 1, 1);
		    SPIM_Read_MMSL(test_addr+offset, BUFFER_SIZE, g_buff);

        pData = (uint32_t *)g_buff;
			  history_point_flash = *pData; pData++;
				if(history_point_flash == 0xFFFFFFFF) history_point_flash = 0;
				for (i = 0; i < 511; i ++) //for (i = 0; i < 255; i ++)
			  {
					for (j = 0; j < 18; j ++)
					{
						history_record_array[i][j] = *pData; //welding_record_flash[i][j] = *pData;
						pData++;
					}
				}
//    }
		//history_point_display = history_point_flash+history_point_fpga;
		history_point_set = history_point_flash;
		prev_history_point = history_point_flash;
}

void save_welding_record(void)
{
    uint32_t    i, j, offset;
    uint32_t    *pData;
    //uint8_t     idBuf[3];
	int test_loop = 3;
	uint32_t test_addr = 0;
	
	test_addr = TEST_BLOCK_ADDR*test_loop;
    // Verify flash page be erased
	  //SPIM_EraseBlock(test_addr, USE_4_BYTES_MODE, OPCODE_BE_64K, 1, 1);
	  SPIM_EraseBlock_MMSL(test_addr);
    
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        //SPIM_IO_Read(test_addr+offset, USE_4_BYTES_MODE, BUFFER_SIZE, g_buff, OPCODE_FAST_READ, 1, 1, 1, 1);
		    SPIM_Read_MMSL(test_addr+offset, BUFFER_SIZE, g_buff);

        pData = (uint32_t *)g_buff;
        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != 0xFFFFFFFF)
            {
                SYS_LockReg();
            }
        }
    }

    // Program data to flash block
    
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        pData = (uint32_t *)g_buff;
      *pData = history_point_display; pData++;
			for (i = 0; i < 511; i ++) //for (i = 0; i < 255; i ++)
			{
			  if (i<1){
				for (j = 0; j < 18; j ++)
				{
					  *pData = pseudo_welding_record[i][j]; // *pData = history_record_array[i][j];
					  pData++;
				}}
				else
				{
					for (j = 0; j < 18; j ++)
					{
						*pData = history_record_array[i][j]; // *pData = pseudo_welding_record[i][j];
					  pData++;
					}
				}
			}
        //SPIM_IO_Write(test_addr+offset, USE_4_BYTES_MODE, BUFFER_SIZE, g_buff, OPCODE_PP, 1, 1, 1);
		    SPIM_Write_MMSL(test_addr+offset, BUFFER_SIZE, g_buff);
    }
}
/*
void external_flash_test()
{
    uint32_t    i, offset;             // variables
    uint32_t    *pData;
    //uint8_t     idBuf[3];
	int test_loop = 1;
	uint32_t test_addr = 0;

    SYS_Init_External_Flash();                        // Init System, IP clock and multi-function I/O

    UART_Open(UART0, 115200);

    printf("+-------------------------------------------+\n");
    printf("|    M480 SPIM I/O mode read/write sample   |\n");
    printf("+-------------------------------------------+\n");

    SYS_UnlockReg();                   // Unlock register lock protect

	SPIM_Init_MMSL();*/
#if 0	
    SPIM_SET_CLOCK_DIVIDER(1);        /* Set SPIM clock as HCLK divided by 2 */

    SPIM_SET_RXCLKDLY_RDDLYSEL(0);    /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */
    SPIM_SET_RXCLKDLY_RDEDGE();       /* Use SPI input clock rising edge to sample received data. */

    SPIM_SET_DCNUM(8);                /* Set 8 dummy cycle. */

    if (SPIM_InitFlash(1) != 0)        /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        goto lexit;
    }

    SPIM_ReadJedecId(idBuf, sizeof (idBuf), 1);
    printf("SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n", idBuf[0], idBuf[1], idBuf[2]);

    SPIM_WinbondUnlock(1);

	if (USE_4_BYTES_MODE)
	{
		if (SPIM_Enable_4Bytes_Mode(USE_4_BYTES_MODE, 1) != 0)
		{
			printf("SPIM_Enable_4Bytes_Mode failed!\n");
			goto lexit;
		}
	}
#endif	
#if 0
    /*
     *  Erase flash page
     */
    printf("Erase SPI flash block 0x%x...", TEST_BLOCK_ADDR);
    SPIM_EraseBlock(TEST_BLOCK_ADDR, USE_4_BYTES_MODE, OPCODE_BE_64K, 1, 1);
    printf("done.\n");

    /*
     *  Verify flash page be erased
     */
    printf("Verify SPI flash block 0x%x be erased...", TEST_BLOCK_ADDR);
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        SPIM_IO_Read(TEST_BLOCK_ADDR+offset, USE_4_BYTES_MODE, BUFFER_SIZE, g_buff, OPCODE_FAST_READ, 1, 1, 1, 1);

        pData = (uint32_t *)g_buff;
        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != 0xFFFFFFFF)
            {
                printf("FAILED!\n");
                printf("Flash address 0x%x, read 0x%x!\n", TEST_BLOCK_ADDR+i, *pData);
                goto lexit;
            }
        }
    }
    printf("done.\n");

    /*
     *  Program data to flash block
     */
    printf("Program sequential data to flash block 0x%x...", TEST_BLOCK_ADDR);
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        pData = (uint32_t *)g_buff;
        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
            *pData = (i << 16) | (TEST_BLOCK_ADDR + offset + i);

        SPIM_IO_Write(TEST_BLOCK_ADDR+offset, USE_4_BYTES_MODE, BUFFER_SIZE, g_buff, OPCODE_PP, 1, 1, 1);
    }
    printf("done.\n");

    /*
     *  Read and compare flash data
     */
    printf("Verify SPI flash block 0x%x data with Fast Read command...", TEST_BLOCK_ADDR);
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        SPIM_IO_Read(TEST_BLOCK_ADDR+offset, USE_4_BYTES_MODE, BUFFER_SIZE, g_buff, OPCODE_FAST_READ, 1, 1, 1, 1);

        pData = (uint32_t *)g_buff;
        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != ((i << 16) | (TEST_BLOCK_ADDR + offset + i)))
            {
                printf("FAILED!\n");
                printf("Flash address 0x%x, read 0x%x, expect 0x%x!\n", TEST_BLOCK_ADDR+i, *pData, (i << 16) | (TEST_BLOCK_ADDR + offset + i));
                goto lexit;
            }
        }

    }
    printf("done.\n");


    /*
     *  Read and compare flash data
     */
    printf("Verify SPI flash block 0x%x data with Fast Read Dual Output command...", TEST_BLOCK_ADDR);
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        SPIM_IO_Read(TEST_BLOCK_ADDR+offset, USE_4_BYTES_MODE, BUFFER_SIZE, g_buff, OPCODE_FAST_DUAL_READ, 1, 1, 2, 1);

        pData = (uint32_t *)g_buff;
        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != ((i << 16) | (TEST_BLOCK_ADDR + offset + i)))
            {
                printf("FAILED!\n");
                printf("Flash address 0x%x, read 0x%x, expect 0x%x!\n", TEST_BLOCK_ADDR+i, *pData, (i << 16) | (TEST_BLOCK_ADDR + offset + i));
                goto lexit;
            }
        }

    }
    printf("done.\n");
#endif
/*
//goto_test_loop:
	// Erase flash page

	test_addr = TEST_BLOCK_ADDR*test_loop;
    printf("Erase SPI flash block 0x%x...", test_addr);
	//SPIM_EraseBlock(test_addr, USE_4_BYTES_MODE, OPCODE_BE_64K, 1, 1);
	SPIM_EraseBlock_MMSL(test_addr);
    printf("done.\n");

    // Verify flash page be erased
    printf("Verify SPI flash block 0x%x be erased...", test_addr);
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        //SPIM_IO_Read(test_addr+offset, USE_4_BYTES_MODE, BUFFER_SIZE, g_buff, OPCODE_FAST_READ, 1, 1, 1, 1);
		SPIM_Read_MMSL(test_addr+offset, BUFFER_SIZE, g_buff);

        pData = (uint32_t *)g_buff;
        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != 0xFFFFFFFF)
            {
                printf("FAILED!\n");
                printf("Flash address 0x%x, read 0x%x!\n", test_addr+i, *pData);
                goto lexit;
            }
        }
    }
    printf("done.\n");

    // Program data to flash block
    printf("Program sequential data to flash block 0x%x...", test_addr);
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        pData = (uint32_t *)g_buff;
        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
            *pData = (i << 16) | (test_addr + offset + i);

        //SPIM_IO_Write(test_addr+offset, USE_4_BYTES_MODE, BUFFER_SIZE, g_buff, OPCODE_PP, 1, 1, 1);
		SPIM_Write_MMSL(test_addr+offset, BUFFER_SIZE, g_buff);
    }
    printf("done.\n");

    // Read and compare flash data
    printf("Verify SPI flash block 0x%x data with Fast Read command...", test_addr);
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        //SPIM_IO_Read(test_addr+offset, USE_4_BYTES_MODE, BUFFER_SIZE, g_buff, OPCODE_FAST_READ, 1, 1, 1, 1);
		SPIM_Read_MMSL(test_addr+offset, BUFFER_SIZE, g_buff);

        pData = (uint32_t *)g_buff;
        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != ((i << 16) | (test_addr + offset + i)))
            {
                printf("FAILED!\n");
                printf("Flash address 0x%x, read 0x%x, expect 0x%x!\n", test_addr+i, *pData, (i << 16) | (test_addr + offset + i));
                goto lexit;
            }
        }

    }
    //printf("done.\n");
	//if (++test_loop <= 0x7F)
	//	goto goto_test_loop;

    printf("\nSPIM I/O read/write demo done.\n");

lexit:

    SYS_LockReg();                     // Lock protected registers
    //while (1);
}*/
