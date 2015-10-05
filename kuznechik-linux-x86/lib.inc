
%define ARG0 rdi
%define ARG1 rsi
%define ARG2 rdx
%define ARG3 rcx
%define ARG4 r8
%define ARG5 r9

%define IS_REGISTER(var) (var = al || var = bl || var = cl || var = dl || var = ax || \
	var = bx || var = cx || var = dx || var = si || var = di || var = eax || var = ebx || \
	var = ecx || var = edx || var = esi || var = edi || var = rax || var = rbx || var = rdx || \
	var = rsi || var = rdi || var = r8b || var = r9b || var = r10b || var = r11b || var = r12b || \
	var = r13b || var = r14b || var = r15b || var = r8w || var = r9w || var = r10w || var = r11w || \
	var = r12w || var = r13w || var = r14w || var = r15w || var = r8d || var = r9w || var = r10w || \
	var = r11w || var = r12w || var = r13w || var = r14w || var = r15w || var = r8 || var = r9 || \
	var = r10 || var = r11 || var = r12 || var = r13 || var = r14 || var = r15)

%define TYPE_8 byte
%define TYPE_16 word
%define TYPE_32 dword
%define TYPE_64 qword

%define ACCESS_ARRAY(base, index, size) ([base + index * (size / 8)])

%macro general_register_parts 1
%define REGISTER_PART_%1_8 %1l
%define REGISTER_PART_%1_16 %1x
%define REGISTER_PART_%1_32 e%1x
%define REGISTER_PART_%1_64 r%1x

%define REGISTER_PART_r%1x_8 %1l
%define REGISTER_PART_r%1x_16 %1x
%define REGISTER_PART_r%1x_32 e%1x
%define REGISTER_PART_r%1x_64 r%1x
%endmacro

%macro index_register_parts 1
%define REGISTER_PART_%1_16 %1i
%define REGISTER_PART_%1_32 e%1i
%define REGISTER_PART_%1_64 r%1i

%define REGISTER_PART_r%1i_16 %1i
%define REGISTER_PART_r%1i_32 e%1i
%define REGISTER_PART_r%1i_64 r%1i
%endmacro

%macro expanded_register_parts 1
%define REGISTER_PART_%1_8 r%1b
%define REGISTER_PART_%1_16 r%1w
%define REGISTER_PART_%1_32 r%1d
%define REGISTER_PART_%1_64 r%1

%define REGISTER_PART_r%1_8 r%1b
%define REGISTER_PART_r%1_16 r%1w
%define REGISTER_PART_r%1_32 r%1d
%define REGISTER_PART_r%1_64 r%1
%endmacro

general_register_parts a
general_register_parts b
general_register_parts c
general_register_parts d

index_register_parts r
index_register_parts s

expanded_register_parts 8
expanded_register_parts 9
expanded_register_parts 10
expanded_register_parts 11
expanded_register_parts 12
expanded_register_parts 13
expanded_register_parts 14
expanded_register_parts 15

; FREE TO USE REGISTERS:
; rax (RES), rcx (ARG3), rdx (ARG2), rsi (ARG1), rdi (ARG0), r8 (ARG4), r9 (ARG5), r10, r11

%rmacro internal_call 1-2
%if %0 = 1
	%1_internal
%else
%if %2 = 1
	%1_internal ARG0
%endif
%if %2 = 2
	%1_internal ARG0, ARG1
%endif
%if %2 = 3
	%1_internal ARG0, ARG1, ARG2
%endif
%if %2 = 4
	%1_internal ARG0, ARG1, ARG2, ARG3
%endif
%if %2 = 5
	%1_internal ARG0, ARG1, ARG2, ARG3, ARG4
%endif
%if %2 = 6
	%1_internal ARG0, ARG1, ARG2, ARG3, ARG4, ARG5
%endif
%endif
%endmacro

%rmacro external 1-2
%1:
	internal_call %{1:-1}
	retn
%endmacro

; concat value by bytes into one
; 1 - array
; 2 - offset
; 3 - bits
%rmacro gather_def_value 3
%define gather_array %1
%assign gather_offset %2
%assign gather_bits %3
%assign gather_value 0
%assign gather_count %[gather_bits] / 8
%assign num 0
%rep %[gather_count]
%assign new_gather_index %[gather_count]*gather_offset + num
%assign new_gather_value %[gather_array]_%[new_gather_index]
%assign gather_value (%[gather_value] << 8) | %[new_gather_value]
;%assign gather_value (%[new_gather_value] << (8*num)) | %[gather_value]
%assign num num+1
%endrep
%endmacro


