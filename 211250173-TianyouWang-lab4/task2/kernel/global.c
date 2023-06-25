
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE
#define READER_FIRST

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "proto.h"
#include "global.h"


PUBLIC	PROCESS			proc_table[NR_TASKS];

PUBLIC	char			task_stack[STACK_SIZE_TOTAL];




PUBLIC TASK task_table[NR_TASKS] = {
#ifdef READER_FIRST
		{reader_first_read, STACK_SIZE_READ1, "Read1"},
		{reader_first_read, STACK_SIZE_READ2, "Read2"},
		{reader_first_read, STACK_SIZE_READ3, "Read3"},
		{reader_first_write, STACK_SIZE_WRITE1, "Write1"},
		{reader_first_write, STACK_SIZE_WRITE2, "Write2"},
		{TestA, STACK_SIZE_TESTA, "A"},
#else
		{writer_first_read, STACK_SIZE_READ1, "Read1"},
		{writer_first_read, STACK_SIZE_READ2, "Read2"},
		{writer_first_read, STACK_SIZE_READ3, "Read3"},
		{writer_first_write, STACK_SIZE_WRITE1, "Write1"},
		{writer_first_write, STACK_SIZE_WRITE2, "Write2"},
		{TestA, STACK_SIZE_TESTA, "A"},
#endif
};

PUBLIC	irq_handler		irq_table[NR_IRQ];

PUBLIC	system_call		sys_call_table[NR_SYS_CALL] = {sys_get_ticks,sys_sleep_without_time_piece,sys_sprint,sys_p,sys_v};

PUBLIC int workloads[NR_TASKS] = {
		2 * ONE_TIME_TICK,
		3 * ONE_TIME_TICK,
		3 * ONE_TIME_TICK,
		3 * ONE_TIME_TICK,
		4 * ONE_TIME_TICK,
		1 * ONE_TIME_TICK,
};

PUBLIC int sleepTicks[NR_TASKS]={
    sleepTime * ONE_TIME_TICK,
    sleepTime * ONE_TIME_TICK,
    sleepTime * ONE_TIME_TICK,
    sleepTime * ONE_TIME_TICK,
    sleepTime * ONE_TIME_TICK,
    0 * ONE_TIME_TICK,
};


PUBLIC int read_count = 0;	// num of readers
PUBLIC int write_count = 0; // for writer prepare to write

PUBLIC SEMAPHORE fair_sem;	 // 公平读写，访饿死
PUBLIC SEMAPHORE rlimit_sem; // 读者数量限制
PUBLIC SEMAPHORE rc_sem;		 // 增加 read_count 的信号量
PUBLIC SEMAPHORE wc_sem;		 // 增加 write_count 的信号量
PUBLIC SEMAPHORE read_sem;	 // 读信号量
PUBLIC SEMAPHORE write_sem;	 // 写信号量

PUBLIC char *DOING_STR = "  O";
PUBLIC char *WAITING_STR = "  X";
PUBLIC char *SLEEPING_STR = "  Z";

