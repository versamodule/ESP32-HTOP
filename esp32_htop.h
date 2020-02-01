#pragma once

#include "freertos/task.h"
#include "esp_err.h"

// Determine when bar graph changes from green to yellow to red
// Bargraph length is 40
#define CPU_MID_RANGE 21
#define CPU_HIGH_RANGE 31

#define END_BRACKET		"\x1b[32;1m]\x1b[0m"
#define START_BRACKET	"\x1b[32;1m[\x1b[0m"
#define COLOR_GREEN		"\x1b[32m"
#define COLOR_RED		"\x1b[31m"
#define COLOR_YELLOW	"\x1b[33m"
#define COLOR_CYAN		"\x1b[36m"
#define HOME_SCREEN		"\x1b[H"


#define SPIN_ITER           4000000			//Actual CPU cycles used will depend on compiler optimization
#define STATS_TASK_PRIO     30				// ADjust as needed. Typically 3, but if allot of tasks to print then kick this up
#define STATS_TICKS         pdMS_TO_TICKS(250)
#define ARRAY_SIZE_OFFSET   10				//Increase this if print_real_time_stats returns ESP_ERR_INVALID_SIZE

#define TASKS_LISTS			20				// How many tasks are there going to be?
#define TASKS_NAME_LENGTH	20				// Spacing for task names.


typedef struct
{
	char task_name[TASKS_NAME_LENGTH];
	uint32_t percentage_time;

}TOL;


esp_err_t print_real_time_stats(TickType_t xTicksToWait);
void stats_task(void *arg);
