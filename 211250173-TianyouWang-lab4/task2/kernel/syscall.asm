
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

%include "sconst.inc"

_NR_get_ticks       equ 0 ; 要跟 global.c 中 sys_call_table 的定义相对应！
INT_VECTOR_SYS_CALL equ 0x90

_NR_SLEEP_WITHOUT_TIME_PIECE       equ 1 ; 要跟 global.c 中 sys_call_table 的定义相对应！
INT_SLEEP_SYS_CALL equ 0xA0

_NR_SPRINT 			equ 2	;要跟 global.c 中 sys_call_table 的定义相对应！
INT_SPRINT_SYS_CALL equ 0xB0

_NR_P 				equ 3	;要跟 global.c 中 sys_call_table 的定义相对应！
INT_P_SYS_CALL		equ 0xC0

_NR_V 				equ 4	;要跟 global.c 中 sys_call_table 的定义相对应！
INT_V_SYS_CALL		equ 0xD0

; 导出符号
global	get_ticks
global 	sleep_without_time_piece
global 	sprint
global 	p
global  v

bits 32
[section .text]

; ====================================================================
;                              get_ticks
; ====================================================================
get_ticks:
	mov	eax, _NR_get_ticks
	int	INT_VECTOR_SYS_CALL
	ret


; ====================================================================
;                              sleep_without_time_piece
; ====================================================================
sleep_without_time_piece:
	mov	eax, _NR_SLEEP_WITHOUT_TIME_PIECE
	push ebx
	mov ebx, [esp+8]
	int	INT_SLEEP_SYS_CALL
	pop ebx
	ret

; ====================================================================
;                              sprint
; ====================================================================
sprint:
	mov eax, _NR_SPRINT
	push ebx
	mov ebx, [esp+8]
	int INT_SPRINT_SYS_CALL
	pop ebx
	ret

; ====================================================================
;                              p
; ====================================================================
p:
	mov eax, _NR_P
	push ebx
	mov ebx,[esp+8]
	int INT_P_SYS_CALL
	pop ebx
	ret

; ====================================================================
;                             v
; ====================================================================
v:
	mov eax, _NR_V
	push ebx
	mov ebx,[esp+8]
	int INT_V_SYS_CALL
	pop ebx
	ret