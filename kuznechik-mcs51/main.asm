
start:
    ajmp main

$INCLUDE(tables_defines_mcu51.inc)
$INCLUDE(tables_data_mcu51.inc)
$INCLUDE(tables_galois_defines_mcu51.inc)
$INCLUDE(tables_galois_data_mcu51.inc)

testKey:
    db 088h, 099h, 0AAh, 0BBh, 0CCh, 0DDh, 0EEh, 0FFh
    db 000h, 011h, 022h, 033h, 044h, 055h, 066h, 077h
    db 0FEh, 0DCh, 0BAh, 098h, 076h, 054h, 032h, 010h
    db 001h, 023h, 045h, 067h, 089h, 0ABh, 0CDh, 0EFh

testPt:
    db 011h, 022h, 033h, 044h, 055h, 066h, 077h, 000h
    db 0FFh, 0EEh, 0DDh, 0CCh, 0BBh, 0AAh, 099h, 088h
    
testCt:
    db 07Fh, 067h, 09Dh, 090h, 0BEh, 0BCh, 024h, 030h
    db 05Ah, 046h, 08Dh, 042h, 0B9h, 0D4h, 0EDh, 0CDh

BIT_PARTS_128 set 16

; address - idata address (imm)
zero128imm macro address
    currentAddress set address
rept BIT_PARTS_128
    mov currentAddress, #0
    currentAddress set currentAddress+1
endm
endm

; address - idata address (address in reg)
; uses: address reg
zero128reg macro address
rept BIT_PARTS_128
    mov @address, #0
    inc address
endm
endm

; destination - idata address (imm)
; source - idata address (imm)
copy128imm macro destination, source
    currentDestination set destination
    currentSource set source
rept BIT_PARTS_128
    mov currentDestination, currentSource
    currentDestination set currentDestination+1
    currentSource set currentSource+1
endm
endm

; destination - idata address (address in reg)
; source - idata address (address in reg)
; uses: A, destination reg, source reg
copy128reg macro destination, source
rept BIT_PARTS_128
    mov A, @source
    mov @destination, A
    inc source
    inc destination
endm
endm

; x = x xor y
; x - idata address (imm)
; y - idata address (imm)
; uses: A
append128imm macro x, y
    currentX set x
    currentY set y
rept BIT_PARTS_128
    mov A, currentX
    xrl A, currentY
    mov currentX, A
    currentX set currentX+1
    currentY set currentY+1
endm
endm

; x - idata address (address in reg)
; y - idata address (address in reg)
; uses: A, x reg, y reg
append128reg macro x, y
rept BIT_PARTS_128
    mov A, @x
    xrl A, @y
    mov @x, A
    inc x
    inc y
endm
endm

; x = x xor y
; x - idata address (imm)
; y - idata address (reg)
; uses: A, y reg
append128immReg macro x, y
    currentX set x
rept BIT_PARTS_128
    mov A, currentX
    xrl A, @y
    mov currentX, A
    currentX set currentX+1
    inc y
endm
endm

; result - idata address (imm)
; x - idata address (imm)
; y - idata address (imm)
; uses: A
plus128imm macro result, x, y
    currentResult set result
    currentX set x
    currentY set y
rept BIT_PARTS_128
    mov A, currentX
    xrl A, currentY
    mov currentResult, A
    currentResult set currentResult+1
    currentX set currentX+1
    currentY set currentY+1
endm
endm

; result - idata address (address in reg)
; x - idata address (address in reg)
; y - idata address (address in reg)
; uses: A, result reg, x reg, y reg
plus128reg macro result, x, y
rept BIT_PARTS_128
    mov A, @x
    xrl A, @y
    mov @result, A
    inc x
    inc y
    inc result
endm
endm

; prepare DPTR register
; base - code address (imm)
; uses: DPTR
readCodeDataPrepare macro base
    mov DPTR, #base
endm

; read data from code memory based on: @(DPTR+A)
; uses: A
readCodeDataFinal macro result
    movc A, @A+DPTR
endm

; base - code address (imm)
; offset - offset relative to code address (imm)
; uses: DPTR, A, register
readCodeDataImm macro base, offset
    readCodeDataPrepare base
    mov A, #offset
    readCodeDataFinal
endm

; base - code address (imm)
; offset - offset relative to code address (imm)
; idataAddress - address (imm)
; count - value (imm)
; uses: DPTR, A
readCodeDataToMemoryImm macro base, offset, idataAddress, count
    readCodeDataPrepare base
    i set 0
rept count
    mov A, #offset + i
    readCodeDataFinal
    mov idataAddress + i, A
    i set i+1
endm
endm

; base - code address (imm)
; offset - offset relative to code address (reg)
; uses: DPTR, A register
readCodeDataReg macro base, offset
    readCodeDataPrepare base
    mov A, @offset
    readCodeDataFinal
endm

; baseHigh - code address high part (imm)
; baseLow - code address low part (imm)
; offset - offset relative to code address (imm)
; uses: DPTR, A, register
readCodeDataPartedImm macro baseHigh, baseLow, offset
    readCodeDataPrepare %(baseHigh << 8 | baseLow)
    mov A, #offset
    readCodeDataFinal
endm

; result - register (reg)
; baseHigh - code address high part (address in reg)
; baseLow - code address low part (address in reg)
; offset - offset relative to code address (address in reg)
; uses: DPTR, A, register
readCodeDataPartedReg macro baseHigh, baseLow, offset
    mov DPH, @baseHigh
    mov DPL, @baseLow
    mov A, @offset
    readCodeDataFinal
endm

; x - idata address (imm)
; array - code address (imm)
; uses: DPTR, A
convert128imm macro x, array
    currentX set x
    readCodeDataPrepare array
rept BIT_PARTS_128
    mov A, currentX
    readCodeDataFinal
    mov currentX, A
    currentX set currentX+1
endm
endm

; x - idata address (address in reg)
; array - code address (imm)
; uses: DPTR, A, x reg
convert128reg macro x, array
    currentX set x
    readCodeDataPrepare array
rept BIT_PARTS_128
    mov A, @x
    readCodeDataFinal
    mov @x, A
    inc x
endm
endm

; x - value (imm)
; y - value (reg)
; jumpZero - code address (imm)
; uses: A, R0
; options: yIsA
mulImmReg macro x, y, jumpZero
    local next
    
if x = 0
    jmp jumpZero
endif

if yIsA = 0
    mov A, y
endif
    jz jumpZero
    
    readCodeDataPrepare index_of
    readCodeDataFinal
    
    second set INDEX_OF_&x
    
    add A, #second + 1
    
    cpl C
    subb A, #0
    
  ;  jc next
 ;   dec A
;next:

    readCodeDataPrepare alpha_to
    readCodeDataFinal
    
endm

findLvec macro k
    source set KUZ_LVEC_&k
endm

; w - address (imm)
; uses: A, R0, R1, R2
kuzLImm macro w
    local cycle
    local last
    local continue
    
    mov R1, 16
cycle:
    mov R2, w + 15
    
    k set 14
rept 15
    local zero
    mov A, w + k
    mov w + k + 1, A
    findLvec %k
    yIsA set 1
    mulImmReg %source, A, zero
zero:
    
    xrl A, R2
    mov R2, A
    
    k set k-1
endm

    mov w + 0, R2
    
    djnz R1, continue

    sjmp last

continue:
    ljmp cycle

last:
    
    
endm

; w - address (imm)
; uses: A, R0, R1, R2
kuzLInvImm macro w
    local cycle
    
    mov R1, 16
cycle:
    mov R2, w + 0
    k set 0
rept 15
    local zero
    mov A, w + k + 1
    mov w + k, A
    source set KUZ_LVEC_&k
    mulImmReg source, A, zero
zero:
    
    xrl R2, A
    
    k set k+1
endm

    mov w + 15, R2
    
    djnz R1, cycle

endm

; key - address (imm)
; x - address (imm)
; uses: A, R0, R1, R2
kuzEncryptBlockImm macro key, x
    m set 0
rept 9
    append128imm x, %(key + (m * 16))
    convert128imm x, kuz_pi
    kuzLImm x
    
    m set m+1
endm

    append128imm x, %(key + (9 * 16))
endm

testKeyMemory idata 16
testDataMemory idata 48

main:
    readCodeDataToMemoryImm testKey, 0, testKeyMemory, 32
    readCodeDataToMemoryImm testPt, 0, testDataMemory, 16
    kuzEncryptBlockImm testKeyMemory, testDataMemory
    copy128imm 0, testDataMemory
    sjmp $

end
