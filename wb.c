#include "debug_multicycle_pipeline_ip.h"
#include "multicycle_pipeline_ip.h"
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
#include "stdio.h"
#endif
#endif

static void build_word(
  unsigned char b0,
  unsigned char b1,
  unsigned char b2,
  unsigned char b3,
  uint2         a01,
  bit_t         a1,
  func3_t       msize,
  int          *result){
  char           b;
  unsigned char  ub;
  short          h, h0, h1;
  unsigned short uh, uh0, uh1;
  int            w, ib, ih;
  unsigned int   iub, iuh;
  h0  = ((unsigned short)b1 <<  8) | (unsigned short)b0;
  uh0 = h0;
  h1  = ((unsigned short)b3 <<  8) | (unsigned short)b2;
  uh1 = h1;
  switch(a01){
    case 0b00: b = b0; break;
    case 0b01: b = b1; break;
    case 0b10: b = b2; break;
    case 0b11: b = b3; break;
  }
  ub  = b;
  ib  = b;
  iub = ub;
  h   = (a1)?h1:h0;
  uh  = h;
  ih  = h;
  iuh = uh;
  w = (uh1 << 16) | uh0;
  switch(msize){
    case LB:
      *result = ib;  break;
    case LH:
      *result = ih;  break;
    case LW:
      *result = w;   break;
    case 3:
      *result = 0;   break;
    case LBU:
      *result = iub; break;
    case LHU:
      *result = iuh; break;
    case 6:
    case 7:
      *result = 0;   break;
  }
}
static void get_input(
  wb_in_from_execute_2_t wb_in_from_execute_2,
  wb_in_from_mem_t       wb_in_from_mem,
  wb_status_t           *wb_status){
#pragma HLS INLINE
  int result;
  if (wb_in_from_mem.is_valid){
    build_word(wb_in_from_mem.value_0,
               wb_in_from_mem.value_1,
               wb_in_from_mem.value_2,
               wb_in_from_mem.value_3,
               wb_in_from_mem.a01,
               wb_in_from_mem.a1,
               wb_in_from_mem.msize,
              &result);
    wb_status->value               = result;
    wb_status->rd                  =
      wb_in_from_mem.rd;
    wb_status->has_no_dest         =
      wb_in_from_mem.has_no_dest;
    wb_status->is_ret              = 0;
#ifndef __SYNTHESIS__
    wb_status->pc                  =
      wb_in_from_mem.pc;
    wb_status->instruction         =
      wb_in_from_mem.instruction;
    wb_status->decoded_instruction =
      wb_in_from_mem.decoded_instruction;
#endif
  }
  else{ //if (wb_in_from_execute_2.is_valid){
    wb_status->value               =
      wb_in_from_execute_2.result;
    wb_status->rd                  =
      wb_in_from_execute_2.rd;
    wb_status->has_no_dest         =
      wb_in_from_execute_2.has_no_dest;
    wb_status->is_ret              =
      wb_in_from_execute_2.is_ret;
#ifndef __SYNTHESIS__
    wb_status->pc                  =
      wb_in_from_execute_2.pc;
    wb_status->instruction         =
      wb_in_from_execute_2.instruction;
    wb_status->decoded_instruction =
      wb_in_from_execute_2.decoded_instruction;
    wb_status->target_pc           =
      wb_in_from_execute_2.target_pc;
#endif
  }
}
static void stage_job(
  wb_status_t *wb_status,
  int         *reg_file){
#pragma HLS INLINE
  if (!wb_status->has_no_dest){
    reg_file[wb_status->rd] = wb_status->value;
  }
}
void write_back(
  wb_in_from_execute_2_t wb_in_from_execute_2,
  wb_in_from_mem_t       wb_in_from_mem,
  int                   *reg_file,
  reg_num_t             *wb_destination,
  wb_status_t           *wb_status){
#pragma HLS INLINE off
  bit_t     save_input;
  reg_num_t dest;
  dest = (wb_in_from_mem.is_valid)?
          wb_in_from_mem.rd       :
          wb_in_from_execute_2.rd;
  if (wb_in_from_execute_2.is_valid ||
      wb_in_from_mem.is_valid)
    *wb_destination = dest;
  else
    *wb_destination = 0;
  save_input =
    (wb_in_from_execute_2.is_valid ||
     wb_in_from_mem.is_valid);
  if (save_input){
    get_input(wb_in_from_execute_2, wb_in_from_mem, wb_status);
    stage_job(wb_status, reg_file);
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
    printf("wb       ");
    printf("%04d\n", wb_status->pc);
    if (!wb_status->decoded_instruction.is_branch &&
        !wb_status->decoded_instruction.is_jalr)
      emulate(reg_file,
              wb_status->decoded_instruction,
              wb_status->target_pc);
#else
#ifdef DEBUG_FETCH
    printf("%04d: %08x      ",
      (unsigned int)wb_status->pc,
      (unsigned int)wb_status->instruction);
#ifndef DEBUG_DISASSEMBLE
    printf("\n");
#endif
#endif
#ifdef DEBUG_DISASSEMBLE
    disassemble(wb_status->pc,
                wb_status->instruction,
                wb_status->decoded_instruction);
#endif
#ifdef DEBUG_EMULATE
    emulate(reg_file,
            wb_status->decoded_instruction,
            wb_status->target_pc);
#endif
#endif
#endif
  }
}
