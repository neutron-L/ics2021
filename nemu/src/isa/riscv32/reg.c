#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
  int register_num = sizeof(regs) / sizeof(char *);

  for (int i = 0; i < register_num; i++)
  {
    printf("%s\t0x%08x\t%u\n", regs[i], cpu.gpr[i]._32, cpu.gpr[i]._32);
  }

  printf("%s\t0x%08x\t%u\n", "pc", cpu.pc, cpu.pc);
}

word_t isa_reg_str2val(const char *s, bool *success) {
  int i;
  int register_num = sizeof(regs) / sizeof(char *);


  if (!strcmp(s, "0"))
  {
    *success = true;
      return cpu.gpr[0]._32;
  }

  for (i = 1; i < register_num; i++)
  {
    if (!strcmp(regs[i], s))
    {
      *success = true;
      return cpu.gpr[i]._32;
    }
  }

  if (!strcmp(s, "pc"))
  {
    *success = true;
      return cpu.pc;
  }

  
  *success = false;
  printf("Register %s does not exist\n", s);

  return 0;
}
