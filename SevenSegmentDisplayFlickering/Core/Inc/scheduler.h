#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#define SCH_MAX_TASKS (10)

//Scheduler Core functions
void sInit(void);
void sDispatchTasks(void);
void sUpdate(void);
uint8_t sAddTask(void (*) (void), const uint16_t, const uint16_t);
uint8_t sDeleteTask(uint8_t);
void sReportStatus(void);
void sResetTaskReleaseCounter();
uint32_t sReturnTaskReleaseCounter();
void sResetOverflowCounter();
uint32_t sReturnOverflowCounter();

//error codes
#define ERROR_SCH_TOO_MANY_TASKS (-1)
#define ERROR_SCH_CANNOT_DELETE_TASK (-2)
#define ERROR_SCH_INDEX_OUT_BOUNDS (-3)

//return code
#define RETURN_ERROR (-1)
#define RETURN_NORMAL (0)

#endif /* __SCHEDULER_H */
