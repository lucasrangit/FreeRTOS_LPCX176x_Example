/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

// TODO: insert other include files here

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Demo includes. */
#include "basic_io.h"

// TODO: insert other definitions and declarations here

/* The task functions. */
void vTaskMeal( void *pvParameters );
void vTaskWork( void *pvParameters );
void vTaskExercise( void *pvParameters );

/*
 * Define time constants using the scale of 24 hours to 1 minutes.
 * Uses the units of ticks for convenience with FreeRTOS API.
 */
#define TICKS_PER_MS 			portTICK_RATE_MS // ticks/ms
#define MS_PER_S 				1000
#define S_PER_MN 				60
#define MN_PER_HR 				60
#define SIX_OCLOCK_TICKS		6  * S_PER_MN * MS_PER_S * TICKS_PER_MS * 1 / 24
#define EIGHT_OCLOCK_TICKS		8  * S_PER_MN * MS_PER_S * TICKS_PER_MS * 1 / 24
#define TWELVE_OCLOCK_TICKS		12 * S_PER_MN * MS_PER_S * TICKS_PER_MS * 1 / 24
#define NINETEEN_OCLOCK_TICKS	19 * S_PER_MN * MS_PER_S * TICKS_PER_MS * 1 / 24
#define TWENTYTWO_OCLOCK_TICKS	22 * S_PER_MN * MS_PER_S * TICKS_PER_MS * 1 / 24
#define FIFTEEN_MN_TICKS 		15 * S_PER_MN * MS_PER_S * TICKS_PER_MS * 1 / 24
#define ONE_HR_TICKS 			FIFTEEN_MN_TICKS * 4 					* 1 / 24

typedef enum {
	MEAL_BREAKFAST,
	MEAL_LUNCH,
	MEAL_DINNER,
} xMealId;

typedef struct
{
	portTickType xStart;
	portTickType xDuration;
	xMealId xId;
} xMeal;

static const char * pcMealNames[] =
{
		"Breakfast",
		"Lunch",
		"Dinner",
};

static xQueueHandle xQueueMeals;

/* start each day with the recommended 3 mean diet goal */
static xMeal xMeals[3] =
{
	{SIX_OCLOCK_TICKS, FIFTEEN_MN_TICKS, MEAL_BREAKFAST},
	{TWELVE_OCLOCK_TICKS, ONE_HR_TICKS, MEAL_LUNCH},
	{NINETEEN_OCLOCK_TICKS, ONE_HR_TICKS, MEAL_DINNER},
};

static xSemaphoreHandle xSemaphoreExercise;

int main(void)
{
	portBASE_TYPE xStatus = 0;
	int i;

	printf( "\n" ); /* initialize the semi-hosting. */

	xQueueMeals = xQueueCreate( sizeof(xMeals)/sizeof(xMeal), sizeof( xMeal ) );
	portTickType xOnTimeTicks = xTaskGetTickCount();

	/* initialize queue */
	for (i = 0; i < sizeof(xMeals)/sizeof(xMeal); ++i) {
		/* adjust for a non-zero initial counter value */
		xMeals[i].xStart += xOnTimeTicks;

		xStatus = xQueueSend(xQueueMeals, &xMeals[i], 0);
		if (pdPASS != xStatus)
			goto abort;
	}

	xTaskCreate( vTaskMeal, "Meal Task", 240, NULL, 3, NULL );

	/* create a binary semaphore and initialize it to taken */
	vSemaphoreCreateBinary(xSemaphoreExercise);
	xSemaphoreTake(xSemaphoreExercise, 0);

	xTaskCreate( vTaskExercise, "Exercise Task", 240, NULL, 2, NULL);

	xTaskCreate( vTaskWork, "Work Task", 240, NULL, 1, NULL );

	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();

	abort:
	for( ;; );
	return 0;
}

void vTaskMeal( void *pvParameters )
{
	portBASE_TYPE xStatus = 0;
	portTickType xCurrentTicks;
	portTickType xDelayTicks;
	xMeal xMealNext;
	char message[120];

	/* Print out the name of this task. */
	vPrintString( "Meal task start\n" );

	for( ;; )
	{
		xCurrentTicks = xTaskGetTickCount();

		xStatus = xQueueReceive(xQueueMeals, &xMealNext, 0);
		if (pdPASS != xStatus)
			goto abort;

		xDelayTicks = xMealNext.xStart - xCurrentTicks;
//		sprintf(message, "Meal task is waiting %d ticks for %s\n", xDelayTicks, pcMealNames[xMealNext.xId]);
//		vPrintString(message);

		/* delay until event start */
		vTaskDelay( xDelayTicks );

		sprintf(message, "Time for %s\n", pcMealNames[xMealNext.xId]);
		vPrintString(message);
	}


	abort:
	vPrintString( "Meal task stop\n" );
	vTaskDelete(NULL);
}

void vTaskExercise( void *pvParameters )
{
	/* Print out the name of this task. */
	vPrintString( "Exercise task start\n" );

	for( ;; )
	{
		xSemaphoreTake(xSemaphoreExercise, portMAX_DELAY);

		vPrintString( "Time for a stretch...\n" );
	}

	for( ;; );
}

void vTaskWork( void *pvParameters )
{
	const portTickType xDelayTicks1S = 1 * MS_PER_S / portTICK_RATE_MS;
	unsigned portBASE_TYPE uxLoopCount = 0;
	portTickType xCurrentTicks;

	/* Print out the name of this task. */
	vPrintString( "Work task start\n" );

	for( ;; )
	{
		xCurrentTicks = xTaskGetTickCount();

		if (xCurrentTicks > TWENTYTWO_OCLOCK_TICKS) {
			vPrintString( "Time for bed...\n" );
			goto abort;
		}

		if (xCurrentTicks > EIGHT_OCLOCK_TICKS) {
			vPrintString( "Writing code...\n" );
			uxLoopCount++;
		}

		if (uxLoopCount > 10) {
			xSemaphoreGive(xSemaphoreExercise);
			uxLoopCount = 0;
		}

		/* delay until event start */
		vTaskDelay( xDelayTicks1S );
	}

	abort:
	vPrintString( "Work task stop\n" );
	vTaskDelete(NULL);
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* This function will only be called if an API call to create a task, queue
	or semaphore fails because there is too little heap RAM remaining. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	/* This function will only be called if a task overflows its stack.  Note
	that stack overflow checking does slow down the context switch
	implementation. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* This example does not use the idle hook to perform any processing. */
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This example does not use the tick hook to perform any processing. */
}
