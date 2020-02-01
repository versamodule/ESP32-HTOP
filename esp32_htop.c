#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_err.h"


#include "esp32_htop.h"


// Store vaules for each port away.
static TOL task_order_list[TASKS_LISTS];


uint8_t task_total = 0;

/***********************************************************************
 *
 * stats_task
 *
 ***********************************************************************/
void stats_task(void *arg)
{
	printf("%s", HOME_SCREEN);
	for (uint16_t x = 0; x < 30; x++)
	{
		printf("                                                                                \r\n");
	}

	// Clear Vars
	for(uint8_t x = 0 ; x < TASKS_LISTS ; x++)
	{
		bzero(task_order_list[x].task_name, sizeof(task_order_list[x].task_name));
		task_order_list[x].percentage_time = 0;
	}

	//Print real time stats periodically
	for(;  ;)
	{
		vTaskDelay(pdMS_TO_TICKS(1000));


		if (print_real_time_stats(STATS_TICKS) == ESP_OK)
		{
			char temp[TASKS_NAME_LENGTH + 1];

			for (int i = 0; i < task_total; i++)
			{
				for (int j = i + 1; j < task_total; j++)
				{
					if (strcmp(task_order_list[i].task_name, task_order_list[j].task_name) > 0)
					{
						strcpy(temp, task_order_list[i].task_name);
						uint32_t pt = task_order_list[i].percentage_time;

						strcpy(task_order_list[i].task_name, task_order_list[j].task_name);
						task_order_list[i].percentage_time = task_order_list[j].percentage_time;

						strcpy(task_order_list[j].task_name, temp);
						task_order_list[j].percentage_time = pt;
					}
				}
			}

			// Set cursor to home screen
			printf("%s", HOME_SCREEN);
			printf("Real time stats over %d ticks\r\n", STATS_TICKS);
			printf("Total Tasks %d\r\n", task_total);
			printf("\x1b[31m-----------------------------------------\x1b[0m\r\n");

			// If arg is 0 then we just want cpu0 & cpu1 stats
			if(arg == 0)
			{
				task_total = 2;
			}

			for (uint8_t j = 0; j < task_total; j++)
			{

				uint32_t cpu_used = 100 - task_order_list[j].percentage_time;
				uint8_t bars = ((int)(((float)cpu_used / 100) * 40)) + 1;


				// Rename IDLE0 & IDLE1 to PRO & App cpu's These are the 2 cores.
				if(strcmp(task_order_list[j].task_name, "IDLE0") == 0)
				{
					strcpy(task_order_list[j].task_name, "CPU 0 (Pro)");
				}
				else if(strcmp(task_order_list[j].task_name, "IDLE1") == 0)
				{
					strcpy(task_order_list[j].task_name, "CPU 1 (App)");
				}
				else
				{
					cpu_used = task_order_list[j].percentage_time;
					bars = ((int)(((float)task_order_list[j].percentage_time / 100) * 40)) + 1;
				}


				// Lets make names all evenly spaced.
				int g = strlen(task_order_list[j].task_name);
				printf("%s%s", COLOR_CYAN, task_order_list[j].task_name);
				for (int l = g; l < 15; l++)
				{
					printf(" ");
				}

				// Start Bracket
				printf("%s", START_BRACKET);

				for (uint8_t s = 0; s < 40; s++)
				{
					if (s < bars)
					{
						if (s < CPU_MID_RANGE)
						{
							printf("%s|", COLOR_GREEN);
						}
						else if ((s >= CPU_MID_RANGE) && (s < CPU_HIGH_RANGE))
						{
							printf("%s|", COLOR_YELLOW);
						}
						else if ((s >= CPU_HIGH_RANGE) && (s < bars))
						{
							printf("%s|", COLOR_RED);
						}
					}
					else
					{
						printf(" ");
					}
				}

				printf("%03d%%", cpu_used);

				// End Bracket
				printf("%s\r\n", END_BRACKET);
			}
		}
		else
		{
			printf("Error getting real time stats\r\n");
		}

	}
}
/********************************************************************
 *
 *
 *  	print_real_time_stats()
 *
 *
 ********************************************************************/
esp_err_t print_real_time_stats(TickType_t xTicksToWait)
{
	TaskStatus_t *start_array = NULL, *end_array = NULL;
	UBaseType_t start_array_size, end_array_size;
	uint32_t start_run_time, end_run_time;
	esp_err_t ret;

	//Allocate array to store current task states
	start_array_size = uxTaskGetNumberOfTasks() + ARRAY_SIZE_OFFSET;
	start_array = malloc(sizeof(TaskStatus_t) * start_array_size);
	if (start_array == NULL)
	{
		ret = ESP_ERR_NO_MEM;
		goto exit;
	}

	//Get current task states
	start_array_size = uxTaskGetSystemState(start_array, start_array_size, &start_run_time);
	if (start_array_size == 0)
	{
		ret = ESP_ERR_INVALID_SIZE;
		goto exit;
	}

	vTaskDelay(xTicksToWait);

	//Allocate array to store tasks states post delay
	end_array_size = uxTaskGetNumberOfTasks() + ARRAY_SIZE_OFFSET;
	end_array = malloc(sizeof(TaskStatus_t) * end_array_size);
	if (end_array == NULL)
	{
		ret = ESP_ERR_NO_MEM;
		goto exit;
	}
	//Get post delay task states
	end_array_size = uxTaskGetSystemState(end_array, end_array_size, &end_run_time);
	if (end_array_size == 0)
	{
		ret = ESP_ERR_INVALID_SIZE;
		goto exit;
	}

	//Calculate total_elapsed_time in units of run time stats clock period.
	uint32_t total_elapsed_time = (end_run_time - start_run_time);
	if (total_elapsed_time == 0)
	{
		ret = ESP_ERR_INVALID_STATE;
		goto exit;
	}

	task_total = uxTaskGetNumberOfTasks();

	//Match each task in start_array to those in the end_array
	for(int i = 0 ; i < start_array_size ; i++)
	{
		int k = -1;
		for (int j = 0; j < end_array_size; j++)
		{
			if (start_array[i].xHandle == end_array[j].xHandle)
			{
				k = j;
				//Mark that task have been matched by overwriting their handles
				start_array[i].xHandle = NULL;
				end_array[j].xHandle = NULL;
				break;
			}
		}
		//Check if matching task found
		// and save the values
		if(k >= 0)
		{
			uint32_t task_elapsed_time = end_array[k].ulRunTimeCounter - start_array[i].ulRunTimeCounter;
			uint32_t percentage_time = (task_elapsed_time * 100UL) / total_elapsed_time;

			strcpy(task_order_list[i].task_name, start_array[i].pcTaskName);
			task_order_list[i].percentage_time = percentage_time;

		}
	}

	//Print unmatched tasks
	for(int i = 0 ; i < start_array_size ; i++)
	{
		if (start_array[i].xHandle != NULL)
		{
			printf("| %s | Deleted\n", start_array[i].pcTaskName);
			bzero(task_order_list[i].task_name, sizeof(task_order_list[i].task_name));
			task_order_list[i].percentage_time = 0;

		}
	}
	for (int i = 0; i < end_array_size; i++)
	{
		if (end_array[i].xHandle != NULL)
		{
			printf("| %s | Created\n", end_array[i].pcTaskName);
		}
	}
	ret = ESP_OK;

exit:
	//Common return path
	free(start_array);
	free(end_array);
	return ret;
}
