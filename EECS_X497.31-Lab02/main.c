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
#include "timer.h"

/* Demo includes. */
#include "basic_io.h"

// TODO: insert other definitions and declarations here

/* The task functions. */
void vTaskMeal( void *pvParameters );
void vTaskWork( void *pvParameters );
void vTaskExercise( void *pvParameters );

/*
 * Define time constants using the scale of 1 day to MN_PER_DAY minutes.
 * Uses the units of ticks for convenience with FreeRTOS API.
 */
#define MN_PER_DAY					1
#define TICKS_PER_MS 				portTICK_RATE_MS // ticks/ms
#define MS_PER_S 					1000
#define S_PER_MN 					60
#define MN_PER_HR 					60
#define HR_PER_DAY					24
#define CLOCK_TO_TICKS(hour)		(hour * S_PER_MN * MS_PER_S * TICKS_PER_MS * MN_PER_DAY / HR_PER_DAY)
#define SIX_OCLOCK_TICKS			CLOCK_TO_TICKS(6)
#define EIGHT_OCLOCK_TICKS			CLOCK_TO_TICKS(8)
#define TWELVE_OCLOCK_TICKS			CLOCK_TO_TICKS(12)
#define NINETEEN_OCLOCK_TICKS		CLOCK_TO_TICKS(19)
#define TWENTYTWO_OCLOCK_TICKS		CLOCK_TO_TICKS(22)
#define MINUTES_TO_TICKS(minutes) 	(minutes * S_PER_MN * MS_PER_S * TICKS_PER_MS * MN_PER_DAY / HR_PER_DAY)
#define FIFTEEN_MN_TICKS 			MINUTES_TO_TICKS(15)
#define ONE_HR_TICKS 				MINUTES_TO_TICKS(60)

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

extern volatile uint32_t timer0_m0_counter;

static void setup_timer()
{
	/* SystemCoreClockUpdate() updates the SystemCoreClock variable */
	SystemCoreClockUpdate();

	/* set ISR handler */
	NVIC_SetPriority( TIMER0_IRQn, 29);

	/* setup timer 0 with 1 ms interval */
	init_timer(0, 1);

	enable_timer(0);
}

static unsigned portLONG read_timer()
{
	return timer0_m0_counter;
}

int main(void)
{
	portBASE_TYPE xStatus = 0;
	int i;

	printf( "\n" ); /* initialize the semi-hosting. */

	setup_timer();

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
	unsigned portLONG ulTimerValue = 0;
	xMeal xMealNext;
	char message[120];

	/* Print out the name of this task. */
	vPrintString( "START: Meal task\n" );

	for( ;; )
	{
		xStatus = xQueueReceive(xQueueMeals, &xMealNext, 0);
		ulTimerValue = read_timer();
		if (pdPASS != xStatus) {
			vPrintString("No more meals\n");
			goto abort;
		}

		xCurrentTicks = xTaskGetTickCount();

		xDelayTicks = xMealNext.xStart - xCurrentTicks;
//		sprintf(message, "Meal task is waiting %d ticks for %s\n", xDelayTicks, pcMealNames[xMealNext.xId]);
//		vPrintString(message);

		/* delay until event start */
		vTaskDelay( xDelayTicks );

		sprintf(message, "Eat %s @", pcMealNames[xMealNext.xId]);
		vPrintStringAndNumbers(message, xTaskGetTickCount(), ulTimerValue);
	}


	abort:
	vPrintString( "STOP: Meal task\n" );
	vTaskDelete(NULL);
}

void vTaskExercise( void *pvParameters )
{
	/* Print out the name of this task. */
	vPrintString( "START: Exercise task\n" );
	unsigned portLONG ulTimerValue = 0;

	for( ;; )
	{
		xSemaphoreTake(xSemaphoreExercise, portMAX_DELAY);
		ulTimerValue = read_timer();

		vPrintStringAndNumbers( "Time for a stretch @" , xTaskGetTickCount(), ulTimerValue);
	}

	for( ;; );
}

void vTaskWork( void *pvParameters )
{
	const portTickType xDelayTicks1S = 1 * MS_PER_S / portTICK_RATE_MS;
	unsigned portBASE_TYPE uxLoopCount = 0;
	portTickType xCurrentTicks;
	unsigned portLONG ulTimerValue = 0;

	/* Print out the name of this task. */
	vPrintString( "START: Work task\n" );

	for( ;; )
	{
		xCurrentTicks = xTaskGetTickCount();
		ulTimerValue = read_timer();

		if (xCurrentTicks >= TWENTYTWO_OCLOCK_TICKS) {
			vPrintStringAndNumbers( "Bed time @", xTaskGetTickCount(), ulTimerValue);
			goto abort;
		}
		else if (xCurrentTicks >= EIGHT_OCLOCK_TICKS) {
			vPrintStringAndNumbers( "Writing code @", xTaskGetTickCount(), ulTimerValue);
			uxLoopCount++;
		}
		else if (xCurrentTicks >= SIX_OCLOCK_TICKS) {
			vPrintStringAndNumbers( "Taking train to work @", xTaskGetTickCount(), ulTimerValue);
		}
		else if (xCurrentTicks < SIX_OCLOCK_TICKS) {
			vPrintStringAndNumbers( "Sleeping @", xTaskGetTickCount(), ulTimerValue);
		}

		if (uxLoopCount > 10) {
			xSemaphoreGive(xSemaphoreExercise);
			uxLoopCount = 0;
		}

		/* delay until event start */
		vTaskDelay( xDelayTicks1S );
	}

	abort:
	vPrintString( "STOP: Work task\n" );
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
