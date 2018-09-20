GLOBAL _throwInvalidOpCode

GLOBAL _rsp

GLOBAL _systemCall

section .text

_systemCall: ;en rdx recibe el código de la syscall
	push rbp
	mov rbp, rsp

	int 80h

	mov rsp, rbp
	pop rbp
	ret

_throwInvalidOpCode:
    rsm
    ret
