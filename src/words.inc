    ;; init
    native "init", init
    mov rstack, rstack_start
    mov pc, prog_stub
    jmp next

    ;; implementation of any colon word
    native "docol", docol
    rpush pc
    add xt, cellsize
    mov pc, xt
    jmp next

    ;; return back from colon word
    native "decol", decol
    rpop pc
    jmp next

    ;; jump to location
    ;; location is an offset relative to the arg end
    native "branch", branch
    add pc, [pc]
    add pc, cellsize
    jmp next

    ;; jump to location if ToS(d) = 0
    ;; location is an offset relative to the arg end
    native "branch0", branch0
    pop rax
    test rax, rax
    jnz .skip
    add pc, [pc]
.skip:
    add pc, cellsize
    jmp next

    ;; Pushs a value immediately following this XT
    native "lit", lit
    dpush [pc]
    add pc, cellsize    ; skip the value pushed
    jmp next

    ;; interpreter
    colon "interpreter", interpreter
.start:
    dq xt_inbuf
    dq xt_word
    branch0 .end_of_input
    dq xt_drop
    dq xt_inbuf
    dq xt_find
    branch0 .no_word
    dq xt_wh2xt
    dq xt_execute
    branch .start
.no_word:
    dq xt_drop
    dq xt_inbuf
    dq xt_prints
    dq xt_lit
    msg_err_find    ; this address is pushed and skipped
    dq xt_prints
    branch .start
.end_of_input:
    dq xt_decol

    ;; (addr --)
    ;; Prints a null-terminated string
    native "prints", prints
    pop rdi
    call print_string
    jmp next

    ;; ( -- addr_to_input_buffer)
    native "inbuf", inbuf
    dpush input_buf
    jmp next

    ;; (addr -- len)
    ;; read a word from stdin to buffer pointed by addr
    native "word", word
    dpop rdi
    call read_word
    dpush rdx
    jmp next

    ;; (hoge -- )
    native "drop", drop
    dpop rdi
    jmp next

    ;; (addr_to_str -- addr_to_word_hdr)
    native "find", find
    mov rsi, [last_word]
.loop:
    dfetch rdi
    dpush rsi
    add rsi, cellsize    ; skip ptr to next word
    call string_equals
    dpop rsi
    test rax, rax
    jnz .found
    mov rsi, [rsi]       ; go to next word
    test rsi, rsi
    jnz .loop
.not_found:
    dpop rdi
    call print_string
    mov rdi, msg_err_find
    call print_string
    dpush 0
    jmp next
.found:
    dpost rsi
    jmp next

    ;; (addr_to_word_hdr -- addr_to_xt)
    native "wh2xt", wh2xt
    dpop rsi
    add rsi, cellsize    ; skip ptr to next word
.loop:
    mov al, [rsi]
    test al, al
    jz .end
    inc rsi
    jmp .loop
.end:
    add rsi, 2    ; skip terminating null byte and flag
    dpush rsi
    jmp next

    ;; (xt -- )
    ;; Execute word on Top of data stack
    native "execute", execute
    pop xt
    jmp [xt]

    ;; test for interpreter loop
    native "hello", hello
    mov rdi, msg_test
    call print_string
    jmp next
    
    ;; exit
    native "exit", exit
    mov rax, 60
    xor rdi, rdi
    syscall
