org 0100h
    mov ax, 0B800h
    mov gs, ax
    mov ah, 0Fh         ;0000:black background, 1111:white word
    mov al, 'L'
    ;mov [gs:((80*0+39)*2)], ax          ;0th row,39th column on the screen
