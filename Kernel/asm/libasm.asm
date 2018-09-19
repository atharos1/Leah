GLOBAL _cpuVendor

GLOBAL _RTC
GLOBAL _readKey

GLOBAL _picMasterMask
GLOBAL _picSlaveMask

GLOBAL _int80handler
GLOBAL _irq00Handler
GLOBAL _irq01Handler

GLOBAL _sti
GLOBAL _cli

GLOBAL _halt

GLOBAL _beep_start
GLOBAL _beep_stop

GLOBAL _ex00Handler
GLOBAL _ex06Handler

GLOBAL _rsp

GLOBAL stackPointerBackup
GLOBAL instructionPointerBackup

GLOBAL _initialize_stack_frame

GLOBAL _force_timer_tick
GLOBAL _force_scheduler


EXTERN irqDispatcher
EXTERN exDispatcher
EXTERN int80Handler
EXTERN schedule
EXTERN scheduler_nextTask

EXTERN end

section .bss
	stackPointerBackup:  resq 1
	instructionPointerBackup: resq 1

section .text

%macro pushStateNoRax 0
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

_executionWrapper:
	call rax

_initialize_stack_frame:
	mov rcx, rsp 
	mov rsp, rdx
	;mov rax, rsi
	push 0x0
	push rdx 
	push 0x202 
	push 0x08 
	push rdi
	mov rdi, rsi
	push 0x0
	pushStateNoRax
	mov rax, rsp
	mov rsp, rcx
	ret

_rsp:
	mov rax, rsp
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
	;cantidad de bytes que agrego al stackpointer: 15*8
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

%macro exHandlerMaster 1

	pushState

	mov rsi, rsp
	add rsi, 15*8 ; RSI APUNTE A LA DIRECCION DE RETORNO A LA FUNCION QUE FALLO
	mov rdx, rsp
	add rdx, 15*8
	add rdx, 3*8 ; RDX APUNTE A LA DIRECCION DE STACK DE LA FUNCION QUE FALLO

	mov rdi, %1 ; pasaje de parametro
	call exDispatcher

	popState

	iretq
%endmacro

_force_timer_tick:
	int 20h
	ret

_halt:
	hlt
	ret

_force_scheduler:
	mov rdi, rsp
	call scheduler_nextTask
	mov rsp, rax
	popState

	iretq

;Timer (Timer Tick)
_irq00Handler:
	;irqHandlerMaster 0
	pushState
	mov rdi, 0
	call irqDispatcher ;;ejecutamos la rutina de interrupcion del tick original
	mov rdi, rsp
	call schedule
	mov rsp, rax

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;DivByZero
_ex00Handler:
	exHandlerMaster 0

;InvalidOpCode
_ex06Handler:
	exHandlerMaster 6

_int80handler: ;hay que recibir si o si en los regsitros de 32 bits? Y si una direccion de memoria no entra en uno de 32 bits?
	push rbp
	mov rbp, rsp

	call int80Handler

	mov rsp, rbp
	pop rbp

	iretq

_cpuVendor:

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


_RTC:
	mov rax,rdi
	out 70h,al
	in al,71h
	ret

_readKey:
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

_picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

_picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out		0A1h,al
    pop     rbp
    retn

_beep_start:
	push rbp
	mov rbp, rsp

	mov al, 0xB6
	out 43h,al

	mov rbx, rdi
	mov rax, 0
	mov ax, bx

	;mov ax, 1193 ;1193180 / nFrequence;
	out 42h,al
	mov al,ah
	out 42h,al

	in al, 61h ;lo esta apagando?
 	or al, 03h
	out 61h,al

	mov rsp, rbp
	pop rbp
	ret

_beep_stop:
  	in al, 61h
	and al, 0xFC
	out 61h, al
  	ret
