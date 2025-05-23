#include <stdbool.h>
#include "stm32f4xx.h"
#include "scheduler.h"
#include "bsp.h"

//Task structure
typedef struct
{
	void (*pTask)(void);
	uint16_t Delay;
	uint16_t Period;
	uint8_t RunMe;

} sTask;

//Array of tasks
sTask ScheduleTasks[SCH_MAX_TASKS];

//Number of tasks released so far
uint32_t ReleasedTasks = 0;

//Indicate a running task
bool TaskRunning = false;

//Number of tasks that were executed and exceeded the
//current time slice
uint32_t OverflowTasks = 0;

//Error code
uint8_t ErrorCode = 0;

//private functions
//static void SchGoToSleep(void);

//private variables
//static uint16_t ErrorTickCount;
//static uint8_t LastErrorCode;


void sInit(void)
{
	uint8_t Index;
	for(Index=0;Index < SCH_MAX_TASKS; Index++)
	{
		sDeleteTask(Index);
	}
	//initialize tick timer, in this case SysTickHandler is already initialized
	//so we do not need to do it here again
}

void sDispatchTasks(void)
{
	uint8_t Index;
	for(Index=0; Index < SCH_MAX_TASKS; Index++)
	{
		if(ScheduleTasks[Index].RunMe > 0)
		{
			ReleasedTasks+=1; //one more task has been released
			TaskRunning = true; //task is now running
			(*ScheduleTasks[Index].pTask)(); //run the specific task
			TaskRunning = false; //scheduler regained control
			ScheduleTasks[Index].RunMe -= 1; //reset RunMe flag
			//if it was a one-shot task, remove it from task array
			if(ScheduleTasks[Index].Period == 0)
			{
				sDeleteTask(Index);
			}

		}
	}
}

uint8_t sAddTask(void (*pFunction) (), const uint16_t Delay, const uint16_t Period)
{
	uint8_t Index = 0;
	//find an empty space in the task array
	while((ScheduleTasks[Index].pTask != 0) && (Index < SCH_MAX_TASKS)) Index++;
	//did we reach the end of the list?
	if(Index == SCH_MAX_TASKS)
	{
		//task list is full
		//set global error code
		ErrorCode = ERROR_SCH_TOO_MANY_TASKS;
		return SCH_MAX_TASKS;
	}
	//there is a free place available
	ScheduleTasks[Index].pTask = pFunction;
	ScheduleTasks[Index].Delay = Delay;
	ScheduleTasks[Index].Period = Period;
	ScheduleTasks[Index].RunMe = 0;
	//return position of task in array
	return Index;
}


uint8_t sDeleteTask(uint8_t TaskIndex)
{
	uint8_t ReturnValue;
	if(TaskIndex < SCH_MAX_TASKS) //is the index in bounds?
	{
		if(ScheduleTasks[TaskIndex].pTask == 0)
		{
			//no task at that position
			ErrorCode = ERROR_SCH_CANNOT_DELETE_TASK;
			ReturnValue = RETURN_ERROR;
		}
		else
		{
			ReturnValue = RETURN_NORMAL;
		}
		//clear task stats regardless
		ScheduleTasks[TaskIndex].pTask = 0;
		ScheduleTasks[TaskIndex].Delay = 0;
		ScheduleTasks[TaskIndex].Period = 0;
		ScheduleTasks[TaskIndex].RunMe = 0;
	}
	else
	{
		ErrorCode = ERROR_SCH_INDEX_OUT_BOUNDS;
		ReturnValue = RETURN_ERROR;
	}
	return ReturnValue;
}


void sReportStatus(void)
{

}

//Update function of the scheduler, detects if a new task is due to run. If so, it
//return true, otherwise false
void sUpdate(void)
{
	//Loop through entire task array
	for(uint8_t Index = 0; Index < SCH_MAX_TASKS; Index++)
	{
		//check if there is a task at this location
		if(ScheduleTasks[Index].pTask != 0)
		{
			if(ScheduleTasks[Index].Delay == 0) //is the task due to run?
			{
				ScheduleTasks[Index].RunMe += 1; //set the run flag of the task
				if(ScheduleTasks[Index].Period != 0) //is it a periodic task?
				{
					//then schedule it to run again
					ScheduleTasks[Index].Delay = ScheduleTasks[Index].Period;
				}
			}
			else //task is not due to run, decrement its delay
			{
				ScheduleTasks[Index].Delay -= 1;
			}
		}
	}
	//check if there has been a time slice overflow
	if(TaskRunning == true)
	{
		OverflowTasks++;
	}

}


void sResetTaskReleaseCounter()
{
	ReleasedTasks = 0;
}

uint32_t sReturnTaskReleaseCounter()
{
	return ReleasedTasks;
}


void sResetOverflowCounter()
{
	OverflowTasks = 0;
}


uint32_t sReturnOverflowCounter()
{
	return OverflowTasks;
}
