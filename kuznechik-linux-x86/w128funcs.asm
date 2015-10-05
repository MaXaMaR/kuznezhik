
bits 64

%define MIN_BITS 8
%define MAX_BITS 128

%ifndef BITS
%define BITS 8
%endif

%ifndef USE_MUL_TABLE
%define USE_MUL_TABLE 0
%endif

%ifndef USE_TABLES
%define USE_TABLES 0
%endif

%define BIT_PARTS_8 (MAX_BITS / 8)
%define BIT_PARTS_16 (MAX_BITS / 16)
%define BIT_PARTS_32 (MAX_BITS / 32)
%define BIT_PARTS_64 (MAX_BITS / 64)

%define MIN_REGISTER_SIZE 8
%define MAX_REGISTER_SIZE 64

%define OPTIMIZED 1
%define MUL_TABLE 2
%define TABLES 3

%include "lib.inc"
%include "tables_defines.inc"
%include "tables_data.inc"
%if !USE_MUL_TABLE
%include "tables_galois_defines.inc"
%include "tables_galois_data.inc"
%endif
%if USE_MUL_TABLE
%include "tables_mul_defines.inc"
%include "tables_mul_data.inc"
%endif
%if USE_TABLES
%include "tables_precompiled_data.inc"
%endif
%include "tables_keys_defines.inc"

; 1 - call
; 2 - arguments count
%rmacro bitscall 1-*
	%1%[BITS]_internal %{2:-1}
%endmacro

global _zero128asm8
global _zero128asm16
global _zero128asm32
global _zero128asm64
global _zero128asm

; 1 - pointer of x
; 2 - type size in bits
%rmacro _zero128asm_proto 2
%assign i 0
%rep BIT_PARTS_%2
	mov %[TYPE_%2] [%1 + i * (%2 / 8)], 0
%assign i i+1
%endrep
%endmacro

; 1 - pointer of x
%rmacro _zero128asm8_internal 1
	_zero128asm_proto %1, 8
%endmacro

external _zero128asm8, 1

; 1 - pointer of x
%rmacro _zero128asm16_internal 1
	_zero128asm_proto %1, 16
%endmacro

external _zero128asm16, 1

; 1 - pointer of x
%rmacro _zero128asm32_internal 1
	_zero128asm_proto %1, 32
%endmacro

external _zero128asm32, 1

; 1 - pointer of x
%rmacro _zero128asm64_internal 1
	_zero128asm_proto %1, 64
%endmacro

external _zero128asm64, 1

; 1 - pointer of x
%rmacro _zero128asm_internal 1
	bitscall _zero128asm, %1
%endmacro

external _zero128asm, 1

global _copy128asm8
global _copy128asm16
global _copy128asm32
global _copy128asm64
global _copy128asm

%macro copy123 3
	push rcx
	push rdi
	push rsi
%if %1 != rdi
	mov rdi, %1
%endif
%if %1 != rsi
	mov rsi, %2
%endif
	mov rcx, 16
%if %3 == 8
	rep movsb
%elif %3 == 16
	rep movsw
%elif %3 == 32
	rep movsd
%elif %3 == 64
	rep movsq
%endif
	pop rsi
	pop rdi
	pop rcx
%endmacro

; 1 - pointer of destination
; 2 - pointer of source
; 3 - register name
; 4 - type size in bits
%rmacro _copy128asm_proto 4
%xdefine copy_destination %1
%xdefine copy_source %2
%xdefine copy_register %3
%xdefine copy_bits %4
%assign i 0
%rep BIT_PARTS_%[copy_bits]
	mov %[REGISTER_PART_%[copy_register]_%[copy_bits]], [%[copy_source] + i * (%[copy_bits] / 8)]
	mov [%[copy_destination] + i * (%[copy_bits] / 8)], %[REGISTER_PART_%[copy_register]_%[copy_bits]]
%assign i i+1
%endrep
;	copy123 %1, %2, %4
%endmacro

; 1 - pointer of destination
; 2 - pointer of source
; uses: rax
%rmacro _copy128asm8_internal 2
	_copy128asm_proto %{1:-1}, a, 8
%endmacro

external _copy128asm8, 2

; 1 - pointer of destination
; 2 - pointer of source
; uses: rax
%rmacro _copy128asm16_internal 2
	_copy128asm_proto %{1:-1}, a, 16
%endmacro

external _copy128asm16, 2

; 1 - pointer of destination
; 2 - pointer of source
; uses: rax
%rmacro _copy128asm32_internal 2
	_copy128asm_proto %{1:-1}, a, 32
%endmacro

external _copy128asm32, 2

; 1 - pointer of destination
; 2 - pointer of source
; uses: rax
%rmacro _copy128asm64_internal 2
	_copy128asm_proto %{1:-1}, a, 64
%endmacro

external _copy128asm64, 2

; 1 - pointer of destination
; 2 - pointer of source
; uses: rax
%rmacro _copy128asm_internal 2
	bitscall _copy128asm, %{1:-1}
%endmacro

external _copy128asm, 2

; 1 - pointer of y (w128)
; 2 - register (pointer of x)
; 3 - register 2
; 4 - register 3
; 5 - offset
; 6 - type size in bits
; auto-optimized version for big bits numbers
%rmacro append128_operation 6
%define append_y %1
%xdefine append_register0 %2
%xdefine append_offset %5
%xdefine append_bits %6
%ifndef %1_0
	xor %[REGISTER_PART_%[append_register0]_%[append_bits]], [%[append_y] + %[append_offset] * (%[append_bits] / 8)]
%else
	gather_def_value append_y, append_offset, append_bits
	;%xdefine gather_value 0
	;%error append_y, append_offset, append_bits
	xor %[REGISTER_PART_%[append_register0]_%[append_bits]], gather_value
%endif
%endmacro

; 1 - pointer of y (array)
; 2 - register (pointer of x)
; 3 - register 2
; 4 - register 3
; 5 - offset
; 6 - type size in bits
; manually optimize for big bits numbers
%rmacro convert128_operation 6
%xdefine convert_y %1
%xdefine convert_register0 %[REGISTER_PART_%2_%[MAX_REGISTER_SIZE]]
%xdefine convert_register0min %[REGISTER_PART_%2_8]
%xdefine convert_register1 %[REGISTER_PART_%3_%[MAX_REGISTER_SIZE]]
%xdefine convert_register2 %[REGISTER_PART_%4_%[MAX_REGISTER_SIZE]]
%xdefine convert_offset %5
%xdefine convert_bits %6
%if %[convert_bits] = 8
	movzx %[convert_register0], byte [%[convert_y] + %[convert_register0]]
%else
	xor %[convert_register2], %[convert_register2]
%assign k 0
%rep %[convert_bits] / 8
	movzx %[convert_register1], %[convert_register0min]
	movzx %[convert_register1], byte [%[convert_y] + %[convert_register1]]
%if k != 0
	shl %[convert_register1], k * 8
%endif
;%if k != 0
;	shl %[convert_register2], 8
;%endif
%if k != (%[convert_bits] / 8) - 1
	shr %[convert_register0], 8
%endif
	add %[convert_register2], %[convert_register1]
%assign k k+1
%endrep
	mov %[convert_register0], %[convert_register2]
%endif
%endmacro

; 1 - pointer of source
; 2 - pointer of result
; 3 - used register
; 4 - used register 2
; 5 - used register 3
; 6 - type size in bits
; 7 - first operation
; 8 - first operation arg
; ...
%rmacro _multi_op 6-*
%xdefine n ((%0 - 6) / 2)
%xdefine source %1
%xdefine result %2
%xdefine register0 %3
%xdefine register1 %4
%xdefine register2 %5
%xdefine bits %6
%assign i 0
%rep BIT_PARTS_%[bits]
%if %[bits] = %[MAX_REGISTER_SIZE] || %[bits] = 32 ; fix for 32 -> 64
	mov %[REGISTER_PART_%[register0]_%[bits]], [%[source] + i * (%[bits] / 8)]
%else
	movzx %[REGISTER_PART_%[register0]_%[MAX_REGISTER_SIZE]], %[TYPE_%[bits]] [%[source] + i * (%[bits] / 8)]
%endif
%rotate 6
%assign l 0
%rep n
	%1 %2, register0, register1, register2, i, bits
%assign l l+1
%rotate 2
%endrep
	mov [%[result] + i * (%[bits] / 8)], %[REGISTER_PART_%[register0]_%[bits]]
%assign i i+1
%endrep
%endmacro

; 1 - pointer of source
; 2 - pointer of result
; 3 - used register
; 4 - used register 2
; 5 - used register 3
; 6 - type size in bits
; 7 - first operation
; 8 - first operation arg
; 9 - second operation
; 10 - second operation arg
%rmacro _double_op 10
	_multi_op %{1:-1}
%endmacro

global _append128asm8
global _append128asm16
global _append128asm32
global _append128asm64
global _append128asm

; 1 - pointer of x
; 2 - pointer of y
; 3 - register
; 4 - type size in bits
%rmacro _append128asm_proto 4
%xdefine append_x %1
%define append_y %2
%xdefine append_register %3
%xdefine append_bits %4
%assign i 0
%rep BIT_PARTS_%[append_bits]
	mov %[REGISTER_PART_%[append_register]_%[append_bits]], [%[append_x] + i * (%[append_bits] / 8)]
%ifndef %2_0
	xor %[REGISTER_PART_%[append_register]_%[append_bits]], [%[append_y] + i * (%[append_bits] / 8)]
%else
	gather_def_value append_y, i, append_bits
	;%xdefine gather_value 0
	xor %[REGISTER_PART_%[append_register]_%[append_bits]], gather_value
%endif
	mov [%[append_x] + i * (%[append_bits] / 8)], %[REGISTER_PART_%[append_register]_%[append_bits]]
%assign i i+1
%endrep
%endmacro

; 1 - pointer of x
; 2 - pointer of y
; uses: rdx
%rmacro _append128asm8_internal 2
	_append128asm_proto %{1:-1}, d, 8
%endmacro

external _append128asm8, 2

; 1 - pointer of x
; 2 - pointer of y
; uses: rdx
%rmacro _append128asm16_internal 2
	_append128asm_proto %{1:-1}, d, 16
%endmacro

external _append128asm16, 2

; 1 - pointer of x
; 2 - pointer of y
; uses: rdx
%rmacro _append128asm32_internal 2
	_append128asm_proto %{1:-1}, d, 32
%endmacro

external _append128asm32, 2

; 1 - pointer of x
; 2 - pointer of y
; uses: rdx
%rmacro _append128asm64_internal 2
	_append128asm_proto %{1:-1}, d, 64
%endmacro

external _append128asm64, 2

; 1 - pointer of x
; 2 - pointer of y
; uses: rdx
%rmacro _append128asm_internal 2
	bitscall _append128asm, %{1:-1}
%endmacro

external _append128asm, 2

global _plus128asm8
global _plus128asm16
global _plus128asm32
global _plus128asm64
global _plus128asm

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of y
; 4 - register
; 5 - type size in bits
%rmacro _plus128asm_proto 5
%xdefine plus_result %1
%xdefine plus_x %2
%xdefine plus_y %3
%xdefine plus_register %4
%xdefine plus_bits %5
%assign i 0
%rep BIT_PARTS_%[plus_bits]
	mov %[REGISTER_PART_%[plus_register]_%[plus_bits]], [%[plus_x] + i * (%[plus_bits] / 8)]
%ifndef %3_0
	xor %[REGISTER_PART_%[plus_register]_%[plus_bits]], [%[plus_y] + i * (%[plus_bits] / 8)]
%else
	gather_def_value plus_y, i, plus_bits
	xor %[REGISTER_PART_%[plus_register]_%[plus_bits]], gather_value
%endif
	mov [%[plus_result] + i * (%[plus_bits] / 8)], %[REGISTER_PART_%4_%[plus_bits]]
%assign i i+1
%endrep
%endmacro

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of y
; uses: rax
%rmacro _plus128asm8_internal 3
	_plus128asm_proto %{1:-1}, a, 8
%endmacro

external _plus128asm8, 3

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of y
; uses: rax
%rmacro _plus128asm16_internal 3
	_plus128asm_proto %{1:-1}, a, 16
%endmacro

external _plus128asm16, 3

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of y
; uses: rax
%rmacro _plus128asm32_internal 3
	_plus128asm_proto %{1:-1}, a, 32
%endmacro

external _plus128asm32, 3

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of y
; uses: rax
%rmacro _plus128asm64_internal 3
	_plus128asm_proto %{1:-1}, a, 64
%endmacro

external _plus128asm64, 3

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of y
%rmacro _plus128asm_internal 3
	bitscall _plus128asm, %{1:-1}
%endmacro

external _plus128asm, 3

global _convert128asm

; 1 - pointer of x
; 2 - pointer of array
; uses: rax
%rmacro _convert128asm_internal 2
%assign i 0
%rep BIT_PARTS_8
	movzx rax, byte [%1 + i]	; load byte
	movzx rax, byte [%2 + rax]	; convert it using the same register
	mov [%1 + i], al 			; save byte
%assign i i+1
%endrep
%endmacro

external _convert128asm, 2

global _plus128multi_asm8
global _plus128multi_asm16
global _plus128multi_asm32
global _plus128multi_asm64
global _plus128multi_asm

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of array
; 4 - type size in bits
; uses: rax, rcx, rdx, r8
%rmacro _plus128multi_asm_proto 4

	; zero128(result);
	;_zero128asm%4_internal %1

%ifndef %3_0
	; r8 <- array[0]
%if IS_REGISTER(%3)
	mov r8, %3
%else
	lea r8, [rel %3]
%endif
%endif

	; for (int i = 0; i < MAX_BIT_PARTS; i++)
;	xor rcx, rcx
;%%cycle:
%assign k 0
%rep BIT_PARTS_8
	; rax <- ACCESS_128_VALUE_8(x, i)
	movzx rax, byte [%2 + k]
;	movzx rax, byte [%2 + rcx]

	; rax <- ACCESS_128_VALUE_8(x, i) * 16
	shl rax, 4

%ifdef %3_0
	; r8 <- array[0]
	lea r8, [rel %3_%[k]]
%endif

	; rax <- array[i][ACCESS_128_VALUE_8(x, i)]
	add rax, r8

%if k == 0
%ifdef %3_0
	_copy128asm_proto %1, rax, d, %4
%else
	_copy128asm_proto %1, rax, d, %4
%endif
%else
	; append128(result, array[i][ACCESS_128_VALUE_8(x, i)])
	; use: rdx
	_append128asm%4_internal %1, rax
%endif

%ifndef %3_0
	; r8 <- array[i++]
	add r8, 256 * 16
%endif

;	add rcx, 1
;	cmp rcx, 16
;	jne %%cycle
%assign k k+1
%endrep

%endmacro

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of array
; uses: rax, r8, r9, rdx
%rmacro _plus128multi_asm8_internal 3
	_plus128multi_asm_proto %{1:-1}, 8
%endmacro

external _plus128multi_asm8, 3

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of array
%rmacro _plus128multi_asm16_internal 3
	_plus128multi_asm_proto %{1:-1}, 16
%endmacro

external _plus128multi_asm16, 3

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of array
%rmacro _plus128multi_asm32_internal 3
	_plus128multi_asm_proto %{1:-1}, 32
%endmacro

external _plus128multi_asm32, 3

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of array
%rmacro _plus128multi_asm64_internal 3
	_plus128multi_asm_proto %{1:-1}, 64
%endmacro

external _plus128multi_asm64, 3

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of array
%rmacro _plus128multi_asm_internal 3
	bitscall _plus128multi_asm, %{1:-1}
%endmacro

external _plus128multi_asm, 3

global _append128multi_asm8
global _append128multi_asm16
global _append128multi_asm32
global _append128multi_asm64
global _append128multi_asm

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of array
; 4 - type size in bits
; uses: rax, rcx, rdx, r8, r9
%rmacro _append128multi_asm_proto 4

	; plus128multi(result, x, array);
	_plus128multi_asm%4_internal %1, %2, %3

	; copy128(x, result);
	_copy128asm%4_internal %2, %1

%endmacro

%rmacro _append128multi_asm8_internal 3
	_append128multi_asm_proto %{1:-1}, 8
%endmacro

external _append128multi_asm8, 3

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of array
%rmacro _append128multi_asm16_internal 3
	_append128multi_asm_proto %{1:-1}, 16
%endmacro

external _append128multi_asm16, 3

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of array
%rmacro _append128multi_asm32_internal 3
	_append128multi_asm_proto %{1:-1}, 32
%endmacro

external _append128multi_asm32, 3

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of array
%rmacro _append128multi_asm64_internal 3
	_append128multi_asm_proto %{1:-1}, 64
%endmacro

external _append128multi_asm64, 3

; 1 - pointer of result
; 2 - pointer of x
; 3 - pointer of array
%rmacro _append128multi_asm_internal 3
	bitscall _append128multi_asm, %{1:-1}
%endmacro

external _append128multi_asm, 3

; TODO: DO SOMETHING!!!!!!!!!!
; 1 - pointer of result
; 2 - pointer of array
; uses: r10, rcx, rax, r8, rdx
%rmacro _sum128asm_internal 2

	; zero128(result);
	_zero128asm_internal %1

	; r10 <- array[0]
	mov r10, %2

	; don't optimize that
	mov rcx, BIT_PARTS_8
%%cycle:
;%assign m 0
;%rep BIT_PARTS_8

	; r9 <- array[m]
	add r10, 16

	; for (int i = 0; i < MAX_BIT_PARTS; i++)
%assign k 0
%rep BIT_PARTS_8
	; rax <- ACCESS_128_VALUE_8(x, i)
	movzx rax, byte [r10 + k]

	; rax <- ACCESS_128_VALUE_8(x, i) * 16
	shl rax, 4

	; rax <- array[i][ACCESS_128_VALUE_8(x, i)]
	add rax, r8

	; append128(result, array[i][ACCESS_128_VALUE_8(x, i)])
	_append128asm_internal %1, rax

	; r8 <- array[i++]
	add r8, 256 * 16

%assign k k+1
%endrep

;%assign m m+1
;%endrep
	dec cl
	jnz %%cycle

%endmacro

external _sum128asm, 2

; 1 - x
; 2 - y
; 3 - intermediate register 0
; 4 - intermediate register 1
; 5 - intermediate register 2
; 6 - jmp address if zero
; uses: rax, rdx
; should be used only if one of the args is register
; don't work if index_of(x) == 255 && index_of(y) == 255
; that never happens in current configuration
%rmacro _galois_mul_asm_optimized_proto 5-6
%if !USE_MUL_TABLE

%xdefine x %1
%xdefine y %2
%xdefine inter0 %3
%xdefine inter1 %4
%xdefine inter2 %5

%if !IS_REGISTER(%[x]) && IS_REGISTER(%[y])
%error "_galois_mul_asm_optimized_internal should be called with at least one register" 
%endif

%if IS_REGISTER(%[x])
	test %[x], %[x]
%if %0 = 5
	je %%zero
%else
	je %6
%endif
%else
%if %[x] = 0
%if %0 = 5
	jmp %%zero
%else
	jmp %6
%endif
%endif
%endif

%if IS_REGISTER(%[y])
	test %[y], %[y]
%if %0 = 5
	je %%zero
%else
	je %6
%endif
%else
%if %[y] = 0
%if %0 = 5
	jmp %%zero
%else
	jmp %6
%endif
%endif
%endif

	lea %[inter0], [rel index_of]
%if IS_REGISTER(%[x]) && IS_REGISTER(%[y])
	movzx rax, byte [%[inter0] + %[x]]
	movzx %[inter2], byte [%[inter0] + %[y]]
	%xdefine second REGISTER_PART_%[inter2]_8
	;add rax, rdx
%elif IS_REGISTER(%[x])
	movzx rax, byte [%[inter0] + %[x]]
	%xdefine second INDEX_OF_%[y]
	;add rax, INDEX_OF_%2
%elif IS_REGISTER(%[y])
	movzx rax, byte [%[inter0] + %[y]]
	%xdefine second INDEX_OF_%[x]
	;add rax, INDEX_OF_%1
%endif


%if 0
; possible implementation:
	add al, 1
	add al, %[second]
	cmp al, %[second]
	jbe %%continue
	sub rax, 1
%%continue:
%endif

	xor %[inter0], %[inter0]
%if IS_REGISTER(%[second])
	; INDEX_OF never go 255 so we can safely
	; add 1 without overflow and use the same
	; modulo 255 algo with both registers and
	; register-const versions
	add al, 1
	add al, %[second]
%else
	add al, %[second] + 1
%endif
	cmc
	adc %[inter0], 0
	sub rax, %[inter0]

	lea %[inter1], [rel alpha_to]
	movzx rax, byte [%[inter1] + rax]

%if %0 = 5
	jmp %%result
%%zero:
	mov rax, 0
%endif
%%result:

%endif
%endmacro

global _galois_mul_asm_optimized

%rmacro _galois_mul_asm_optimized_internal 2-3
%if %0 = 2
	_galois_mul_asm_optimized_proto %1, %2, rdx, rdx, rdx
%elif %0 = 3
	_galois_mul_asm_optimized_proto %1, %2, rdx, rdx, rdx, %3
%endif
%endmacro

external _galois_mul_asm_optimized, 2-3

; exists only if USE_MUL_TABLE is set
; 1 - x
; 2 - y
; 3 - intermediate register
; 4 - result register
; uses: rdx, %2
%rmacro _galois_mul_asm_table_proto 4
%if USE_MUL_TABLE

%xdefine x %1
%xdefine y %2
%xdefine inter %3
%xdefine res %4

%if !IS_REGISTER(%[x]) && IS_REGISTER(%[y])
%error "_galois_mul_asm_internal should be called with at least one register" 
%endif

%if IS_REGISTER(%[x]) && IS_REGISTER(%[y])
	lea %[inter], [rel mul_table]
	add %[inter], %[x]
	shl %[y], 9
	movzx %[res], byte [%[inter] + %[y]]
%elif IS_REGISTER(%[x])
	lea %[inter], [rel mul_table_%[y]]
	movzx %[res], byte [%[inter] + %[x]]
%elif IS_REGISTER(%[y])
	lea %[inter], [rel mul_table_%[x]]
	movzx %[res], byte [%[inter] + %[y]]
%endif

%endif
%endmacro

global _galois_mul_asm_table

%rmacro _galois_mul_asm_table_internal 2
	_galois_mul_asm_table_proto %1, %2, rdx, rax
%endmacro

external _galois_mul_asm_table, 2

; 1 - pointer of w
; 2 - type
; uses: rax, rcx, rdx, r8, r9
%rmacro _kuz_l_asm_proto 2

	mov r8, %1

	mov rcx, 16
%%cycle:

;%assign i 0
;%rep 16

	movzx r9, byte [r8 + 15]

; DON't optimize that!
%assign k 14
%rep 15

	movzx rax, byte [r8 + k]
	mov [r8 + k + 1], al

%xdefine source KUZ_LVEC_%[k]
%if %2 = OPTIMIZED
	; use: rax, rdx
	_galois_mul_asm_optimized_internal rax, source, %%continue_from_zero_%[k]
	; rax is already zero, so ok
%%continue_from_zero_%[k]:
%elif %2 = MUL_TABLE
	; use: rax, rdx
	_galois_mul_asm_table_internal rax, source
%endif
	xor r9, rax

%assign k k-1
%endrep

	mov [r8 + 0], r9b

	dec rcx
	jnz %%cycle

;%assign i i+1
;%endrep

%endmacro

global _kuz_l_asm

; 1 - pointer of w
; uses: rax, rdx, r8, r9
%rmacro _kuz_l_asm_internal 1
	_kuz_l_asm_proto %1, OPTIMIZED
%endmacro

external _kuz_l_asm, 1

global _kuz_l_asm_table

; 1 - pointer of w
; uses: rax, rdx, r8, r9
%rmacro _kuz_l_asm_table_internal 1
	_kuz_l_asm_proto %1, MUL_TABLE
%endmacro

external _kuz_l_asm_table, 1

; 1 - pointer of w
; 2 - type
; uses: rax, rcx, rdx, r8, r9
%rmacro _kuz_l_inv_asm_proto 2

	mov r8, %1

	mov rcx, 16
%%cycle:
;%assign i 0
;%rep 16

	movzx r9, byte [r8 + 0]

%assign k 0
%rep 15

	movzx rax, byte [r8 + k + 1]
	mov [r8 + k], al

%if %2 = OPTIMIZED
	; use: rax, rdx
	_galois_mul_asm_optimized_internal rax, KUZ_LVEC_%[k], %%continue_from_zero_%[k]
	; rax is zero
%%continue_from_zero_%[k]:
%elif %2 = MUL_TABLE
	; use: rax, rdx
	_galois_mul_asm_table_internal rax, KUZ_LVEC_%[k]
%endif
	xor r9, rax

%assign k k+1
%endrep

	mov byte [r8 + 15], r9b

	dec rcx
	jnz %%cycle

;%assign i i+1
;%endrep

%endmacro

global _kuz_l_inv_asm

; 1 - pointer of w
; uses: rax, rdx, r8, r9
%rmacro _kuz_l_inv_asm_internal 1
	_kuz_l_inv_asm_proto %1, OPTIMIZED
%endmacro

external _kuz_l_inv_asm, 1

global _kuz_l_inv_asm_table

; 1 - pointer of w
; uses: rax, rdx, r8, r9
%rmacro _kuz_l_inv_asm_table_internal 1
	_kuz_l_inv_asm_proto %1, MUL_TABLE
%endmacro

external _kuz_l_inv_asm_table, 1

; 1 - buffer result
; 2 - pointer to key
; 3 - pointer to x
; 4 - type
%rmacro _kuz_encrypt_block_asm_proto 4

	push r12
;	push r13

	mov r10, %2

	lea r11, [rel kuz_pi]

	mov r12, %3

;	mov r13, 9
;%%cycle:

%assign m 0
%rep 9

	;lea r10, [%2 + 16]

	; use: rdx
	;_append128asm_internal rcx, r10
	; use: rax
	;_convert128asm_internal rcx, r11

	; 1 - pointer of source & result
	; 2 - used register
	; 3 - used register 2
	; 4 - used register 3
	; 5 - type size in bits
	; 6 - first operation
	; 7 - first operation arg
	; 8 - second operation
	; 9 - second operation arg
	; use: rax, rdx, r8
	_double_op r12, r12, a, d, 8, BITS, append128_operation, r10, convert128_operation, r11

%if %4 = OPTIMIZED
	; use: rax, rcx, rdx, r8, r9
	_kuz_l_asm_internal r12
%elif %4 = MUL_TABLE
	; use: rax, rcx, rdx, r8, r9
	_kuz_l_asm_table_internal r12
%endif

	; key->k[i++]
	add r10, 16

;	dec r13
;	jnz %%cycle

%assign m m+1
%endrep

	;mov r10, %2
	;add r10, 9 * 16
	;lea r10, [%2 + 9 * 16]

	_append128asm_internal r12, r10

;	pop r13
	pop r12

%endmacro

global _kuz_encrypt_block_asm

; 1 - buffer result
; 2 - pointer to key
; 3 - pointer to x
%rmacro _kuz_encrypt_block_asm_internal 3
	_kuz_encrypt_block_asm_proto %1, %2, %3, OPTIMIZED
%endmacro

external _kuz_encrypt_block_asm, 3

global _kuz_encrypt_block_asm_mul_table

; 1 - buffer result
; 2 - pointer to key
; 3 - pointer to x
%rmacro _kuz_encrypt_block_asm_mul_table_internal 3
	_kuz_encrypt_block_asm_proto %1, %2, %3, MUL_TABLE
%endmacro

external _kuz_encrypt_block_asm_mul_table, 3

; exists only if USE_TABLES is set
; 1 - buffer result
; 2 - pointer to key
; 3 - pointer to x
%rmacro _kuz_encrypt_block_asm_tables_proto 3
%if USE_TABLES

;	push r13

	mov r10, %2

	mov r11, %3

;	mov r13, 9
;%%cycle:
%assign m 0
%rep 9

	; use: rdx
	_append128asm_internal r11, r10
	;_append128asm_internal r11, ENCRYPT_KEY_%[m]

	; use: rax, rcx, rdx, r8, r9
	_append128multi_asm_internal %1, r11, kuz_pil_enc128

;%if m % 2 == 0

	; use: rdx
;	_append128asm_internal r11, r10

	; use: rax, rcx, rdx, r8, r9
	;_append128multi_asm_internal %1, r11, kuz_pil_enc128

	; use: rax, r8, r9, rdx
;	_plus128multi_asm_internal %1, r11, kuz_pil_enc128
;%else

	; use: rdx
;	_append128asm_internal %1, r10

	; use: rax, r8, r9, rdx
;	_plus128multi_asm_internal r11, %1, kuz_pil_enc128

;%endif

;%%end:
	; key->k[i++]
	add r10, 16

;	dec r13
;	jnz %%cycle
%assign m m+1
%endrep

	;_copy128asm_internal r11, %1

	;mov r10, %2
	;add r10, 9 * 16

	; use: rdx
	;_append128asm_internal r11, r10
	;_append128asm_internal r11, ENCRYPT_KEY_9
	_append128asm_internal r11, r10

	;pop r13

%endif
%endmacro

global _kuz_encrypt_block_asm_tables

%rmacro _kuz_encrypt_block_asm_tables_internal 3
	_kuz_encrypt_block_asm_tables_proto %1, %2, %3
%endmacro

external _kuz_encrypt_block_asm_tables, 3

; 1 - pointer to key
; 2 - pointer to x
; 3 - type
%rmacro _kuz_decrypt_block_asm_proto 3

	push r12
	;push r13

	mov r11, %2

	lea r12, [rel kuz_pi_inv]

	;mov r10, %1
	;add r10, 9 * 16
	lea r10, [%1 + 9 * 16]

	; use: rdx
	_append128asm_internal r11, r10

	;mov r10, %1
	;add r10, 8 * 16
	sub r10, 16

;	mov r13, 9
;%%cycle:

%assign m 8
%rep 9

%if %3 = OPTIMIZED
	; use: rax, rcx, rdx, r8, r9
	_kuz_l_inv_asm_internal r11
%elif %3 = MUL_TABLE
	; use: rax, rcx, rdx, r8, r9
	_kuz_l_inv_asm_table_internal r11
%endif

	; use: rax
	;_convert128asm_internal r11, rcx
	; use: rdx
	;_append128asm_internal r11, r10

	; 1 - pointer of source & result
	; 2 - used register
	; 3 - used register 2
	; 4 - used register 3
	; 5 - type size in bits
	; 6 - first operation
	; 7 - first operation arg
	; 8 - second operation
	; 9 - second operation arg
	; use: rax, rdx, r8
	_double_op r11, r11, a, d, 8, %[BITS], convert128_operation, r12, append128_operation, r10

	; key->k[i--]
	sub r10, 16

;	dec r13
;	jnz %%cycle

%assign m m-1
%endrep

	;pop r13
	pop r12

%endmacro

global _kuz_decrypt_block_asm

; 1 - pointer to key
; 2 - pointer to x
%rmacro _kuz_decrypt_block_asm_internal 2
	_kuz_decrypt_block_asm_proto %1, %2, OPTIMIZED
%endmacro

external _kuz_decrypt_block_asm, 2

global _kuz_decrypt_block_asm_mul_table

; 1 - pointer to key
; 2 - pointer to x
%rmacro _kuz_decrypt_block_asm_mul_table_internal 2
	_kuz_decrypt_block_asm_proto %1, %2, MUL_TABLE
%endmacro

external _kuz_decrypt_block_asm_mul_table, 2

; exists only if USE_TABLES is set
; 1 - result buffer
; 2 - pointer to key
; 3 - pointer to x
%rmacro _kuz_decrypt_block_asm_tables_proto 3
%if USE_TABLES

	push r12
	push r13

	mov r11, %3

	lea r13, [rel kuz_pi_inv]

	; use: rax, rcx, rdx, r8
	_append128multi_asm_internal %1, r11, kuz_l_dec128

	; use: rax, r8, r9, rdx
	;_plus128multi_asm_internal %1, r11, kuz_l_dec128

	;mov r10, %2
	;add r10, 9 * 16
	lea r10, [%2 + 9 * 16]

	mov r12, 8
%%cycle:
;%assign m 9
;%rep 8

	; use: rdx
	_append128asm_internal r11, r10

	; use: rax, r8, r9, rdx
	_append128multi_asm_internal %1, r11, kuz_pil_dec128

;%if m % 2 == 0
	; use: rdx
;	_append128asm_internal r11, r10

	; use: rax, r8, r9, rdx
	;_append128multi_asm_internal %1, r11, kuz_pil_dec128

	; use: rax, r8, r9, rdx
;	_plus128multi_asm_internal %1, r11, kuz_pil_dec128
;%else
	; use: rdx
;	_append128asm_internal %1, r10

	; use: rax, r8, r9, rdx
;	_plus128multi_asm_internal r11, %1, kuz_pil_dec128
;%endif

	; key->k[i--]
	sub r10, 16

	dec r12
	jnz %%cycle

;%assign m m-1
;%endrep

	;_copy128asm_internal r11, %1

	;mov r10, %2
	;add r10, 16
	;sub r10, 16
	; use: rdx
	;_append128asm_internal r11, r10

	; use: rax
	;_convert128asm_internal r11, r13

	; 1 - pointer of source & result
	; 2 - used register
	; 3 - used register 2
	; 4 - used register 3
	; 5 - type size in bits
	; 6 - first operation
	; 7 - first operation arg
	; 8 - second operation
	; 9 - second operation arg
	; use: rax, rdx, r8
	_double_op r11, r11, a, d, 8, %[BITS], append128_operation, r10, convert128_operation, r13

	;mov r10, %2
	sub r10, 16
	; use: rdx
	_append128asm_internal r11, r10

	pop r13
	pop r12

%endif
%endmacro

global _kuz_decrypt_block_asm_tables

%rmacro _kuz_decrypt_block_asm_tables_internal 3
	_kuz_decrypt_block_asm_tables_proto %1, %2, %3
%endmacro

external _kuz_decrypt_block_asm_tables, 3

global _get_tables_size_asm

%rmacro _get_tables_size_asm_internal 1
%if(USE_MUL_TABLE)
	mov qword [%1], 64 * 1024
%elif(USE_TABLES)
	mov qword [%1], (64 + 128) * 1024
%else
	mov qword [%1], 512
%endif
%endmacro

external _get_tables_size_asm, 1

global _get_encrypt_tables_size_asm

%rmacro _get_encrypt_tables_size_asm_internal 1
%if(USE_MUL_TABLE)
	mov qword [%1], 64 * 1024
%elif(USE_TABLES)
	mov qword [%1], 64 * 1024
%else
	mov qword [%1], 512
%endif
%endmacro

external _get_encrypt_tables_size_asm, 1

global _get_decrypt_tables_size_asm

%rmacro _get_decrypt_tables_size_asm_internal 1
%if(USE_MUL_TABLE)
	mov qword [%1], 64 * 1024
%elif(USE_TABLES)
	mov qword [%1], 128 * 1024
%else
	mov qword [%1], 512
%endif
%endmacro

external _get_decrypt_tables_size_asm, 1












