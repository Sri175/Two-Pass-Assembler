; test1.asm - Simple test program
; Loads 10, adds 5, and then halts.

main:
    ldc 10     ; Load 10 into A
    adc 5      ; Add 5 (A = 10 + 5)
    HALT       ; Stop the machine

; This label points to the data
result: data 1234