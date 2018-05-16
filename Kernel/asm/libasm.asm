GLOBAL cpuVendor
GLOBAL pindonga

GLOBAL IO_IN
GLOBAL IO_OUT

GLOBAL RTC

GLOBAL readKey

section .text
	
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

pindonga:
	mov al, 0
	out 70h, al
	in al, 71h
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