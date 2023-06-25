global white_cout
global red_cout
global blue_cout

section .data
red db 1Bh,'[31;1m',0
.len equ $-red
white db 1Bh,'[37;0m',0
.len equ $-white 
blue db 1Bh,'[34;1m',0
.len equ $-blue

section .text

white_cout:
	push rdi
	push rsi
	mov rax, 1
	mov rdi, 1
	mov rsi, white
	mov rdx, white.len
	syscall
	
	pop rsi
	pop rdi
	mov rcx, rsi ; size
	mov rsi, rdi ; *dest
	mov rdx, rcx ; 
	mov rdi, 1         ; fd; 0: stdin; 1: stdout; 2: stderr
	
	
	mov rax, 1         ; system call number
	syscall
	ret

red_cout:
	push rdi
	push rsi
	mov rax, 1
	mov rdi, 1
	mov rsi, red
	mov rdx, red.len
	syscall
	

	pop rsi
	pop rdi
	mov rcx, rsi ; size
	mov rsi, rdi ; *dest
	mov rdx, rcx ; 
	mov rdi, 1 
	mov rax, 1
	syscall
	ret

blue_cout:
	push rdi
	push rsi
	mov rax, 1
	mov rdi, 1
	mov rsi, blue
	mov rdx, blue.len
	syscall
	
	pop rsi
	pop rdi
	mov rcx, rsi ; size
	mov rsi, rdi ; *dest
	mov rdx, rcx ; 
	mov rdi, 1         ; fd; 0: stdin; 1: stdout; 2: stderr
	
	
	mov rax, 1         ; system call number
	syscall
	ret