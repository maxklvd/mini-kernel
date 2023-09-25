; (c) 2023 Max Kolesnikov
; This code is licensed under the GPL 2.0 license, available at the root
; application directory.

extern isr_page_fault :proc

.code

page_fault_wrapper proc
            ;
            ; Save general-purpose registers
            ;
            push    rax
            push    rcx
            push    rdx
            push    rbx
            push    rbp
            push    rsi
            push    rdi
            push    r8
            push    r9
            push    r10
            push    r11
            push    r12
            push    r13
            push    r14
            push    r15

            ;
            ; Get the error code from the stack
            ;
            mov     rcx, [rsp + 78h]
            ;
            ; Call #PF handler
            ;
            call    isr_page_fault

            ;
            ; Restore saved registers
            ;
            pop     r15
            pop     r14
            pop     r13
            pop     r12
            pop     r11
            pop     r10
            pop     r9
            pop     r8
            pop     rdi
            pop     rsi
            pop     rbp
            pop     rbx
            pop     rdx
            pop     rcx
            pop     rax

            add     rsp, 8                  ; Remove the error code
            iretq
page_fault_wrapper endp

;
; Gets the interrupt error code, if any
;
irq_error_code proc
            mov     rax, [rsp + 30h]        ; Error code
            ret
irq_error_code endp

end
