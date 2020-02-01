//Select Enable FreeRTOS to collect run time stats under Component Config > FreeRTOS (this should be enabled in the example by default)
//
//Choose the clock source for run time stats configured under Component Config > FreeRTOS. The esp_timer should be selected be default.
//This option will affect the time unit resolution in which the statistics are measured with respect to.

//https://wiki.bash-hackers.org/scripting/terminalcodes



#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <nvs_flash.h>
#include "esp32_htop.h"


#define PRO_CPU	0
#define APP_CPU	1

// Report Detailes stats or not
// 0 = Just printf Cpu0 & Cpu1 stats
// 1 = Print all thread stats as well.
#define DETAIL_LEVEL	1


xTaskHandle TaskHandle_IdleLoop1;
xTaskHandle TaskHandle_IdleLoop2;
xTaskHandle TaskHandle_IdleLoop3;
xTaskHandle TaskHandle_IdleLoop4;
xTaskHandle TaskHandle_IdleLoop5;


/********************************************************************
 *
 *
 *  	idleloop1()
 *
 *
 ********************************************************************/
void idleloop1(void * parameter)
{

	for (;;)
	{
		//Consume CPU cycles
		for(uint32_t i = 0 ; i < SPIN_ITER ; i++)
		{
			__asm__ __volatile__("NOP");
		}

		uint8_t t = rand() % 200;

		vTaskDelay(pdMS_TO_TICKS(t + 10));
	}
}
/********************************************************************
 *
 *
 *  	idleloop2()
 *
 *
 ********************************************************************/
void idleloop2(void * parameter)
{

	for (;;)
	{
		//Consume CPU cycles
		for(uint32_t i = 0 ; i < SPIN_ITER ; i++)
		{
			__asm__ __volatile__("NOP");
		}

		uint8_t t = rand() % 200;

		vTaskDelay(pdMS_TO_TICKS(t + 20));
	}
}
/********************************************************************
 *
 *
 *  	idleloop3()
 *
 *
 ********************************************************************/
void idleloop3(void * parameter)
{

	for (;;)
	{
		//Consume CPU cycles
		for(uint32_t i = 0 ; i < SPIN_ITER ; i++)
		{
			__asm__ __volatile__("NOP");
		}

		uint8_t t = rand() % 200;

		vTaskDelay(pdMS_TO_TICKS(t + 20));
	}
}
/********************************************************************
 *
 *
 *  	idleloop4()
 *
 *
 ********************************************************************/
void idleloop4(void * parameter)
{

	for (;;)
	{
		//Consume CPU cycles
		for(uint32_t i = 0 ; i < SPIN_ITER ; i++)
		{
			__asm__ __volatile__("NOP");
		}

		uint8_t t = rand() % 200;

		vTaskDelay(pdMS_TO_TICKS(t + 20));
	}
}
/********************************************************************
 *
 *
 *  	idleloop5()
 *
 *
 ********************************************************************/
void idleloop5(void * parameter)
{

	for (;;)
	{
		//Consume CPU cycles
		for(uint32_t i = 0 ; i < SPIN_ITER ; i++)
		{
			__asm__ __volatile__("NOP");
		}

		uint8_t t = rand() % 200;

		vTaskDelay(pdMS_TO_TICKS(t + 20));
	}
}
/***********************************************************************
 *
 * app_main
 *
 ***********************************************************************/
void app_main()
{
	//Initialize NVS
	esp_err_t ret = nvs_flash_init();

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);


	printf("\r\n***************************\r\n");
	printf("***   ESP32 System Up   **\r\n");
	printf("***************************\r\n");

	printf("Compiled at:");
	printf(__TIME__);
	printf(" ");
	printf(__DATE__);
	printf("\r\n");



	//Spin up Task
	xTaskCreatePinnedToCore(idleloop1, "idleloop1", 2048, NULL, 10, &TaskHandle_IdleLoop1, APP_CPU);

	//Spin up Task
	xTaskCreatePinnedToCore(idleloop2, "idleloop2", 2048, NULL, 10, &TaskHandle_IdleLoop2, APP_CPU);

	//Spin up Task
	xTaskCreatePinnedToCore(idleloop3, "idleloop3", 2048, NULL, 10, &TaskHandle_IdleLoop3, APP_CPU);

	//Spin up Task
	xTaskCreatePinnedToCore(idleloop4, "idleloop4", 2048, NULL, 10, &TaskHandle_IdleLoop4, PRO_CPU);

	//Spin up Task
	xTaskCreatePinnedToCore(idleloop5, "idleloop5", 2048, NULL, 10, &TaskHandle_IdleLoop5, PRO_CPU);


	//Create and start stats task
	xTaskCreatePinnedToCore(stats_task, "stats", 4096, (void *) DETAIL_LEVEL, STATS_TASK_PRIO, NULL, PRO_CPU);



}