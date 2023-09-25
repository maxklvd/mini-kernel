; (c) 2023 Max Kolesnikov
; This code is licensed under the GPL 2.0 license, available at the root
; application directory.

.code

;
; Gets the CS register value
;
read_cs     proc
            mov     rax, cs
            ret
read_cs     endp

;
; Gets the SS register value
;
read_ss     proc
            mov     rax, ss
            ret
read_ss     endp

;
; Gets the task register value
;
read_tr     proc
            xor     rax, rax
            str     ax
            ret
read_tr     endp

;
; Sets the task register value
;
write_tr    proc
            ltr     cx
            ret
write_tr    endp

;
; Reloads segment registers CS, DS, ES, FS, GS, and SS
;
reload_registers    proc
            ;
            ; rcx - code_sel
            ; rdx - data_sel
            ;

            push    rcx                     ; Push new CS
            mov     rax, offset reload
            push    rax                     ; Push RIP
            retfq                           ; Far return
reload:
            mov     ds, dx                  ; KERNEL_DATA
            mov     es, dx
            mov     fs, dx
            mov     gs, dx
            mov     ss, dx
            ret
reload_registers endp

end
