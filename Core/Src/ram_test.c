#include "ram_test.h"
#include "usart.h"

#define OSPI_HYPERRAM_SIZE          23
#define OSPI_HYPERRAM_END_ADDR      (1 << OSPI_HYPERRAM_SIZE)

#define OSPI_HYPERRAM_INCR_SIZE     256


uint8_t aTxBuffer[] = " ****Memory-mapped OSPI communication****   ****Memory-mapped OSPI communication****   ****Memory-mapped OSPI communication****   ****Memory-mapped OSPI communication****   ****Memory-mapped OSPI communication****  ****Memory-mapped OSPI communication**** ";

#define BUFFERSIZE                  (COUNTOF(aTxBuffer) - 1)
/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)         (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

typedef unsigned char datum;    /* Set the data bus width to 8 bits.  */

datum expected_val, actual_val;


/**********************************************************************
 *
 * Function:    memTestDataBus()
 *
 * Description: Test the data bus wiring in a memory region by
 *              performing a walking 1's test at a fixed address
 *              within that region.  The address (and hence the
 *              memory region) is selected by the caller.
 *
 * Notes:       
 *
 * Returns:     0 if the test succeeds.  
 *              A non-zero result is the first pattern that failed.
 *
 **********************************************************************/
datum memTestDataBus(volatile datum * address)
{
    datum pattern;
    /*
     * Perform a walking 1's test at the given address.
     */
    for (pattern = 1; pattern != 0; pattern <<= 1)
    {
        /*
         * Write the test pattern.
         */
        *address = pattern;

        /*
         * Read it back (immediately is okay for this test).
         */
        if (*address != pattern) 
        {
            return (pattern);
        }
    }

    return (0);

}   /* memTestDataBus() */

/**********************************************************************
 *
 * Function:    memTestAddressBus()
 *
 * Description: Test the address bus wiring in a memory region by
 *              performing a walking 1's test on the relevant bits
 *              of the address and checking for aliasing. This test
 *              will find single-bit address failures such as stuck
 *              -high, stuck-low, and shorted pins.  The base address
 *              and size of the region are selected by the caller.
 *
 * Notes:       For best results, the selected base address should
 *              have enough LSB 0's to guarantee single address bit
 *              changes.  For example, to test a 64-Kbyte region, 
 *              select a base address on a 64-Kbyte boundary.  Also, 
 *              select the region size as a power-of-two--if at all 
 *              possible.
 *
 * Returns:     NULL if the test succeeds.  
 *              A non-zero result is the first address at which an
 *              aliasing problem was uncovered.  By examining the
 *              contents of memory, it may be possible to gather
 *              additional information about the problem.
 *
 **********************************************************************/
datum* memTestAddressBus(volatile datum * baseAddress, unsigned long nBytes)
{
    unsigned long addressMask = (nBytes/sizeof(datum) - 1);
    unsigned long offset;
    unsigned long testOffset;

    datum pattern     = (datum) 0xAAAAAAAA;
    datum antipattern = (datum) 0x55555555;


    /*
     * Write the default pattern at each of the power-of-two offsets.
     */
    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        baseAddress[offset] = pattern;
    }

    /* 
     * Check for address bits stuck high.
     */
    testOffset = 0;
    baseAddress[testOffset] = antipattern;

    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        if (baseAddress[offset] != pattern)
        {
            return ((datum *) &baseAddress[offset]);
        }
    }

    baseAddress[testOffset] = pattern;

    /*
     * Check for address bits stuck low or shorted.
     */
    for (testOffset = 1; (testOffset & addressMask) != 0; testOffset <<= 1)
    {
        baseAddress[testOffset] = antipattern;

		if (baseAddress[0] != pattern)
		{
			return ((datum *) &baseAddress[testOffset]);
		}

        for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
        {
            if ((baseAddress[offset] != pattern) && (offset != testOffset))
            {
                return ((datum *) &baseAddress[testOffset]);
            }
        }

        baseAddress[testOffset] = pattern;
    }

    return (NULL);

}   /* memTestAddressBus() */

/**********************************************************************
 *
 * Function:    memTestDevice()
 *
 * Description: Test the integrity of a physical memory device by
 *              performing an increment/decrement test over the
 *              entire region.  In the process every storage bit 
 *              in the device is tested as a zero and a one.  The
 *              base address and the size of the region are
 *              selected by the caller.
 *
 * Notes:       
 *
 * Returns:     NULL if the test succeeds.
 *
 *              A non-zero result is the first address at which an
 *              incorrect value was read back.  By examining the
 *              contents of memory, it may be possible to gather
 *              additional information about the problem.
 *
 **********************************************************************/
datum* memTestDevice(volatile datum * baseAddress, unsigned long nBytes)	
{
    unsigned long offset;
    unsigned long nWords = nBytes / sizeof(datum);

    datum pattern;
    datum antipattern;
    expected_val=0;
    actual_val=0;


    /*
     * Fill memory with a known pattern.
     */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        baseAddress[offset] = pattern;
    }

    /*
     * Check each location and invert it for the second pass.
     */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        if (baseAddress[offset] != pattern)
        {
            actual_val=pattern;
            expected_val=baseAddress[offset];
            return ((datum *) &baseAddress[offset]);
        }

        antipattern = ~pattern;
        baseAddress[offset] = antipattern;
    }

    /*
     * Check each location for the inverted pattern and zero it.
     */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        antipattern = ~pattern;
        if (baseAddress[offset] != antipattern)
        {
            actual_val=antipattern;
            expected_val=baseAddress[offset];
            return ((datum *) &baseAddress[offset]);
        }
    }

    return (NULL);

}   /* memTestDevice() */

uint8_t TestStarted[]="RAM Test Started\n";
uint8_t TestPassed[]="RAM Test Passed\n";

void memTest(void)
{
#define BASE_ADDRESS  (volatile datum *) 0x70000000
#define NUM_BYTES     (512 * 1024)
char print_buff[100];
uint8_t status=0;

HAL_UART_Transmit(&huart8, TestStarted, strlen(TestStarted), 1000);

/*datum pattern = memTestDataBus(BASE_ADDRESS);
if (pattern != 0)
{// pattern failure
    sprintf(print_buff, "Pattern Failure: 0x%x\n", pattern);   
    HAL_UART_Transmit(&huart8, print_buff, 23, 1000);
    status=1;
}

datum* alias = memTestAddressBus(BASE_ADDRESS, NUM_BYTES);
if(alias != NULL)
{//alias detected
    sprintf(print_buff, "Alias Detected at: 0x%x\n", alias);   
    HAL_UART_Transmit(&huart8, print_buff, 31, 1000);
    status=1;
}*/

datum* address = memTestDevice(BASE_ADDRESS, NUM_BYTES);
if(address != NULL)
{//address failure
    sprintf(print_buff, "Data Error at: 0x%08x, Expected Value: 0x%02x, Actual Value: 0x%02x\n", address,expected_val,actual_val);   
    HAL_UART_Transmit(&huart8, print_buff, 69, 1000);
    status=1;
}

if(status==0)
{
    HAL_UART_Transmit(&huart8, TestPassed, strlen(TestPassed), 1000);
}

  /*  if ((memTestDataBus(BASE_ADDRESS) != 0) ||
        (memTestAddressBus(BASE_ADDRESS, NUM_BYTES) != NULL) ||
        (memTestDevice(BASE_ADDRESS, NUM_BYTES) != NULL))
    {
        return (-1);
    }
    else
    {
        return (0);
    }*/
		
}   /* memTest() */

#define MEM_SIZE 0x80000

void HyperramTest()
{
    /* Intensive Access ----------------------------------------------- */
    uint32_t *mem_addr;
    uint32_t index;
    uint32_t mem_addr_read_val;
    char print_buff[100];
    uint8_t status = 0;
    uint32_t temp_addr;

    mem_addr=OCTOSPI2_BASE;
    /*clear the entire ram first*/
    for (index = 0; index < MEM_SIZE; (index += 4))
    {   
        *mem_addr=0;
        mem_addr++;       
    }

    mem_addr=OCTOSPI2_BASE;
    temp_addr=mem_addr;
    for (index = 0; index < (MEM_SIZE/4); index++)
    {
        /* Writing Sequence --------------------------------------------------- */        
        *mem_addr = mem_addr;

        /* Reading Sequence --------------------------------------------------- */
        mem_addr_read_val = *mem_addr;
        if (*mem_addr != mem_addr_read_val)
        {
      //      sprintf(print_buff, "Data Error at: 0x%08x, Expected Value: 0x%08x, Actual Value: 0x%08x\n", mem_addr, mem_addr, mem_addr_read_val);
      //      //HAL_UART_Transmit(&huart8, print_buff, 81, 1000);
            status = 1;
            break;
        }
        mem_addr++;        
    }

    if (status == 0)
    { // test passed
        HAL_UART_Transmit(&huart8, TestPassed, strlen(TestPassed), 1000);
    }

    

    /*address += OSPI_HYPERRAM_INCR_SIZE;
    if(address >= OSPI_HYPERRAM_END_ADDR)
    {
      address = 0;
    } */
}

void ST_Hyperam_Test()
{
    uint32_t address = 0;
  uint16_t index;
  __IO uint32_t *mem_addr;
  uint8_t status = 0;
  uint32_t temp;

    while (1)
    {
      /* Intensive Access ----------------------------------------------- */
      mem_addr = (__IO uint32_t *)(OCTOSPI2_BASE + address);
  
      for (index = 0; index < BUFFERSIZE; (index += 4))
      {
        /* Writing Sequence --------------------------------------------------- */
        temp = *(uint32_t *)&aTxBuffer[index];
        *mem_addr = temp;
  
        /* Reading Sequence --------------------------------------------------- */
        if (*mem_addr != *(uint32_t *)&aTxBuffer[index])
        {
      //    sprintf(print_buff, "Data Error at: 0x%08x, Expected Value: 0x%08x, Actual Value: 0x%08x\n", mem_addr, mem_addr, mem_addr_read_val);
      //      //HAL_UART_Transmit(&huart8, print_buff, 81, 1000);
            status = 1;
        }
  
        mem_addr++;
      }  
      
      HAL_Delay(10);
  
      address += OSPI_HYPERRAM_INCR_SIZE;
      if(address >= OSPI_HYPERRAM_END_ADDR)
      {
        address = 0;
      }
    }  
}