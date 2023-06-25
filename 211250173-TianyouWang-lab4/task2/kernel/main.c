
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
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



PRIVATE void init_semaphore(SEMAPHORE *semaphore, int val)
{
	semaphore->value = val;
	semaphore->head = semaphore->tail = 0;
}

PRIVATE void init_all_semaphore()
{
	read_count = 0;
	write_count = 0;

	rc_sem.value = 1;
	rc_sem.head = rc_sem.tail = 0;

	wc_sem.value = 1;
	wc_sem.head = wc_sem.tail = 0;

	read_sem.value = 1;
	read_sem.head = read_sem.tail = 0;

	write_sem.value = 1;
	write_sem.head = write_sem.tail = 0;

	fair_sem.value = 1;
	fair_sem.head = fair_sem.tail = 0;

	rlimit_sem.value = MAX_READER_COUNT;
	rlimit_sem.head = rlimit_sem.tail = 0;
}


/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{

	TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	u16		selector_ldt	= SELECTOR_LDT_FIRST;
	int i;
	for (i = 0; i < NR_TASKS; i++) {
		strcpy(p_proc->p_name, p_task->name);	// name of the process
		p_proc->pid = i;			// pid

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;
		p_proc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| RPL_TASK;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = 0x1202; /* IF=1, IOPL=1 */

		p_proc->proc_state = PROC_STATE_READY;
		p_proc->wake_tick = get_ticks();
		p_proc->is_work = PROC_NOT_RUNNING;
		
		p_proc->work_load=workloads[i];
		p_proc->sleep_tick=sleepTicks[i];

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	//proc_table[0].ticks = proc_table[0].priority =  200; /* 0x96 */
	//proc_table[1].ticks = proc_table[1].priority =  200; /* 0x32 */
	//proc_table[2].ticks = proc_table[2].priority =  200; /* 0x1E */
	for(int m=0;m<NR_TASKS;++m){
		proc_table[m].ticks=proc_table[m].priority = 5;
	}

	k_reenter = 0;
	ticks = 0;

	init_all_semaphore();

	p_proc_ready	= proc_table;

        /* 初始化 8253 PIT */
        out_byte(TIMER_MODE, RATE_GENERATOR);
        out_byte(TIMER0, (u8) (TIMER_FREQ/HZ) );
        out_byte(TIMER0, (u8) ((TIMER_FREQ/HZ) >> 8));

        put_irq_handler(CLOCK_IRQ, clock_handler); /* 设定时钟中断处理程序 */
        enable_irq(CLOCK_IRQ);                     /* 让8259A可以接收时钟中断 */


	disp_pos = 0;
	for (i = 0; i < 80*25; i++) {
		disp_str(" ");
	}
	disp_pos = 0;

	disp_str("max reader num: ");
	disp_int(MAX_READER_COUNT);
	disp_str("\n");

	for (PROCESS *tmp = proc_table; tmp < proc_table + NR_TASKS - 1; tmp++)
			{
				if (tmp->is_work == PROC_RUNNING)
					disp_color_str(DOING_STR, DOING_COLOR);

				else if (tmp->proc_state == PROC_STATE_SLEEPING)
					disp_color_str(SLEEPING_STR, SLEEPING_COLOR);

				else
					disp_color_str(WAITING_STR, WAITING_COLOR);
			}

	sprint("\n");

	restart();

	while(1){}
}



/*======================================================================*
                              Read1~3
 *======================================================================*/
void Read1()
{
	while(1){
		//disp_color_str("r1.", BRIGHT | MAKE_COLOR(BLACK, RED));
		//disp_int(get_ticks());
		milli_delay(2*ONE_TIME_TICK);
	}
}

void Read2()
{
	while(1){
		//disp_color_str("r2.", BRIGHT | MAKE_COLOR(BLACK, RED));
		//disp_int(get_ticks());
		milli_delay(3*ONE_TIME_TICK);
	}
}

void Read3()
{
	while(1){
		//disp_color_str("r3.", BRIGHT | MAKE_COLOR(BLACK, RED));
		//disp_int(get_ticks());
		milli_delay(3*ONE_TIME_TICK);
	}
}

/*======================================================================*
                              Write1~2
 *======================================================================*/
void Write1()
{
	while(1){
		//disp_color_str("w1.", BRIGHT | MAKE_COLOR(BLACK, RED));
		//disp_int(get_ticks());
		milli_delay(3*ONE_TIME_TICK);
	}
}

void Write2()
{
	while(1){
		//disp_color_str("w2.", BRIGHT | MAKE_COLOR(BLACK, RED));
		//disp_int(get_ticks());
		milli_delay(4*ONE_TIME_TICK);
	}
}


/*======================================================================*
															 working process
 *======================================================================*/
/**
 * 读者、写者增加各自 count 并上锁
 */
void reader_count_lock(SEMAPHORE *semaphore)
{
	p(&rc_sem);
	if (read_count == 0)
		p(semaphore);
	read_count++;
	v(&rc_sem);
}

void writer_count_lock(SEMAPHORE *semaphore)
{
	p(&wc_sem);
	if (write_count == 0)
		p(semaphore);
	write_count++;
	v(&wc_sem);
}

void reader_count_unlock(SEMAPHORE *semaphore)
{
	p(&rc_sem);
	read_count--;
	if (read_count == 0)
		v(semaphore);
	v(&rc_sem);
}

void writer_count_unlock(SEMAPHORE *semaphore)
{
	p(&wc_sem);
	write_count--;
	if (write_count == 0)
		v(semaphore);
	v(&wc_sem);
}

void reader_first_read()
{
	PROCESS *proc = p_proc_ready;
	while (1)
	{
#ifdef PREVENT_HUNGER
		p(&fair_sem);
#endif
		reader_count_lock(&write_sem);
		// 限制读者数量
		p(&rlimit_sem);

#ifdef PREVENT_HUNGER
		v(&fair_sem);
#endif
		// 开始读
		proc->is_work = PROC_RUNNING;
		
		milli_delay(proc->work_load);

		v(&rlimit_sem);
		reader_count_unlock(&write_sem);

		// 结束读，休息
		sleep_without_time_piece(proc->sleep_tick);
	}
}

void reader_first_write()
{
	PROCESS *proc = p_proc_ready;
	while (1)
	{
#ifdef PREVENT_HUNGER
		p(&fair_sem);
#endif
		p(&write_sem);
		// 开始写
		proc->is_work = PROC_RUNNING;
		milli_delay(proc->work_load);

		v(&write_sem);

#ifdef PREVENT_HUNGER
		v(&fair_sem);
#endif
		// 结束写，休息
		sleep_without_time_piece(proc->sleep_tick);
	}
}

void writer_first_read()
{
	PROCESS *proc = p_proc_ready;
	while (1)
	{
#ifdef PREVENT_HUNGER
		p(&fair_sem);
#endif
		// 若有写者进程，会被阻塞在此。
		p(&read_sem);
		reader_count_lock(&write_sem);
		v(&read_sem);

		p(&rlimit_sem);

#ifdef PREVENT_HUNGER
		v(&fair_sem);
#endif
		// 开始读
		proc->is_work = PROC_RUNNING;
		milli_delay(proc->work_load);

		v(&rlimit_sem);
		reader_count_unlock(&write_sem);

		// 结束读，休息
		sleep_without_time_piece(proc->sleep_tick);
	}
}

void writer_first_write()
{
	PROCESS *proc = p_proc_ready;
	while (1)
	{

#ifdef PREVENT_HUNGER
		p(&fair_sem);
#endif
		writer_count_lock(&read_sem);

		p(&write_sem);

		// 开始写
		proc->is_work = PROC_RUNNING;
		milli_delay(proc->work_load);

		v(&write_sem);
		writer_count_unlock(&read_sem);

#ifdef PREVENT_HUNGER
		v(&fair_sem);
#endif
		// 结束写，休息
		sleep_without_time_piece(proc->sleep_tick);
	}
}


/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
	int workload = p_proc_ready->work_load;
	while (1)
	{
		// 打印 20 次
		for (int i = 1; i <= 20; i++)
		{
			// 数字打印补位（前导0）
			if (i < 10)
				sprint("0");
			char num_str[16];
			itoa_my(num_str, i);
			sprint(num_str);
			// 从 B 开始打印
			for (PROCESS *tmp = proc_table; tmp < proc_table + NR_TASKS - 1; tmp++)
			{
				if (tmp->is_work == PROC_RUNNING)
					disp_color_str(DOING_STR, DOING_COLOR);

				else if (tmp->proc_state == PROC_STATE_SLEEPING)
					disp_color_str(SLEEPING_STR, SLEEPING_COLOR);

				else
					disp_color_str(WAITING_STR, WAITING_COLOR);
			}
			sprint("\n");
			// 休息一个时间片
			sleep_without_time_piece(p_proc_ready->work_load);
		}
		// 阻塞
		while (1)
		{
		}
	}
}