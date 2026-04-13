/*
 * main.c
 *
 *  Created on: 9/04/2026
 *      Author: jevaa
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#include "system.h"
#include "sys/alt_irq.h"
#include "io.h"
#include "altera_avalon_pio_regs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

// Definition of Task Stacks
#define   TASK_STACKSIZE       2048

/* The parameters passed to the reg test tasks.  This is just done to check
 the parameter passing mechanism is working correctly. */
#define mainREG_TEST_1_PARAMETER    ( ( void * ) 0x12345678 )
#define mainREG_TEST_2_PARAMETER    ( ( void * ) 0x87654321 )
#define mainREG_TEST_PRIORITY       ( tskIDLE_PRIORITY + 1)



// initial code for measuring the frequency. change it later accordingly
void freq_relay(){
	unsigned int temp = IORD(FREQUENCY_ANALYSER_BASE, 0);
	printf("%f Hz\n", 16000/(double)temp);
	return;
}

void vblinkTask(){

	while(1){
		IOWR_ALTERA_AVALON_PIO_DATA(GREEN_LEDS_BASE, 0xaa);
		vTaskDelay(1000);
		IOWR_ALTERA_AVALON_PIO_DATA(GREEN_LEDS_BASE, 0xaa);
		vTaskDelay(1000);
	}

}
//blink the led while printing the frequecncy
int main()
{
	alt_irq_register(FREQUENCY_ANALYSER_IRQ, 0, freq_relay);

	xTaskCreate(vblinkTask, "Blink", TASK_STACKSIZE, NULL, 1, NULL);

	vTaskStartScheduler();

	for (;;);


  return 0;
}
