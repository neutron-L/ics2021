def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc) {
  rtl_addi(s, ddest, &(s->pc), id_src1->imm);
}


def_EHelper(jal) {
  rtl_addi(s, ddest, &(s->pc), 4);
  rtl_addi(s, &(s->dnpc), &(s->pc), id_src1->imm);
}

// immediate arithmetric
def_EHelper(addi) {
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(sltiu) {
  // get rs1 `s` bit
  rtl_msb(s, s0, dsrc1, 4);
  // get imm `s` bit
  rtl_li(s, s1, id_src2->imm);
  rtl_msb(s, s1, s1, 4);

  if (*s0 != *s1)
  {
    rtl_mv(s, ddest, s1);
  }
  else
  {
    rtl_subi(s, s0, dsrc1, id_src2->imm);
    rtl_msb(s, s0, s0, 4);
    rtl_mv(s, ddest, s0);
  }
}

def_EHelper(slli) {
  rtl_slli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srai) {
  rtl_srai(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srli) {
  rtl_srli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(jalr) {
  rtl_addi(s, ddest, &(s->pc), 4);
  rtl_addi(s, &(s->dnpc), dsrc1, id_src2->imm);
}

def_EHelper(andi) {
  rtl_andi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(xori) {
  rtl_xori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(ori) {
  rtl_ori(s, ddest, dsrc1, id_src2->imm);
}

// registers
def_EHelper(add) {
  rtl_add(s, ddest, dsrc1, dsrc2);
}


def_EHelper(sub) {
  rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mul) {
  rtl_mulu_lo(s, ddest, dsrc1, dsrc2);
}


def_EHelper(mulh) {
  rtl_muls_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulhu) {
  rtl_mulu_hi(s, ddest, dsrc1, dsrc2);
}


def_EHelper(rem) {
  rtl_divs_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(remu) {
  rtl_divu_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(div) {
  rtl_divs_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(divu) {
  rtl_divu_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(xor) {
  rtl_xor(s, ddest, dsrc1, dsrc2);
}


def_EHelper(or) {
  rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sltu) {
  rtl_msb(s, s0, dsrc1, 4);
  rtl_msb(s, s1, dsrc2, 4);
  if (*s0 == *s1)
  {
    rtl_sub(s, s1, dsrc1, dsrc2);
    rtl_msb(s, s1, s1, 4);
  }
  rtl_mv(s, ddest, s1);
}

def_EHelper(slt) {
  rtl_sub(s, s0, dsrc1, dsrc2);
  rtl_msb(s, s0, s0, 4);
  rtl_mv(s, ddest, s0);
}

def_EHelper(sll) {
  rtl_sll(s, ddest, dsrc1, dsrc2);
}


def_EHelper(srl) {
  rtl_srl(s, ddest, dsrc1, dsrc2);
}


def_EHelper(sra) {
  rtl_sra(s, ddest, dsrc1, dsrc2);
}

def_EHelper(and) {
  rtl_and(s, ddest, dsrc1, dsrc2);
}

// condition
def_EHelper(beq) {
  rtl_sub(s, s0, dsrc1, dsrc2);
  if (*s0 == 0)
  {
    rtl_addi(s, &(s->dnpc), &(s->pc), id_dest->imm);
  }
}

def_EHelper(bne) {
  rtl_sub(s, s0, dsrc1, dsrc2);
  if (*s0 != 0)
  {
    rtl_addi(s, &(s->dnpc), &(s->pc), id_dest->imm);
  }
}

def_EHelper(blt) {
  rtl_sub(s, s0, dsrc1, dsrc2);
  rtl_msb(s, s0, s0, 4);
  if (*s0 == 1)
  {
    rtl_addi(s, &(s->dnpc), &(s->pc), id_dest->imm);
  }
}


def_EHelper(bltu) {
  rtl_msb(s, s0, dsrc1, 4);
  rtl_msb(s, s1, dsrc2, 4);
  if (*s0 == *s1)
  {
    rtl_sub(s, s1, dsrc1, dsrc2);
    rtl_msb(s, s1, s1, 4);
  }
  if (*s1 == 1)
  {
    rtl_addi(s, &(s->dnpc), &(s->pc), id_dest->imm);
  }
}

def_EHelper(bge) {
  // same sign
  rtl_msb(s, s0, dsrc1, 4);
  rtl_msb(s, s1, dsrc2, 4);
  if (*s0 == *s1)
  {
    rtl_sub(s, s0, dsrc1, dsrc2);
    rtl_msb(s, s0, s0, 4);
    // positive
    if (*s0 == 0)
    {
      rtl_addi(s, &(s->dnpc), &(s->pc), id_dest->imm);
    }
  }
  // diff sign
  else
  {
    if (*s1 == 1)
    {
      rtl_addi(s, &(s->dnpc), &(s->pc), id_dest->imm);
    }
  }
}

def_EHelper(bgeu) {
  rtl_sub(s, s0, dsrc1, dsrc2);
  rtl_msb(s, s0, s0, 4);
  // positive
  if (*s0 == 0)
  {
    rtl_addi(s, &(s->dnpc), &(s->pc), id_dest->imm);
  }
}
