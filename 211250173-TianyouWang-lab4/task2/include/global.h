
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* EXTERN is defined as extern except in global.c */
#ifdef	GLOBAL_VARIABLES_HERE
#undef	EXTERN
#define	EXTERN
#endif

EXTERN	int		ticks;

EXTERN	int		disp_pos;


extern int read_count;
extern int write_count;     /* read_count、write_count分别是等待读、写的进程数 */

extern SEMAPHORE read_sem;
extern SEMAPHORE write_sem;     /* read_sem、write_sem分别是文件的读锁和写锁 */

extern SEMAPHORE rc_sem;
extern SEMAPHORE wc_sem;
extern SEMAPHORE rlimit_sem;
extern SEMAPHORE fair_sem;


EXTERN	u8		gdt_ptr[6];	// 0~15:Limit  16~47:Base
EXTERN	DESCRIPTOR	gdt[GDT_SIZE];
EXTERN	u8		idt_ptr[6];	// 0~15:Limit  16~47:Base
EXTERN	GATE		idt[IDT_SIZE];

EXTERN	u32		k_reenter;

EXTERN	TSS		tss;
EXTERN	PROCESS*	p_proc_ready;

extern	PROCESS		proc_table[];
extern	char		task_stack[];
extern  TASK            task_table[];
extern	irq_handler	irq_table[];
extern  int workloads[];
extern  int sleepTicks[];

#define DOING_COLOR GREEN
#define WAITING_COLOR RED
#define SLEEPING_COLOR BLUE

#define sleepTime 1

EXTERN char *DOING_STR;
EXTERN char *WAITING_STR;
EXTERN char *SLEEPING_STR;

//EXTERN int t;