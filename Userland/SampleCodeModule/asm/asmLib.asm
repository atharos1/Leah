GLOBAL _read
GLOBAL _timerAppend
GLOBAL _timerRemove
GLOBAL _halt
GLOBAL _write
GLOBAL _clearScreen
GLOBAL _rtc
GLOBAL _setFontColor
GLOBAL _setBackgroundColor
GLOBAL _enableCursor
GLOBAL _drawPixel
GLOBAL _setFontSize
GLOBAL _setCursor
GLOBAL _setGraphicCursorStatus
GLOBAL _throwInvalidOpCode
GLOBAL _beep

GLOBAL _rsp

section .text

_rsp:
	mov rax, rsp
	ret

_enableCursor:

    push rbp
    mov rbp, rsp

    push rax
    push rcx

    mov ah, 1
    mov ch, 10
    mov cl, 12
    int 10h

    pop rcx
    pop rax

    mov rsp, rbp
    pop rbp
    ret

_timerAppend:
    push rbp
    mov rbp, rsp

    push rbx
    push rcx

    mov rax, 100    ; syscall 100
    mov rbx, rdi    ; function pointer
    mov rcx, rsi    ; ticks
    int 80h

    pop rcx
    pop rbx

    mov rsp, rbp
    pop rbp

	ret

_setCursor:
    push rbp
    mov rbp, rsp

    push rbx
    push rcx

    mov rax, 10    ; syscall 10
    mov rbx, rdi    ; function pointer
    mov rcx, rsi    ; ticks
    int 80h

    pop rcx
    pop rbx

    mov rsp, rbp
    pop rbp

	ret

_timerRemove:
    push rbp
    mov rbp, rsp

    push rbx

    mov rax, 101    ; syscall 101
    mov rbx, rdi    ; function pointer
    int 80h

    pop rbx

    mov rsp, rbp
    pop rbp

	ret

_setGraphicCursorStatus:
    push rbp
    mov rbp, rsp

    push rbx

    mov rax, 11    ; syscall 11
    mov rbx, rdi    ; status
    int 80h

    pop rbx

    mov rsp, rbp
    pop rbp

	ret

_throwInvalidOpCode:
    rsm
    ret

_rtc:
    push rbp
    mov rbp, rsp

    push rbx

    mov rax, 200    ; syscall 101
    mov rbx, rdi    ; what to fetch
    int 80h

    pop rbx

    mov rsp, rbp
    pop rbp

	ret

_write:
    push rbp
    mov rbp, rsp

    push rbx
    push rcx
    push rdx

    mov rax, 4      ; syscall 4
    mov rbx, rdi    ; file descriptor
    mov rcx, rsi    ; buffer
    mov rdx, rdx    ; count
    int 80h

    pop rdx
    pop rcx
    pop rbx

    mov rsp, rbp
    pop rbp

	ret

_read:
    push rbp
    mov rbp, rsp

    push rbx
    push rcx
    push rdx

    mov rax, 3      ; syscall 3
    mov rbx, rdx    ; file descriptor
    mov rcx, rsi    ; buffer
    mov rdx, rdi    ; count
    int 80h

    pop rdx
    pop rcx
    pop rbx

    mov rsp, rbp
    pop rbp

	ret

_drawPixel:
    push rbp
    mov rbp, rsp

    push rbx
    push rcx
    push rdx

    mov rax, 9      ; syscall 9
    mov rbx, rdi    ; x
    mov rcx, rsi    ; y
    mov rdx, rdx    ; color
    int 80h

    pop rdx
    pop rcx
    pop rbx

    mov rsp, rbp
    pop rbp

	ret

_clearScreen:
    push rbp
    mov rbp, rsp

    mov rax, 5    ; syscall 5
    int 80h

    mov rsp, rbp
    pop rbp

	ret


_setFontColor:
    push rbp
    mov rbp, rsp

    push rbx

    mov rax, 6    ; syscall 6
    mov rbx, rdi    ; foreColor
    int 80h

    pop rbx

    mov rsp, rbp
    pop rbp

	ret

_setBackgroundColor:
    push rbp
    mov rbp, rsp

    push rbx

    mov rax, 7    ; syscall 7
    mov rbx, rdi    ; backColor
    int 80h

    pop rbx

    mov rsp, rbp
    pop rbp

	ret

_setFontSize:
    push rbp
    mov rbp, rsp

    push rbx

    mov rax, 8    ; syscall 8
    mov rbx, rdi    ; fontSize
    int 80h

    pop rbx

    mov rsp, rbp
    pop rbp

		ret

_beep:
		push rbp
		mov rbp, rsp

		push rbx
		push rcx

		mov rax, 102    ; syscall 102
		mov rbx, rdi  ; frequence
		mov rcx, rsi	; duration
		int 80h

		pop rcx
		pop rbx

		mov rsp, rbp
		pop rbp

		ret


_halt:
    hlt
    ret
