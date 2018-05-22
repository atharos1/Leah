GLOBAL _read
GLOBAL _timerAppend
GLOBAL _timerRemove
GLOBAL _halt
GLOBAL _write

section .text

_write:
    push rbp
    mov rbp, rsp

    push rbx
    push rcx
    push rdx

    mov rax, 4      ; syscall 3
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

_timerAppend:
    push rbp
    mov rbp, rbp

    push rbx
    push rcx

    mov rax, 100    ; syscall 100
    mov rbx, rdx    ; function pointer
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
    mov rbx, rdx    ; function pointer
    int 80h

    pop rbx

    mov rsp, rbp
    pop rbp

	ret

_halt:
    hlt
    ret