GLOBAL _read
GLOBAL _timerAppend
GLOBAL _timerRemove
GLOBAL _halt
GLOBAL _write
GLOBAL _clearScreen

section .text

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

_clearScreen:
    push rbp
    mov rbp, rsp

    mov rax, 5    ; syscall 5
    int 80h

    mov rsp, rbp
    pop rbp

	ret

_halt:
    hlt
    ret