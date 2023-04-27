//,Division_Program
//,NUM_1_/_NUM_2
//,NUM_1
VAL,#128
//,NUM_2
VAL,#7
//,Init
LDR,R0,3
LDR,R1,5
MOV,R2,R0
MOV,R3,#0
CMP,R1,#0
BEQ,21
//,Main
SUB,R2,R2,R1
ADD,R3,R3,#1
CMP,R1,R2
BEQ,13
BLT,13
STR,R3,30
STR,R2,31









VAL,#0
VAL,#0
HALT