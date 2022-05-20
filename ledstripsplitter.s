; calling convention of avr-gcc
; r0 is a scratch register
; r1 is the zero register
; r18-r27, r30, 31 are call clobbered, so are free for us to use
; r2-r17, r28, r29 are call saved, i.e. need to be restored
; arguments are passed below R26, with even stride downwards
; i.e. two bytes will be passed in r24, r22 etc.
; LSB is in the lowest register
; return values are returned via the same allocation
; 16 bit pointer regs:
; X = R27:R26, Y = R29:R28, Z = R31:R30

; register/stack mapping
; r18: reset counter, or segment counter
; r19: current output port bitmask
; X (r27:r26): current output port toggle register
; r25:r24: bit counter within a single segment
; r23:r22: amount of bits per segment, constant
; Z (r31:r30): contains a pointer 

; initialization

entry_point:
ldi r31, HIGH_DATASTRUCTURE ; high byte of Z 
ldi r30, LOW_DATASTRUCTURE ; low byte of Z
ldi r27, X_HIBYTE ; high byte of X, as it is constant between all the PORT memory spaces (0x04)
ldi r31, Z_HIBYTE ; high byte of Z, the address of the segment datastructure
ldi r30, Z_LOBYTE
ldi r23, BITCOUNT_HI
ldi r22, BITCOUNT_LO


; reset await loop prologue
ldi r18, RESET_CYCLES ; (166)
reset_loop: ; this loops 166 times, and is 6 cycles long for about 1000 cycles, which at 20MHz is 50 us
    sbic INPUT_PORT, INPUT_BIT ; 1 cycle + 2 if next is skipped
    rjmp entry_point ; 2 cycles
    dec r18 ; 1 cycle
    brne reset_loop ; 2 cycles if taken

; segment loop prologue
ldi r18, SEGMENT_COUNT ; amount of segments to go through

segment_loop:
    ; setup for the current segment. need to load the right address in X, and the right bitmask in R20.
    ; both are read from a small datastructure. said data structure should be in the Z pointer. also iterate through it here
    ld r26, Z+ ; 2 cycles, load low byte of X
    ld r20, Z+ ; 2 cycles, load the correct bitmask

    movw r24, r22 ; 1 cycle, move bitcount to counter
    bit_loop:

        rising_edge:
            sbis INPUT_PORT, INPUT_BIT ; 1 cycle, + 2 if next skipped
            rjmp rising_edge ; 2 cycles

        ; rising edge detected, toggle current output pin
        st X, r19 ; 1 cycle

        falling_edge:
            sbic INPUT_PORT, INPUT_BIT ; 1 cycle, + 2 if next skipped
            rjmp falling_edge ; 2 cycles

        ; falling edge detected, toggle current output pin
        st X, r19 ; 1 cycle

        sbiw r24, 1 ; 2 cycles
        brne bit_loop ; 2 cycles if taken, 1 if not

    dec r18 ; 1 cycle
    brne segment_loop ; 2 cycles if taken, 1 if not

rjmp entry_point
; if this proves too slow (or a variable bitcount is needed)
; unrolling segment_loop so there's just one special loop for each segment
; would probably be the best idea






