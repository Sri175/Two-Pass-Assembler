; bubble_sort.asm
; (Corrected Version 3)
; Sorts the 'array' using the bubble sort algorithm.

main:
    ; --- Stack Setup ---
    ldc 0x1000
    a2sp
    adj -8      ; SP+0: i
                ; SP+1: j
                ; SP+2: temp_arr_j
                ; SP+3: temp_arr_j_plus_1
                ; SP+4: n
                ; SP+5: &array
                ; SP+6: scratch_A (for A/B swap)
                ; SP+7: scratch_B (for A/B swap)

    ; --- Initialization ---
    ldc n
    ldnl 0      ; Get value of n
    stl 4       ; Store n in SP+4
    ldc array
    stl 5       ; Store &array in SP+5
    ldc 0
    stl 0       ; i = 0

outer_loop:
    ; --- Outer loop condition: i < (n-1) ---
    ldl 4
    adc -1
    ldl 0
    sub         ; A = (n-1) - i
    brz outer_done      ; if A == 0 (i == n-1), stop.
    brlz outer_done     ; if A < 0 (i > n-1), stop.
    br outer_continue   ; if A > 0 (i < n-1), continue.

outer_continue:
    ldc 0
    stl 1       ; j = 0

inner_loop:
    ; --- Inner loop condition: j < (n-i-1) ---
    ldl 4
    ldl 0
    sub
    adc -1
    ldl 1
    sub         ; A = (n-i-1) - j
    brz inner_done      ; if A == 0, stop.
    brlz inner_done     ; if A < 0, stop.
    br inner_continue   ; if A > 0, continue.

inner_continue:
    ; --- COMPARISON ---
    ; Store arr[j] in SP+2
    ldl 5
    ldl 1
    add
    ldnl 0
    stl 2
    
    ; Store arr[j+1] in SP+3
    ldl 5
    ldl 1
    adc 1
    add
    ldnl 0
    stl 3
    
    ; Load them for comparison
    ldl 2 ; A = arr[j]
    ldl 3 ; B = A (arr[j]), A = arr[j+1]
    
    ; A = arr[j+1], B = arr[j]
    sub ; A = B - A = arr[j] - arr[j+1]
    brlz no_swap ; If (arr[j] <= arr[j+1]), no swap.

    ; --- SWAP ---
    
    ; GOAL 1: mem[addr_j] = arr[j+1]
    ; We need A = addr_j, B = arr[j+1]
    ldl 5   ; A = &array
    ldl 1   ; B = A, A = j
    add     ; A = addr_j
    ldl 3   ; B = A (addr_j), A = arr[j+1]
    ; At this point: A = arr[j+1], B = addr_j
    ; We must swap A and B using our new scratch space
    
    stl 6   ; mem[SP+6] = A (arr[j+1]). A = B (addr_j)
    stl 7   ; mem[SP+7] = A (addr_j).   A = A (addr_j)
    ldl 6   ; B = A (addr_j).           A = mem[SP+6] (arr[j+1])
    ldl 7   ; B = A (arr[j+1]).        A = mem[SP+7] (addr_j)
    
    ; Now: A = addr_j, B = arr[j+1]
    stnl 0  ; mem[A+0] = B -> mem[addr_j] = arr[j+1]
    
    
    ; GOAL 2: mem[addr_j+1] = arr[j]
    ; We need A = addr_j+1, B = arr[j]
    ldl 5   ; A = &array
    ldl 1   ; B = A, A = j
    adc 1   ; B = A, A = j+1
    add     ; A = &array + (j+1) = addr_j+1
    ldl 2   ; B = A (addr_j+1), A = arr[j]
    ; At this point: A = arr[j], B = addr_j+1
    ; We must swap A and B
    
    stl 6   ; mem[SP+6] = A (arr[j]).     A = B (addr_j+1)
    stl 7   ; mem[SP+7] = A (addr_j+1).  A = A (addr_j+1)
    ldl 6   ; B = A (addr_j+1).        A = mem[SP+6] (arr[j])
    ldl 7   ; B = A (arr[j]).           A = mem[SP+7] (addr_j+1)
    
    ; Now: A = addr_j+1, B = arr[j]
    stnl 0  ; mem[A+0] = B -> mem[addr_j+1] = arr[j]
    
no_swap:
    ldl 1
    adc 1
    stl 1
    br inner_loop

inner_done:
    ldl 0
    adc 1
    stl 0
    br outer_loop

outer_done:
    adj 8   ; Clean up the 8 stack slots
    HALT

; --- Data Section ---
n:      data 7
array:  data 50
        data 10
        data 30
        data 5
        data 2
        data 100 ; <-- ADD NEW NUMBER
        data 1   ; <-- ADD NEW NUMBER