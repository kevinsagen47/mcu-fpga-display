/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Show FMC read Flash IDs, erase, read, and write function
 *
 *
 * @copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>

#include "NuMicro.h"

#define DB_FLASH_PAGE_SIZE          0x1000
#define DATA_FLASH_START_ADDR  		0x30000      /* Data Flash start address            */
#define DATA_FLASH_END_ADDR    		0x40000      /* Data Flash end address              */
#define TEST_PATTERN_LEN            0x20

#define APROM_TEST_BASE             0x20000      /* APROM test start address                 */
#define DATA_FLASH_TEST_BASE        0x30000      /* Data Flash test start address            */
#define DATA_FLASH_TEST_END         0x40000      /* Data Flash test end address              */
#define TEST_PATTERN                0x5A5A5A5A   /* Test pattern                             */

static int32_t test_pattern[TEST_PATTERN_LEN] = { 0 };
static int32_t read_data[TEST_PATTERN_LEN] = { 0 };

// [return value] 0x00 : Initialize FMC hardware completely
//                other: Error
int32_t Data_Flash_Init(void)
{
	uint32_t au32Config[2];          /* User Configuration */

	FMC_Open();
	
    /* Read User Configuration 0 & 1 */
    if (FMC_ReadConfig(au32Config, 2) < 0)
    {
        //printf("\nRead User Config failed!\n");       /* Error message */
		FMC_Close();
        return -1;                     /* failed to read User Configuration */
    }

    /* Check if Data Flash is enabled and is expected address. */
    if ((!(au32Config[0] & 0x1)) && (au32Config[1] == DATA_FLASH_START_ADDR))
	{
		//printf("\nno need to modify User Configuration!\n"); 
		FMC_Close();
        return 0;                      /* no need to modify User Configuration */
	}

    FMC_ENABLE_CFG_UPDATE();           /* Enable User Configuration update. */

    au32Config[0] &= ~0x1;             /* Clear CONFIG0 bit 0 to enable Data Flash */
    au32Config[1] = DATA_FLASH_START_ADDR;           /* Give Data Flash base address  */

    /* Update User Configuration settings. */
    if (FMC_WriteConfig(au32Config, 2) < 0)
	{
		//printf("\nfailed to write user configuration!\n"); 
		FMC_Close();
        return -1;                     /* failed to write user configuration */
	}

    //printf("\nSet Data Flash base as 0x%x.\n", DATA_FLASH_START_ADDR);  /* debug message */

    /* Perform chip reset to make new User Config take effect. */
    SYS->IPRST0 = SYS_IPRST0_CHIPRST_Msk;
	
	/* Read User Configuration CONFIG0 */
    FMC_Read(FMC_CONFIG_BASE); //printf("  User Config 0 ......................... [0x%08x]\n", FMC_Read(FMC_CONFIG_BASE));
    if (g_FMC_i32ErrCode != 0)
    {
        //printf("FMC_Read(FMC_CONFIG_BASE) failed!\n");
		FMC_Close();
        return -1;
    }

    /* Read User Configuration CONFIG1 */
    FMC_Read(FMC_CONFIG_BASE+4); //printf("  User Config 1 ......................... [0x%08x]\n", FMC_Read(FMC_CONFIG_BASE+4));
    if (g_FMC_i32ErrCode != 0)
    {
        //printf("FMC_Read(FMC_CONFIG_BASE+4) failed!\n");
		FMC_Close();
        return -1;
    }

    /* Read Data Flash base address */
    FMC_ReadDataFlashBaseAddr(); //printf("  Data Flash Base Address ............... [0x%08x]\n", FMC_ReadDataFlashBaseAddr());   /* information message */
	FMC_Close();
	
    return 0;
}

// [return value] 0x00 : Write and Verify Flash completely
//                other: Error
int32_t Data_Flash_Write_Page(int32_t flash_addr, int32_t *buf_ptr, uint16_t buf_len)
{	
	int32_t u32Addr = flash_addr & 0xFFFFF000;  // page alignment
	int32_t u32Data = 0;
	uint16_t u16Len = buf_len;
	uint16_t i = 0;
	
	if ((u32Addr < DATA_FLASH_START_ADDR) || (u32Addr >= DATA_FLASH_END_ADDR))
	{
		return -1;
	}
	
	if ((u16Len * 4) > DB_FLASH_PAGE_SIZE)
	{
		printf("Data Length 0x%x overflow !\n", u16Len * 4);
		FMC_Close();
        return -1;
	}
	
	FMC_Open();
	
	/* Erase page */
    if (FMC_Erase(u32Addr) != 0)
    {
        printf("FMC_Erase address 0x%x failed!\n", u32Addr);
		FMC_Close();
        return -1;
    }
	
	/* Write page */
	for (i = 0; i < u16Len; i++)
	{
		printf("EFC_Write 0x%X:0x%X\n", u32Addr + i * 4, buf_ptr[i]);
		if (FMC_Write(u32Addr + i * 4, buf_ptr[i]) != 0)          /* Program flash */
        {
            printf("FMC_Write address 0x%x failed!\n", u32Addr + i * 4);
			FMC_Close();
            return -1;
        }
	}
	
	/* Verify page */
	for (i = 0; i < u16Len; i++)
	{
		u32Data = FMC_Read(u32Addr + i * 4);	
		if (g_FMC_i32ErrCode != 0)
        {
            printf("FMC_Read address 0x%x failed!\n", u32Addr);
			FMC_Close();
            return -1;
        }
		
		if (u32Data != buf_ptr[i])
		{
			printf("\nFMC_Read data verify failed at address 0x%x, read=0x%x, expect=0x%x\n", u32Addr + i * 4, u32Data, buf_ptr[i]);
			FMC_Close();
            return -1;                 /* data verify failed */
		}
	}
	
	FMC_Close();
	
	return 0;
}

// [return value] 0x00 : Read Flash completely
//                other: Error
int32_t Data_Flash_Read(int32_t flash_addr, int32_t *buf_ptr, uint16_t buf_len)
{	
	int32_t u32Addr = flash_addr;
	uint16_t u16Len = buf_len;
	uint16_t i = 0;
	
	if ((u32Addr < DATA_FLASH_START_ADDR) || (u32Addr >= DATA_FLASH_END_ADDR))
	{
		return -1;
	}
	
	if ((u16Len * 4) > DB_FLASH_PAGE_SIZE)
	{		
		return -1;
	}
	
	FMC_Open();		
	
	for (i = 0; i < u16Len; i++)
	{
		buf_ptr[i] = FMC_Read(u32Addr + i * 4);	
		if (g_FMC_i32ErrCode != 0)
        {
            printf("FMC_Read address 0x%x failed!\n", u32Addr);
			FMC_Close();
            return -1;
        }
		printf("\nFMC_Read 0x%X:0x%X ", u32Addr + i * 4, buf_ptr[i]);
	}
	
	FMC_Close();	
	
	return 0;
}

void example_flow()
{
	int i = 0;
	
    printf("+------------------------------------------+\n");
    printf("|    M487 DB Flash API Example             |\n");
    printf("+------------------------------------------+\n");
		
	if (Data_Flash_Init() == 0)
	{
		printf("\n\n[Test Page 0]\n");
		for (i = 0; i < TEST_PATTERN_LEN; i++)
		{
			test_pattern[i] = i;
		}
	
		if (Data_Flash_Write_Page(DATA_FLASH_START_ADDR, test_pattern, TEST_PATTERN_LEN) == 0)
		{
			;
		}		
		if (Data_Flash_Read(DATA_FLASH_START_ADDR, read_data, TEST_PATTERN_LEN) == 0)
		{
			;
		}
		
		printf("\n\n[Test Page 1]\n");
		for (i = 0; i < TEST_PATTERN_LEN; i++)
		{
			test_pattern[i] = i * 4;
		}
		if (Data_Flash_Write_Page(DATA_FLASH_START_ADDR + DB_FLASH_PAGE_SIZE, test_pattern, TEST_PATTERN_LEN) == 0)
		{
			;
		}		
		if (Data_Flash_Read(DATA_FLASH_START_ADDR + DB_FLASH_PAGE_SIZE, read_data, TEST_PATTERN_LEN) == 0)
		{
			;
		}
	}
}
