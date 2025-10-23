; sum_n.asm
; Calculates the sum of 'n' numbers stored in 'array'.
; The result will be stored in the 'sum' variable.

main:
    ; --- Stack Setup ---
    ldc 0x1000  ; Set up a stack pointer high in memory
    a2sp
    adj -3      ; Reserve space for 3 local variables:
                ; SP+0: current sum
                ; SP+1: counter (i)
                ; SP+2: pointer to array (ptr)

    ; --- Initialization ---
    ldc 0
    stl 0       ; sum = 0
    ldc n       ; Load address of 'n'
    ldnl 0      ; Load value of 'n' (A=5)
    stl 1       ; i = 5
    ldc array   ; Load base address of 'array'
    stl 2       ; ptr = &array

loop:
    ; --- Loop Condition (check if i == 0) ---
    ldl 1       ; Load counter 'i'
    brz done    ; If i == 0, jump to 'done'

    ; --- Loop Body ---
    ldl 2       ; Load 'ptr'
    ldnl 0      ; A = memory[ptr] (get array value)
    ldl 0       ; Load 'sum'
    add         ; A = sum + memory[ptr]
    stl 0       ; Store new 'sum'

    ; --- Update Pointers/Counters ---
    ldl 2       ; Load 'ptr'
    adc 1       ; ptr++
    stl 2       ; Store new 'ptr'

    ldl 1       ; Load 'i'
    adc -1      ; i--
    stl 1       ; Store new 'i'

    br loop     ; Repeat

done:
    ; --- Store Final Result ---
    ldl 0       ; Load final 'sum' from stack
    ldc sum     ; Load address of 'sum' variable
    stnl 0      ; memory[&sum] = A (store sum)

    ; --- Cleanup & Halt ---
    adj 3       ; Clean up stack
    HALT

; --- Data Section ---
n:      data 5
array:  data 10
        data 20
        data 5
        data 2
        data 8
sum:    data 0      ; Result will be stored here (45)