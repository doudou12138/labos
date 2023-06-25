
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
#include<stddef.h>

/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS *p; // 修改此结构体
	int current_tick = get_ticks();
	
	
	while (1)
	{
		p_proc_ready++;
		if (p_proc_ready >= proc_table + NR_TASKS)
		{
			//disp_str("haha");
			p_proc_ready = proc_table;
		}
		// 休眠 -> 就绪
		if (p_proc_ready->proc_state == PROC_STATE_SLEEPING &&
				p_proc_ready->wake_tick <= current_tick)
		{
			p_proc_ready->proc_state = PROC_STATE_READY;
			p_proc_ready->wake_tick = 0;
		}
		// 寻找进程
		if (p_proc_ready->proc_state == PROC_STATE_READY &&
				p_proc_ready->wake_tick <= current_tick)
		{
			break; // 寻找到进程
		}
	}
	
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}

/*======================================================================*
                           sys_sprint
 *======================================================================*/
PUBLIC void sys_sprint(char* str)
{
	disp_str(str);
}

/*======================================================================*
                    sys_sleep_without_time_piece
 *======================================================================*/
PUBLIC void sys_sleep_without_time_piece(int milli_second)
{

	p_proc_ready->wake_tick = get_ticks() + (milli_second /(1000/HZ) );
	//disp_int(p_proc_ready->wake_tick);
	p_proc_ready->proc_state = PROC_STATE_SLEEPING;
	p_proc_ready->is_work = PROC_NOT_RUNNING;

	schedule();
}




PRIVATE void enqueue(PROCESS *proc, SEMAPHORE *semaphore)
{
	// MAX_BLOCKED_PROC 大于 6。一定不会超出队列长处
	int tail = semaphore->tail;
	semaphore->waiting_queue[tail] = proc;
	// 在这里保证每次 tail 的正确性
	semaphore->tail = ((tail + 1) % MAX_BLOCKED_PROC);
}

PRIVATE PROCESS *dequeue(SEMAPHORE *semaphore)
{
	int head = semaphore->head;
	if (semaphore->tail == semaphore->head)
	{
		return NULL;
	}

	PROCESS *p = semaphore->waiting_queue[head];
	semaphore->head = ((head + 1) % MAX_BLOCKED_PROC);
	return p;
}

PRIVATE void block_process(SEMAPHORE *semaphore)
{

	p_proc_ready->proc_state = PROC_STATE_BLOCKED;
	enqueue(p_proc_ready, semaphore);
}

PRIVATE void recover_process(SEMAPHORE *semaphore)
{
	PROCESS *p = dequeue(semaphore);
	if (p != NULL)
	{
		p->proc_state = PROC_STATE_READY;
	}
}

/*======================================================================*
                    sys_p
 *======================================================================*/
PUBLIC void sys_p(SEMAPHORE* s)
{
	s->value--;
	if(s->value<0){
		block_process(s);
		schedule();
	}
}

/*======================================================================*
                    sys_v
 *======================================================================*/
PUBLIC void sys_v(SEMAPHORE* s)
{
	s->value++;
	if(s->value<=0){
		recover_process(s);
		schedule();
	}
}
