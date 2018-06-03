GLOBAL cpuVendor

GLOBAL RTC
GLOBAL readKey
GLOBAL poolKey

GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask

GLOBAL _int80handler
GLOBAL _cli
GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _halt

GLOBAL _beep_start
GLOBAL _beep_stop

GLOBAL _in
GLOBAL _out

GLOBAL _ex00Handler
GLOBAL _ex06Handler

;Getters de los registros
GLOBAL _rax
GLOBAL _rbx
GLOBAL _rcx
GLOBAL _rdx
GLOBAL _rbp
GLOBAL _rsp
GLOBAL _rdi
GLOBAL _rsi
GLOBAL _rip
GLOBAL _r8
GLOBAL _r9
GLOBAL _r10
GLOBAL _r11
GLOBAL _r12
GLOBAL _r13
GLOBAL _r14
GLOBAL _r15
GLOBAL _rsp_set
GLOBAL _rdi_set

GLOBAL _popState
GLOBAL _pushState

GLOBAL stackPointerBackup
GLOBAL instructionPointerBackup


EXTERN irqDispatcher
EXTERN exDispatcher
EXTERN int80Handler

section .bss
	stackPointerBackup:  resq 1
	instructionPointerBackup: resq 1

section .text

_rax:
	ret
_rbx:
	mov rax, rbx
	ret
_rcx:
	mov rax, rcx
	ret
_rdx:
	mov rax, rdx
	ret
_rbp:
	mov rax, rbp
	ret
_rsp:
	mov rax, rsp
	ret
_rip:
	;mov rax, rel $
	ret
_rdi:
	mov rax, rdi
	ret
_rsi:
	mov rax, rsi
	ret
_r8:
	mov rax, r8
	ret
_r9:
	mov rax, r9
	ret
_r10:
	mov rax, r10
	ret
_r11:
	mov rax, r11
	ret
_r12:
	mov rax, r12
	ret
_r13:
	mov rax, r13
	ret
_r14:
	mov rax, r14
	ret
_r15:
	mov rax, r15
	ret

_rsp_set:
	mov rsp, rdi
	ret

_rdi_set:
	mov rdi, rdi
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
	mov rax, 15*8 ;cantidad de bytes que agrego al stackpointer
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
	add rsi, rax ; RSI APUNTE A LA DIRECCION DE RETORNO A LA FUNCION QUE FALLO
	mov rdx, rsp
	add rdx, rax
	add rdx, 3*8 ; RDX APUNTE A LA DIRECCION DE STACK DE LA FUNCION QUE FALLO

	mov rdi, %1 ; pasaje de parametro
	call exDispatcher

	popState

	iretq
%endmacro

_popState:
	popState

_pushState:
	pushState

_halt:
	hlt
	ret

;Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

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

poolKey:
.loop:
	in al,64h
	test al,1
	jnz .nothing
	in al,60h
	jmp .end
.nothing:
	mov rax,0
	jmp .loop
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

_in:
	mov rax,0
	mov rdx,rdi
	in ax,dx
	ret

;rdi port
;rsi value
_out:
	mov rdx,rdi
	mov rax,rsi
	out dx,ax
	ret

_beep_start:
	push rbp
	mov rbp, rsp

	mov al, 182; B6h FIJO?
	out 43h,al

	mov rbx, rdi
	mov rax, 0
	mov ax, bx

	;mov ax, 1193 ;1193180 / nFrequence;
	out 42h,al
	mov al,ah
	out 42h,al
	;in al, 61h ;lo esta apagando?
	mov al, 03h
	out 61h,al

	mov rsp, rbp
	pop rbp
	ret

_beep_stop:
  ;in al, 61h
	mov al, 00h
	out 61h,al
  ret
