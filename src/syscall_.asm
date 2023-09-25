; (c) 2023 Max Kolesnikov
; This code is licensed under the GPL 2.0 license, available at the root
; application directory.

.data

;
; User stack frame
;
align 16
user_stack_low      db 1024 dup (?)
user_stack_pointer:
user_stack_high     db 1024 dup (?)

;
; Stores the kernel stack pointer
;
ring0_rsp           dq 0
;
; Stores the user function pointer
;
ring3_func          dq 0

.code

;
; Configures syscall and sysret calls
;
setup_syscall proc
            ;
            ; rcx - syscall_cs_ss
            ; rdx - sysret_cs_ss
            ;

            or      rdx, 3                  ; Add RPL
            shl     rdx, 16
            mov     dx, cx
            mov     r8, rdx                 ; Save the result in R8

            mov     rcx, 0c0000080h         ; EFER MSR
            rdmsr
            or      eax, 1                  ; Enable SCE bit
            wrmsr
            mov     rcx, 0c0000081h         ; STAR MSR
            rdmsr
            mov     edx, r8d                ; Set syscall/sysret selectors from R8
            wrmsr

            mov     rcx, 0c0000082h         ; LSTAR MSR
            mov     rax, syscall_entry      ; EDX:EAX = syscall_entry
            mov     rdx, rax
            shr     rdx, 32
            wrmsr

            ret
setup_syscall endp

;
; A wrapper function that calls the user function and returns to the kernel
;
user_func_wrapper proc
            ;
            ; Call the user function
            ;
            call    [ring3_func]

            ;
            ; Return to the kernel after executing the user function
            ;
            xor     rax, rax
            syscall
user_func_wrapper endp

;
; Calls the specified function in user mode through sysret
;
usermode_enter proc
            ;
            ; rcx - user_function
            ;

            mov     [ring0_rsp], rsp
            mov     [ring3_func], rcx

            mov     rcx, user_func_wrapper
            mov     rsp, user_stack_pointer
            mov     r11, 202h               ; eflags
            sysretq                         ; Go to userspace
usermode_enter endp

;
; Exits the current user task and returns to the kernel
;
usermode_exit proc
            cmp     [ring3_func], 0
            jz      no_userspace
            ;
            ; Exit from user mode
            ;
            xor     rax, rax
            syscall

no_userspace:
            ;
            ; Do nothing, just return
            ;
            ret
usermode_exit endp

;
; SYSCALL handler
;
syscall_entry proc
            ;
            ; rax - system call number, ignored, any syscall execution leads to a return to the kernel
            ;

            mov     [ring3_func], 0         ; Clear the user function pointer

            mov     rsp, [ring0_rsp]
            mov     rax, [rsp]
            add     rsp, 8
            jmp     rax                     ; Continue execution in the kernel
syscall_entry endp

end
