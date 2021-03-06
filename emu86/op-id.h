// LIB86 - 80x86 library

#pragma once

#include "op-common.h"


// Operation identifiers

// TODO: continuous ID to enable LUT

#define OP_NULL  0xFFFF

#define OP_MOV   0x0001
#define OP_LEA   0x0002
#define OP_LDS   0x0003

#define OP_XCHG  0x0100

#define OP_IN    0x0200
#define OP_OUT   0x0201

#define OP_CALC2 0x0300  // base for 2 variables calculation
#define OP_ADD   0x0300
#define OP_OR    0x0301
#define OP_ADC   0x0302
#define OP_SBB   0x0303
#define OP_AND   0x0304
#define OP_SUB   0x0305
#define OP_XOR   0x0306
#define OP_CMP   0x0307
#define OP_TEST  0x0308

#define OP_NOT   0x0400
#define OP_NEG   0x0401
#define OP_MUL   0x0402
#define OP_IMUL  0x0403
#define OP_DIV   0x0404
#define OP_IDIV  0x0405

#define OP_INC   0x0500
#define OP_DEC   0x0501

#define OP_ROL   0x0600
#define OP_ROR   0x0601
#define OP_RCL   0x0602
#define OP_RCR   0x0603
#define OP_SHL   0x0604
#define OP_SHR   0x0605
#define OP_SAL   0x0606
#define OP_SAR   0x0607

#define OP_PUSH  0x0700
#define OP_POP   0x0701
#define OP_PUSHF 0x0702
#define OP_POPF  0x0703
#define OP_PUSHA 0x0704
#define OP_POPA  0x0705

#define OP_JMP   0x0800
#define OP_JMPF  0x0801
#define OP_CALL  0x0802
#define OP_CALLF 0x0803

#define OP_INT   0x0900
#define OP_INT3  0x0903
#define OP_INTO  0x0904

#define OP_RET   0x0A00
#define OP_RETF  0x0A01
#define OP_IRET  0x0A02

#define OP_JO    0x0B00
#define OP_JNO   0x0B01
#define OP_JB    0x0B02
#define OP_JNB   0x0B03
#define OP_JZ    0x0B04
#define OP_JNZ   0x0B05
#define OP_JNA   0x0B06
#define OP_JA    0x0B07
#define OP_JS    0x0B08
#define OP_JNS   0x0B09
#define OP_JP    0x0B0A
#define OP_JNP   0x0B0B
#define OP_JL    0x0B0C
#define OP_JNL   0x0B0D
#define OP_JNG   0x0B0E
#define OP_JG    0x0B0F

#define OP_REPNZ 0x0C10
#define OP_REPZ  0x0C11

#define OP_MOVSB 0x0C00
#define OP_MOVSW 0x0C01
#define OP_CMPSB 0x0C02
#define OP_CMPSW 0x0C03
#define OP_STOSB 0x0C04
#define OP_STOSW 0x0C05
#define OP_LODSB 0x0C06
#define OP_LODSW 0x0C07
#define OP_SCASB 0x0C08
#define OP_SCASW 0x0C09

#define OP_AAA   0x0D00
#define OP_AAD   0x0D01
#define OP_AAM   0x0D02
#define OP_AAS   0x0D03

#define OP_CBW   0x0E00
#define OP_CWD   0x0E01

#define OP_CLC   0x0F00
#define OP_CLD   0x0F01
#define OP_CLI   0x0F02
#define OP_CMC   0x0F03
#define OP_STC   0x0F04
#define OP_STD   0x0F05
#define OP_STI   0x0F06

#define OP_SAHF  0x1000
#define OP_LAHF  0x1001

#define OP_HLT   0x1100

#define OP_LOOP   0x1200
#define OP_LOOPNZ 0x1201
#define OP_LOOPZ  0x1202

#define OP_SEG   0x1300

// TODO: finalize op id

#define OP_WAIT   0xFF00
#define OP_XLAT   0xFF01
#define OP_ESC    0xFF02
#define OP_DAA    0xFF03
#define OP_DAS    0xFF04
#define OP_JCXZ   0xFF05
#define OP_LES    0xFF07
#define OP_LOCK   0xFF08
