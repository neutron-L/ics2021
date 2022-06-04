#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(lw) f(sw) f(inv) f(nemu_trap) f(addi) f(auipc) \
 f(jal) f(jalr) f(add) f(sub) f(sltiu) f(beq) f(bne) f(slli) f(mul) f(mulh) f(mulhu) f(xor) \
 f(or) f(sltu) f(blt) f(slt) f(andi) f(sh) f(xori) f(sb) f(srai) f(lb) f(lbu) f(sll) f(and) \
 f(rem) f(remu) f(bge) f(bgeu) f(lh) f(lhu) f(bltu) f(div) f(srli) f(sra) f(srl) f(divu) \
 f(ori)
def_all_EXEC_ID();
