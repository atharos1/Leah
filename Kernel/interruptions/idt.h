#ifndef _idt_
#define _idt_

void writeIDT();
void _sti();
void _cli();
void irqDispatcher(int n);
void kernelPanic();

#endif