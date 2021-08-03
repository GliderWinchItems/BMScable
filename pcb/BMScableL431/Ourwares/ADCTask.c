/******************************************************************************
* File Name          : ADCTask.c
* Date First Issued  : 02/01/2019
* Description        : Processing ADC readings after ADC/DMA issues interrupt
*******************************************************************************/
/* 10/23/2020: Revised for Levelwind */

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "malloc.h"

#include "ADCTask.h"
#include "adctask.h"
#include "morse.h"
#include "adcfastsum16.h"
#include "adcextendsum.h"

#include "adcparams.h"
#include "main.h"

extern ADC_HandleTypeDef hadc1;

uint32_t adcsumdb[6]; // debug
uint32_t adcdbctr = 0;// debug

TaskHandle_t ADCTaskHandle;

float fclpos;

/* *************************************************************************
 * void StartADCTask(void const * argument);
 *	@brief	: Task startup
 * *************************************************************************/
uint16_t* p16;

void StartADCTask(void *argument)
{
	#define TSK02BIT02	(1 << 0)  // Task notification bit for ADC dma 1st 1/2 (adctask.c)
	#define TSK02BIT03	(1 << 1)  // Task notification bit for ADC dma end (adctask.c)

	uint16_t* pdma;

	/* A notification copies the internal notification word to this. */
	uint32_t noteval = 0;    // Receives notification word upon an API notify

	/* Get buffers, "our" control block, and start ADC/DMA running. */
	struct ADCDMATSKBLK* pblk = adctask_init(&hadc1,TSK02BIT02,TSK02BIT03,&noteval);
	if (pblk == NULL) {morse_trap(15);}

  /* Infinite loop */
  for(;;)
  {
		/* Wait for DMA interrupt */
		xTaskNotifyWait(0,0xffffffff, &noteval, portMAX_DELAY);

		if (noteval & TSK02BIT02)
		{
			pdma = adc1dmatskblk[0].pdma1;
p16 = pdma;			
		}
		else
		{
			pdma = adc1dmatskblk[0].pdma2;
		}

		/* Sum the readings 1/2 of DMA buffer to an array. */
		adcfastsum16(&adc1.chan[0], pdma); // Fast in-line addition
		adc1.ctr += 1; // Update count

#define DEBUGGINGADCREADINGS
#ifdef DEBUGGINGADCREADINGS
		/* Save sum for defaultTask printout for debugging */
		adcsumdb[0] = adc1.chan[0].sum;
		adcsumdb[1] = adc1.chan[1].sum;
		adcsumdb[2] = adc1.chan[2].sum;
		adcsumdb[3] = adc1.chan[3].sum;
		adcsumdb[4] = adc1.chan[4].sum;
//		adcsumdb[5] = adc1.chan[5].sum;
		adcdbctr += 1;
#endif

		/* Extended sum for smoothing and display. */
		adcextendsum(&adc1);

		/* Calibrate and filter ADC readings. */
		adcparams_cal();

		/* Notify GEVCUr Task that new readings are ready. */
//		if( LevelwindTaskHandle == NULL) morse_trap(51); // JIC task has not been created	
//		xTaskNotify(LevelwindTaskHandle, GEVCUBIT00, eSetBits);

  }
}
/* *************************************************************************
 * osThreadId xADCTaskCreate(uint32_t taskpriority);
 * @brief	: Create task; task handle created is global for all to enjoy!
 * @param	: taskpriority = Task priority (just as it says!)
 * @return	: ADCTaskHandle
 * *************************************************************************/
osThreadId xADCTaskCreate(uint32_t taskpriority)
{
	BaseType_t xRet;

	xRet = xTaskCreate(
		StartADCTask,     /* Function that implements the task. */
		"ADCTask",        /* Text name for the task. */
		128,              /* Stack size in words, not bytes. */
		(void*)1,         /* Parameter passed into the task. */
		taskpriority,     /* Priority at which the task is created. */
		&ADCTaskHandle ); /* Used to pass out the created task's handle. */ 

	if( xRet == pdPASS )
    {
    	return ADCTaskHandle;	
    }
    return NULL;

}

