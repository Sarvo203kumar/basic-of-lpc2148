    AREA MyCode, CODE, READONLY
    ENTRY

start
    MOV R1, #0x1000       ; Load R1 with 4096 (0x1000)

    ADD R0, R1, #0xFF    ; Add 4095 to R1, result in R0 (R0 = 0x1FFF)
    SUB R2, R1, #0x800    ; Subtract 2048 from R1, result in R2 (R2 = 0x0800)
    AND R3, R1, #0xFF     ; Perform bitwise AND between R1 and 0xFF, result in R3 (R3 = 0x00)
    ORR R4, R1, #0x0F     ; Perform bitwise OR between R1 and 0x0F, result in R4 (R4 = 0x100F)
    EOR R5, R1, #0xFF     ; Perform bitwise XOR between R1 and 0xFF, result in R5 (R5 = 0x10FF)

    ; End the program (infinite loop)
end
    B end

    END
