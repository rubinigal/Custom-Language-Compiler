.entry LENGTH
.extern W
MAIN: moV r3 ,LENGTH
LOOP: jmp MAIN (#-1,r6)
prn #-5a
bne W(r4,r5
sub r1, #4
bne L3
1: inc K
.entry LOOPP
bne (K,W)
END: stop r3
STR: .string "abcdef
LENGTH: .da 6,-9,15
K: .data 22 44
.extern LENGTH