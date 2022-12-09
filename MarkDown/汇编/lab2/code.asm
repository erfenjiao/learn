data segment
 negative db 0 ;负数
 positive db 0 ;正数
 zero db 0
 buf db -12,0,4,23,-25,0,14,-9,5,58
 negativeNum db 'negative: $' ;负数
 positiveNum db 'positive: $' ;正数
 zeroNum db 'zero: $'
data ends
code segment
 assume cs:code,ds:data
start:
  mov ax,data
  mov ds,ax
  mov si,0
  mov cx,10
L1:
  mov al,buf[si]
  cmp al,0
  jge next
  mov ah,negative
  add ah,1
  mov negative,ah
L2:
  inc si
  loop L1
halt:
  mov dx, offset negativeNum
  mov ah,9
  int 21h
  mov dl, negative
  add dl,30h
  mov ah,2
  int 21h
  mov dl,0dh
  mov ah,2
  int 21h
  mov dl,0ah
  mov ah,2
  int 21h
  mov dx, offset positiveNum
  mov ah,9
  int 21h
  mov dl, positive
  add dl,30h
  mov ah,2
  int 21h
  mov dl,0dh
  mov ah,2
  int 21h
  mov dl,0ah
  mov ah,2
  int 21h
  mov dx, offset zeroNum
  mov ah,9
  int 21h
  mov dl, zero
  add dl,30h
  mov ah,2
  int 21h
  mov ah,4ch
  int 21h
next:je zare
  mov ah,positive
  add ah,1
  mov positive,ah
  jmp L2
zare:mov ah,zero
  add ah,1
  mov zero,ah
  jmp L2
code ends
 end start
