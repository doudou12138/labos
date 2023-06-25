max_in_len equ 203
array_len equ 100

section .data
    
    DIV_BY_ZERO db "Error:divided_by_zero"
    FORMAT_ERROR db "please key in the correct format"
    quotient_show db "Quotient: "
    And_show db " "
    remainder_show db "Remainder: "

    DIV_BY_ZERO_LEN equ 21
    FORMAT_ERROR_LEN equ 32
    quo_show_len equ 10
    and_show_len equ 1
    rem_show_len equ 11

section .bss
    input_buffer:   resb max_in_len    ; the whole input string
    dividend:     resd array_len      ; the int array which stores the first num
    divisor:     resd array_len 
    quotient:    resd array_len
    dividend_len: resd 1              ; the length of dividend
    divisor_len: resd 1              ; the length of divisor
    quotient_len: resd 1

section .text
    global _start
    
_start:

    call get_inp 
    
    call parse_inp

    call calculate_and_print
    
    call exit

;----------------------------------------------------------------------------
; 函数名: get_inp
;----------------------------------------------------------------------------
; 作用:
;;输入格式：被除数和除数之间用一个空格分隔，以回车结尾
; 调用系统调用 3（read）从标准输入读取用户输入
; 存储到 input_buffer 中，最多读取 203 个字符
get_inp:
    push eax
    push ebx
    push ecx
    push edx

    mov eax, 3      ; 系统调用号 3（read）
    mov ebx, 0      ; 标准输入文件描述符为 0
    mov ecx, input_buffer    ; 缓冲区地址
    mov edx, max_in_len 
    int 0x80        ; 调用系统调用

    pop eax
    pop ebx
    pop ecx
    pop edx
    ret
;----------------------------------------------------------------------------



;----------------------------------------------------------------------------
; 函数名: parse_inp
;----------------------------------------------------------------------------
; 
parse_inp:
    push    eax
    push    ebx
    push    ecx
    push    edx
    push    esi
    push    edi

    mov     eax, input_buffer
    call    get_str_len
    mov     ebx, eax ; ebx = slen
    dec     ebx      ; ignore the line separator
    mov     ecx, 0

find_operator_index:
    cmp     ecx, ebx ; if reach end of string, quit
    jge     strs2int_arrays
    cmp     byte [input_buffer + ecx], ' ' ; 
    jz      strs2int_arrays
    inc     ecx
    jmp     find_operator_index

strs2int_arrays:
    push    dividend_len
    push    ecx      ; end of the first string
    push    dword 0  ; start Index
    push    dividend
    call    str2array
    pop     eax
    pop     eax
    pop     ecx
    pop     eax

    inc     ecx      ; ecx here represent the starting Index

    push    divisor_len
    push    ebx      ; end of the string
    push    ecx      ; start index of second string
    push    divisor
    call    str2array
    pop     eax
    pop     ecx
    pop     ebx
    pop     eax

    pop     edi
    pop     esi
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax
    ret
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; 函数名: str2array
;----------------------------------------------------------------------------
; 作用:
str2array:
  push    ebp
  mov     ebp, esp          ; use ebp to point func stack
  push    eax
  push    ebx
  push    ecx
  push    edx
  push    esi
  push    edi
  mov     esi, [ebp + 12]    ; start(0)->esi
  mov     edi, [ebp + 16]    ; end_index->edi

set_array_len:
  mov     eax, [ebp + 20]    ; num_array1_len->eax
  mov     ebx, edi           ; end_index->ebx
  sub     ebx, esi           ; end_index - index->ebx
  mov     [eax], ebx         ; set len of array
  mov     ecx, esi           ; index->ecx
  mov     edx, [ebp + 8]    ; num_array1->edx
  mov     ebx,0

 str2arr_loop:
  mov     eax, 0
  mov     al, [input_buffer + ecx]
  sub     al, '0'       ; al = int(input_string[ecx])
  mov     [edx + ebx], eax  ; set str2int_array
  
  inc     ecx           ; index++
  inc     ebx
  cmp     ecx, edi      
  jnge str2arr_loop 

  pop     edi
  pop     esi
  pop     edx
  pop     ecx
  pop     ebx
  pop     eax
  leave
  ret
;----------------------------------------------------------------------------


;----------------------------------------------------------------------------
; 函数名: get_str_len
;----------------------------------------------------------------------------
; 作用:获取字符串的长度,地址通过eax传进来，返回长度也通过eax
get_str_len:
  push    ebx
  mov     ebx, eax
 
next_char:
  cmp     byte[eax], 0
  jz      string_len_finished
  inc     eax
  jmp     next_char
 
string_len_finished:
  sub     eax, ebx
  pop     ebx
  ret

;----------------------------------------------------------------------------


;----------------------------------------------------------------------------
; 函数名: calculate_and_print
;----------------------------------------------------------------------------
; 作用:
calculate_and_print:
    call    check
    call    div_calculate          ; 必须要在这里call一下
    call    print
    ret
;----------------------------------------------------------------------------


;----------------------------------------------------------------------------
; 函数名:   check
;----------------------------------------------------------------------------
; 作用:
check:
    cmp byte [divisor], 0 
    je sor_head_zero
    jne not_sor_head_zero

    sor_head_zero:
      cmp dword [divisor_len], 1

      je  divided_by_zero
      jne format_error
  
    not_sor_head_zero:
      cmp byte [dividend], 0
      je end_head_zero
      jne div_calculate
    
    end_head_zero:
      cmp dword [dividend_len], 1
      je  div_calculate
      jne format_error

    ret
;----------------------------------------------------------------------------


;----------------------------------------------------------------------------
; 函数名:   div_calculate
;----------------------------------------------------------------------------
; 作用:
div_calculate:
      push edx
      push esi
      push edi
      push ebx
      push ecx
      push eax
      

      mov esi, dividend 
      mov edi, quotient
      mov eax, 0


      push eax
      mov eax,0
      mov [quotient_len],eax   ;quotient_len=0
      pop eax

      ;the dividend is 0
      cmp byte [dividend],0
      jne calcu_loop
      cmp dword [dividend_len],1
      mov dword [quotient_len],1
      je end

calcu_loop:
      mov byte [edi+eax],0   ;set the quotient
      add dword [quotient_len], 1

head_zero:      
        cmp byte [esi],0
        jne no_head_zero
        inc esi
        push ecx
        add ecx,eax
        add ecx,dividend
        add ecx,[divisor_len]
        cmp esi,ecx
        pop ecx
        jge no_head_zero

no_head_zero:    
      push eax
      add eax,[divisor_len]
      cmp eax, [dividend_len]
      pop eax
      jg end

      push ecx
      push edi
      push eax
      push edx
      
      mov edi,divisor

      add eax,dividend
      add eax,[divisor_len]
      sub eax,esi
      ;mov eax,dividend+[divisor_len]+eax-esi
      mov edx,[divisor_len]
      ; 作用: 传入esi，edi；esi为待比较的首地址，edi为除数的首地址,
      ;传出在ecx，
      call compare_two_num
      pop edx
      pop eax
      pop edi
      mov edx,ecx
      pop ecx

      mov ebx,dividend
      add ebx,[divisor_len]
      add ebx,eax
      sub ebx,esi
      ;mov ebx,dividend+[divisor_len]+eax-esi

      cmp edx,1; record the number in the quotient
      mov edx,0
      jne calcuForOne
      
      
      push eax
      add eax,[divisor_len]
      cmp eax,[dividend_len]
      ;cmp [divisor_len]+eax,[dividend_len]
      pop eax

      jge end
      inc eax
      jmp calcu_loop

calcuForOne:
;迭代后被除数首地址以及计算位数esi,ebx,
;除数首地址以及计算位数 constant
      
      push edi
      push eax
      push edx

      mov edi, divisor
      mov eax, ebx
      mov edx, [divisor_len]
      call compare_two_num

      pop edx
      pop eax
      pop edi

      cmp ecx,1
      mov [edi+eax],dl
      inc eax
      je calcu_loop

      dec eax
      inc dl

      mov ecx,1     ;for iter
forOneLoop:
      

      push esi
      push edi
      push ebx
      push ecx
      add esi,ebx

      sub esi,ecx
      mov edi,divisor
      add edi,[divisor_len]
      sub edi,ecx
      mov byte bl,[esi]
      mov byte cl,[edi]
      cmp bl,cl
      ;cmp byte [esi+ebx-ecx],[divisor+[divisor_len]-ecx]
      pop ecx
      pop ebx
      pop esi
      pop edi

      
      jnge borrow

      push esi
      push edi
      add esi,ebx
      sub esi,ecx
      add edi,edx
      sub edi,ecx
      push ecx
      mov ecx,[edi]
      
      sub [esi],ecx
      
      
      pop ecx
      pop edi
      pop esi
      ;sub [esi+ebx-ecx],[edi+edx-ecx]


      inc ecx
      cmp ecx,[divisor_len]
      jg  calcuForOne
      jng forOneLoop

borrow:
      push edi
      push edx

      push esi
      add esi,ebx
      sub esi,ecx
      mov edi,esi
      pop esi
      ;mov edi, esi+ebx-ecx
      
      add byte [edi],10
  borrow_loop:
      dec edi
      movzx edx,byte [edi]
      cmp edx,0
      mov byte [edi],9
      je  borrow_loop
      
      mov [edi],edx-1

      pop edx
      pop edi
      jmp forOneLoop


end:
      pop eax
      pop ecx
      pop ebx
      pop edi
      pop esi
      pop edx
      ret
;----------------------------------------------------------------------------


;----------------------------------------------------------------------------
; 函数名:   compare_two_num
;----------------------------------------------------------------------------
;para: esi,eax,edi,edx;return ecx
compare_two_num:
      push esi
      push eax
      push edi
      push edx
delete_headzero:
      cmp  byte [esi],0
      jne  compare_real
      
      inc  esi
      dec  eax
      cmp  eax, 0
      jne  delete_headzero
      mov  ecx, 1
      je   end_compare

compare_real:
      cmp eax,edx
      mov ecx,0
      jg  end_compare
      mov ecx,1
      jnge end_compare

      mov ecx,0
compare_loop:
      push esi
      push edi
      push eax
      push ebx
      add esi,ecx
      add edi,ecx
      mov eax,0
      mov edx,0
      mov byte al,[esi]
      mov byte dl,[edi]
      cmp al,dl
      pop ebx
      pop eax
      pop edi
      pop esi
      ;cmp [esi+ecx], [edi+ecx]
      
      mov ecx,0
      jg  end_compare
      mov ecx,1
      jnge end_compare
      cmp ecx,eax-1
      mov ecx, 2
      jge  end_compare
      inc ecx
      jmp compare_loop

end_compare:
      pop  edx
      pop  edi
      pop  eax
      pop  esi
      ret
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; 函数名: exit
;----------------------------------------------------------------------------
; 作用:
exit:
	mov     eax, 1
	mov     ebx, 0
	int     80h
	ret	
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; 函数名: print
;----------------------------------------------------------------------------
print:
    push eax
    push ebx
    push ecx
    push edx
    push esi


    ; 输出quotient的内容
    mov edx,[quotient_len]
    mov esi,quotient
    call seen

    mov eax, 4      ; 系统调用号 4（write）
    mov ebx, 1      ; 标准输出文件描述符为 1
    mov ecx, quotient    ; 缓冲区地址
    mov edx, [quotient_len]    ; 缓冲区长度
    int 0x80        ; 调用系统调用

    mov eax, 4      ; 系统调用号 4（write）
    mov ebx, 1      ; 标准输出文件描述符为 1
    mov ecx, And_show
    mov edx, and_show_len
    int 0x80        ; 调用系统调

    mov edx,[dividend_len]
    mov esi,dividend
    call seen

    ; 输出remainder的内容
    mov eax, 4      ; 系统调用号 4（write）
    mov ebx, 1      ; 标准输出文件描述符为 1
    mov ecx, dividend
    mov edx, [dividend_len]
    int 0x80        ; 调用系统调用

    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
;----------------------------------------------------------------------------


;----------------------------------------------------------------------------
; 函数名: seen
;----------------------------------------------------------------------------
seen:
    push eax
    push ecx

    mov ecx,0     ;iter
add_zero:
    mov     eax, 0
    mov     al, [esi + ecx]
    add     al, '0'       ; 
    mov     [esi + ecx], al 
    inc     ecx
    cmp     ecx,edx
    jnge    add_zero


    pop ecx
    pop eax
    ret 
;----------------------------------------------------------------------------


;----------------------------------------------------------------------------
; 函数名: error
;----------------------------------------------------------------------------
error:
    divided_by_zero:
      push eax
      mov ecx,DIV_BY_ZERO
      mov edx,DIV_BY_ZERO_LEN
      jmp print_error
    format_error:
      push eax
      mov ecx,FORMAT_ERROR
      mov edx,FORMAT_ERROR_LEN
      jmp print_error

print_error:
      mov eax, 4      ; 系统调用号 4（write）
      mov ebx, 1      ; 标准输出文件描述符为 1
      int 0x80       
      call exit