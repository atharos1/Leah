GLOBAL cpuVendor

GLOBAL RTC
GLOBAL readKey

GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask

GLOBAL _int80handler
GLOBAL _cli
GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _halt

GLOBAL pruebaSysCallWrite

GLOBAL s

EXTERN irqDispatcher
EXTERN todesputes
EXTERN int80Handler

section .data ;prueba para int 80h
	hello:     db 'Hello world!'
	helloLen:  equ $-hello

section .text

s:
	push rbp
	mov rbp, rsp

	push rdi
	push rsi
	push rcx
	push rdx

	mov rax, rdi
	add rax, 50

	pop rdx
	pop rcx
	pop rsi
	pop rdi

	mov rsp, rbp
	pop rbp

	ret

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

_halt:
	hlt
	ret

;Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

pruebaSysCallWrite:
	mov rax, 4
    mov rbx, 1
    mov rcx, hello
    mov rdx, helloLen
    int 80h
	ret

;Keyboard
_irq01Handler:
	irqHandlerMaster 1
	
_int80handler: ;hay que recibir si o si en los regsitros de 32 bits? Y si una direccion de memoria no entra en uno de 32 bits?
	push rbp
	mov rbp, rsp

	push rdi
	push rsi
	push rcx
	push rdx

	mov rdi, rax
	mov rsi, rbx
	mov rax, rcx
	mov rcx, rdx ;ver como arreglamos este desastre para que no se pisen entre ellos!
	mov rdx, rax

	call int80Handler

	pop rdx
	pop rcx
	pop rsi
	pop rdi

	mov rsp, rbp
	pop rbp
	
	iretq
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

IO_OUT: ;DESTINO, ORIGEN
	mov rdx,rdi
	mov rax,rsi
	
	mov [80h], ax
	mov [70h], dx
	
	out dx,ax
	ret

IO_IN: ;ORIGEN
	mov rax,0
	mov rdx,rdi

	;mov [70h], rdi

	in ax,dx
	ret

RTC:
	mov rax,rdi
	out 70h,al
	in al,71h
	ret

readKey:
.loop:
	in al,64h
	test al,1
	jz .nothing
	mov rax,0
	in al,60h
	jmp .end
.nothing:
	mov rax,0
.end:
	ret

_sti:
	sti
	ret

_cli:
	cli
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out		0A1h,al
    pop     rbp
    retn