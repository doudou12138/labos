
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* klib.asm */
PUBLIC void	out_byte(u16 port, u8 value);
PUBLIC u8	in_byte(u16 port);
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);

/* protect.c */
PUBLIC void	init_prot();
PUBLIC u32	seg2phys(u16 seg);

/* klib.c */
PUBLIC void	delay(int time);

/* kernel.asm */
void restart();

/* main.c */
void TestA();
void Read1();
void Read2();
void Read3();
void Write1();
void Write2();
void reader_first_read();
void reader_first_write();
void writer_first_read();
void writer_first_write();

/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);


/* 以下是系统调用相关 */

/* proc.c */
PUBLIC  int     sys_get_ticks();        /* sys_call */
PUBLIC  void     sys_sprint(char* str);           /*sys_call*/
PUBLIC  void     sys_sleep_without_time_piece(int milli_second);
PUBLIC void     sys_p(SEMAPHORE* s);
PUBLIC void     sys_v(SEMAPHORE* s);

/* syscall.asm */
PUBLIC  void    sys_call();             /* int_handler */
PUBLIC  int     get_ticks();
PUBLIC void     sprint(char* str);
PUBLIC void     sleep_without_time_piece(int milli_second);
PUBLIC void     p(SEMAPHORE* s);
PUBLIC void     v(SEMAPHORE* s);

