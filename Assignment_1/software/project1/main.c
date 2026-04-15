// Standard includes
#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// FreeRTOS includes
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/queue.h"
#include "FreeRTOS/semphr.h"
#include "freertos/portmacro.h"

// Altera includes
#include "sys/alt_irq.h"
#include "alt_types.h"
#include "altera_avalon_pio_regs.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_ps2.h"


#define TASK_STACKSIZE 2048
#define STATE_NORMAL      0
#define STATE_MANAGING    1
#define STATE_MAINTENANCE 2

#define freqThreshold 49.0
#define ROCThreshold 0.05

// globals
static QueueHandle_t rawFreqData;
static SemaphoreHandle_t freqMutex;
SemaphoreHandle_t buttonSem;
SemaphoreHandle_t thresholdSem;
SemaphoreHandle_t stateMutex;
SemaphoreHandle_t loadSem;

static double prevFreq = 0;
double freq;
double ROC;
volatile int systemState = 0;
volatile int buttonValue = 0;
volatile unsigned  int uiSwitchValue = 0;
volatile int loadState;


// ISRs

// Frequency hardware interrupt
void FrequencyRelayISR(void* context, alt_u32 id)
{
    unsigned int temp = IORD(FREQUENCY_ANALYSER_BASE, 0);
    printf("%d", temp);
    xQueueSendToBackFromISR(rawFreqData, &temp, pdFALSE); // Send data to the queue
}

// Button interrupt
void button_interrupts_function(void* context, alt_u32 id)
{
  // need to cast the context first before using it
  int* temp = (int*) context;
  (*temp) = IORD_ALTERA_AVALON_PIO_EDGE_CAP(PUSH_BUTTON_BASE);

  // clears the edge capture register
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PUSH_BUTTON_BASE, 0x7);


  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(buttonSem, &xHigherPriorityTaskWoken);
  portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

// TASKS

// calculate the frequency and the rate of change of frequency
void FrequencyAnalysisTask(void *pvParameters)
{
    int rawFreqValue;
    double localFreq;
    double localROC;

    while(1)
    {
        // Wait for data to arrive in the queue
        if (xQueueReceive(rawFreqData, &rawFreqValue, portMAX_DELAY) == pdTRUE)
        {
            localFreq = 16000.0 / (double)rawFreqValue;

            localROC = (localFreq - prevFreq) / (double)rawFreqValue;
            printf("Frequency: %.3f Hz | RoC: %.3f Hz/s\n", localFreq, localROC);

            prevFreq = localFreq;
        }

        xSemaphoreTake(freqMutex, portMAX_DELAY);

        freq = localFreq;
        ROC = localROC;

        xSemaphoreGive(freqMutex);

        // if threshold not met, change the state and go to the load task
        if(freq < freqThreshold || ROC < ROCThreshold){

        	xSemaphoreTake(stateMutex, portMAX_DELAY);

        	if(systemState == 0){
        		systemState = 1;
        	}

        	xSemaphoreGive(loadSem);
        	xSemaphoreGive(stateMutex);

        }
    }
}

// Switches between Normal and Maintenance State
void vCheckButtonTask(){

	while(1){

		// Triggered by the button
		xSemaphoreTake(buttonSem, portMAX_DELAY);

		xSemaphoreTake(stateMutex, portMAX_DELAY);
		if(systemState == 0){
			systemState = 2;
		}
		else if(systemState == 2){
			systemState = 0;
		}

		xSemaphoreGive(stateMutex);
	}
}

// Controls which loads to be shed. INCOMPLETE - load shedding and gaining back logic.
void vLoadControlTask(void *pvParameters) {
    while(1) {

        // starts when state goes to 1
        xSemaphoreTake(loadSem, portMAX_DELAY);

      // IMPLEMENT CODE TO SHED 1st load

        // 500ms timer loop for managing state
        TickType_t breachStart = 0; // apparently can use this to start timer in here
        int breachActive = 0; // make sure that we are still in the state

        while(1) {

        	// MAYBE NO DELAY IS NEEDED BUT NOT SURE (MIGHT HAVE TO REMOVE THIS OR REDUCE IT)
            vTaskDelay(pdMS_TO_TICKS(50)); // poll every 50ms

            xSemaphoreTake(freqMutex, portMAX_DELAY);
            double localFreq = freq;
            double localROC  = ROC;
            xSemaphoreGive(freqMutex);

            xSemaphoreTake(stateMutex, portMAX_DELAY);
            int state = systemState;
            xSemaphoreGive(stateMutex);

            if (localFreq < freqThreshold || localROC < -ROCThreshold) {
                if (!breachActive) {

                    // Breach just started, record time
                    breachStart = xTaskGetTickCount();
                    breachActive = 1;

                    // used to compare the time between events without reseting the clock, check if 500ms have passed
                } else if (xTaskGetTickCount() - breachStart >= pdMS_TO_TICKS(500)) {

                	// ADD CODE TO SHED LOAD
                    breachStart = xTaskGetTickCount(); // reset for next shed
                }
            } else {

                // Recovered, reset timer
                breachActive = 0;

                xSemaphoreTake(stateMutex, portMAX_DELAY);
                systemState = STATE_NORMAL;
                xSemaphoreGive(stateMutex);
                break;

                // WE ARE BACK IN STATE 0 AND NOTHING ELSE NEEDS TO BE DONE.
                // NEED TO ADD CODE TO GAIN BACK THE LOAD THAT WE SHED AS PER THE PRIORITIES
            }
        }
    }
}

// SETUP

void SetUpMisc(void)
{
    rawFreqData = xQueueCreate(2, sizeof(unsigned int)); // Create the queue
    freqMutex = xSemaphoreCreateMutex();
    buttonSem = xSemaphoreCreateBinary();
    stateMutex = xSemaphoreCreateMutex(); //mutex for states
    loadSem = xSemaphoreCreateBinary();	//mutex to start managestate
 }
}

void SetUpISRs(void)
{
    // Register frequency interrupt
   alt_irq_register(FREQUENCY_ANALYSER_IRQ, 0, FrequencyRelayISR);



void SetUpTasks(void)
{
    // Create the task that will process frequency data
    xTaskCreate(FrequencyAnalysisTask, "FreqAnalysis", TASK_STACKSIZE, NULL, 2, NULL);
//    xTaskCreate(vButtonHealthTask, "Button Health", TASK_STACKSIZE, NULL, 2, NULL);
    xTaskCreate(vCheckButtonTask, "check button", TASK_STACKSIZE, NULL, 1, NULL);
    xTaskCreate(vLoadControlTask, "Load Control", TASK_STACKSIZE, NULL, 2, NULL);

    vTaskStartScheduler(); // Start the FreeRTOS scheduler
}

int main(void)
{
    SetUpMisc();
    SetUpISRs();
    SetUpTasks();
    return 0;
}
