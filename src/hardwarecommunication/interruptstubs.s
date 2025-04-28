
.set IRQ_BASE, 0x20

.section .text

# run 'nm interrupts.o' to see the symbols for the functions(this is because we haven't used extern "C"). We can now jump to the functions from the assembly files
.extern _ZN5nilos21hardwarecommunication16InterruptManager15HandleInterruptEhj  # symbol for nilos::hardwarecommunication::InterruptManager::HandleInterrupt function

.macro HandleException num
.global _ZN5nilos21hardwarecommunication16InterruptManager19HandleException\num\()Ev
_ZN5nilos21hardwarecommunication16InterruptManager19HandleException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN5nilos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev  # symbol for nilos::hardwarecommunication::InterruptManager::HandleInterruptRequest0x00 or 0x11 called below
_ZN5nilos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    pushl $0 # for the error code which also need to be pushed
    jmp int_bottom
.endm

HandleException 0x00
HandleException 0x01
HandleException 0x02
HandleException 0x03
HandleException 0x04
HandleException 0x05
HandleException 0x06
HandleException 0x07
HandleException 0x08
HandleException 0x09
HandleException 0x0A
HandleException 0x0B
HandleException 0x0C
HandleException 0x0D
HandleException 0x0E
HandleException 0x0F
HandleException 0x10
HandleException 0x11
HandleException 0x12
HandleException 0x13

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x02
HandleInterruptRequest 0x03
HandleInterruptRequest 0x04
HandleInterruptRequest 0x05
HandleInterruptRequest 0x06
HandleInterruptRequest 0x07
HandleInterruptRequest 0x08
HandleInterruptRequest 0x09
HandleInterruptRequest 0x0A
HandleInterruptRequest 0x0B
HandleInterruptRequest 0x0C
HandleInterruptRequest 0x0D
HandleInterruptRequest 0x0E
HandleInterruptRequest 0x0F
HandleInterruptRequest 0x31

int_bottom:

    # pushing(saving) all the registers because interrupt handler might overwrite them and cause problem for the process running in the userspace earlier
    # pusha
    # pushl %ds
    # pushl %es
    # pushl %fs
    # pushl %gs

    pushl %ebp
    pushl %edi
    pushl %esi

    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    # call C++ handler (in the interrupts.cpp file)
    pushl %esp
    push (interruptnumber)
    call _ZN5nilos21hardwarecommunication16InterruptManager15HandleInterruptEhj
    ; add %esp, 6
    mov %eax, %esp # switch the stack

    # restoring all the registers
    popl %eax
    popl %ebx
    popl %ecx
    popl %edx

    popl %esi
    popl %edi
    popl %ebp
    # pop %gs
    # pop %fs
    # pop %es
    # pop %ds
    # popa

    add $4, %esp

.global _ZN5nilos21hardwarecommunication16InterruptManager15InterruptIgnoreEv
_ZN5nilos21hardwarecommunication16InterruptManager15InterruptIgnoreEv:

    iret


.data
    interruptnumber: .byte 0