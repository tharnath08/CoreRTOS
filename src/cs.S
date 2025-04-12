.global context_Switch
.type context_switch, %function


context_Switch:
    push {r4 - r7}
    mov r1, r8
    mov r2, r9
    mov r3, r10
    mov r4, r11
    push {r1 - r4}

break_here:
    ldr r1, [r0]
    mov sp, r1
    pop {r1 - r4}

    mov r8, r1
    mov r9, r2
    mov r10, r3
    mov r11, r4

    pop {r4 - r7}    
@     @ bx lr