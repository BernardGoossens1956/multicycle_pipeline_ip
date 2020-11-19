#include "debug_multicycle_pipeline_ip.h"
#include "multicycle_pipeline_ip.h"
#include "type.h"
#include "immediate.h"
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
#include <stdio.h>
#include "disassemble.h"
#endif
#endif

static void decode_instruction(
  instruction_t instruction,
  decoded_instruction_t *decoded_instruction){
#pragma HLS INLINE
  decoded_instruction->inst_31    = (instruction >> 31);
  decoded_instruction->inst_30_25 = (instruction >> 25);
  decoded_instruction->inst_24_21 = (instruction >> 21);
  decoded_instruction->inst_20    = (instruction >> 20);
  decoded_instruction->inst_19_12 = (instruction >> 12);
  decoded_instruction->inst_11_8  = (instruction >>  8);
  decoded_instruction->inst_7     = (instruction >>  7);
  decoded_instruction->inst_6_2   = (instruction >>  2);
  decoded_instruction->opcode     = (instruction >>  2);
  decoded_instruction->inst_1_0   = (instruction      );
  decoded_instruction->rd         = (instruction >>  7);
  decoded_instruction->func3      = (instruction >> 12);
  decoded_instruction->rs1        = (instruction >> 15);
  decoded_instruction->rs2        = (instruction >> 20);
  decoded_instruction->func7      = (instruction >> 25);
  decoded_instruction->is_rs1_reg =
                  (decoded_instruction->opcode != JAL   &&
                   decoded_instruction->opcode != LUI   &&
                   decoded_instruction->opcode != AUIPC &&
                   decoded_instruction->rs1    != 0);
  decoded_instruction->is_rs2_reg =
                  (decoded_instruction->opcode != OP_IMM &&
                   decoded_instruction->opcode != LOAD   &&
                   decoded_instruction->opcode != JAL    &&
                   decoded_instruction->opcode != JALR   &&
                   decoded_instruction->opcode != LUI    &&                   
                   decoded_instruction->opcode != AUIPC  &&
                   decoded_instruction->rs2    != 0);
  decoded_instruction->is_load    =
                  (decoded_instruction->opcode == LOAD);
  decoded_instruction->is_store   =
                  (decoded_instruction->opcode == STORE);
  decoded_instruction->is_branch  =
                  (decoded_instruction->opcode == BRANCH);
  decoded_instruction->is_jal     =
                  (decoded_instruction->opcode == JAL);
  decoded_instruction->is_jalr    =
                  (decoded_instruction->opcode == JALR);
  decoded_instruction->is_ret     = (instruction == RET);
  decoded_instruction->is_lui     =
                  (decoded_instruction->opcode == LUI);
  decoded_instruction->is_op_imm  =
                  (decoded_instruction->opcode == OP_IMM);
  decoded_instruction->is_system  =
                  (decoded_instruction->opcode == SYSTEM);
  decoded_instruction->is_jump    =
                  (decoded_instruction->is_jal ||
                   decoded_instruction->is_jalr);
  decoded_instruction->is_ctrl    =
                  (decoded_instruction->is_branch ||
                   decoded_instruction->is_jump);
  decoded_instruction->has_no_dest=
                  (decoded_instruction->is_branch ||
                   decoded_instruction->is_store  ||
                  (decoded_instruction->rd == 0));
  decoded_instruction->opch       =
                   decoded_instruction->opcode>>3;
  decoded_instruction->opcl       =
                   decoded_instruction->opcode;
  decoded_instruction->type       =
              type(decoded_instruction->opch,
                   decoded_instruction->opcl);
  switch(decoded_instruction->type){
    case UNDEFINED_TYPE: decoded_instruction->imm = 0;
                         break;
    case R_TYPE:         decoded_instruction->imm = 0;
                         break;
    case I_TYPE:         decoded_instruction->imm =
                           i_immediate(decoded_instruction);
                         break;
    case S_TYPE:         decoded_instruction->imm =
                           s_immediate(decoded_instruction);
                         break;
    case B_TYPE:         decoded_instruction->imm =
                           b_immediate(decoded_instruction);
                         break;
    case U_TYPE:         decoded_instruction->imm =
                           u_immediate(decoded_instruction);
                         break;
    case J_TYPE:         decoded_instruction->imm =
                           j_immediate(decoded_instruction);
                         break;
    case OTHER_TYPE:     decoded_instruction->imm = 0;
                         break;
  }
  decoded_instruction->is_r_type  =
                  (decoded_instruction->type   == R_TYPE);
}
void init_decode_status(decode_status_t *decode_status){
  decode_status->is_full     = 0;
}
static void get_input(
  decode_in_from_fetch_t decode_in_from_fetch,
  decode_status_t       *decode_status){
#pragma HLS INLINE
  decode_status->pc          =
    decode_in_from_fetch.pc;
  decode_status->instruction =
    decode_in_from_fetch.instruction;
}
static void stage_job(
  decode_status_t        decode_status,
  decoded_instruction_t *decoded_instruction,
  code_address_t        *target_pc){
#pragma HLS INLINE
  decode_instruction(decode_status.instruction,
                     decoded_instruction);
  *target_pc = decode_status.pc +
    ((decoded_instruction->is_jal)   ?
     (decoded_instruction->imm << 1) :
      4);
}
static void set_output(
  decoded_instruction_t  decoded_instruction,
  code_address_t         target_pc,
  decode_status_t        decode_status,
  decode_out_to_fetch_t *decode_out_to_fetch,
  decode_out_to_issue_t *decode_out_to_issue){
#pragma HLS INLINE
  decode_out_to_fetch->target_pc           =
    target_pc;
  decode_out_to_issue->pc                  =
    decode_status.pc;
  decode_out_to_issue->decoded_instruction =
    decoded_instruction;
  if (decoded_instruction.has_no_dest)
    decode_out_to_issue->decoded_instruction.rd = 0;
#ifndef __SYNTHESIS__
  decode_out_to_issue->instruction         =
    decode_status.instruction;
  decode_out_to_issue->target_pc           =
    target_pc;
#endif
}
void set_bits(
  bit_t  issue_status_is_full,
  bit_t  decode_in_from_fetch_is_valid,
  bit_t *decode_status_is_full,
  bit_t *is_valid,
  bit_t *save_input,
  bit_t *decode_out_to_issue_is_valid){
#pragma HLS INLINE
  *is_valid =
    (decode_in_from_fetch_is_valid ||
    *decode_status_is_full);
  *save_input =
     decode_in_from_fetch_is_valid &&
  !(*decode_status_is_full);
  *decode_out_to_issue_is_valid =
    !issue_status_is_full && *is_valid;
  *decode_status_is_full =
     issue_status_is_full && *is_valid;
}
void decode(
  decode_in_from_fetch_t decode_in_from_fetch,
  bit_t                  issue_status_is_full,
  decode_status_t       *decode_status,
  decode_out_to_fetch_t *decode_out_to_fetch,
  decode_out_to_issue_t *decode_out_to_issue,
  bit_t                 *decode_status_is_full){
#pragma HLS INLINE off
  bit_t                 save_input;
  bit_t                 wait_for_target_address;
  bit_t                 is_valid;
  decoded_instruction_t decoded_instruction;
  code_address_t        target_pc;
  set_bits(issue_status_is_full,
           decode_in_from_fetch.is_valid,
          &(decode_status->is_full),
          &is_valid,
          &save_input,
          &(decode_out_to_issue->is_valid));
  if (save_input)
    get_input(decode_in_from_fetch, decode_status);
  if (decode_out_to_issue->is_valid){
    stage_job(*decode_status, &decoded_instruction, &target_pc);
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
    printf("decoded  %04d: ", decode_status->pc);
    disassemble(decode_status->pc,
                decode_status->instruction,
                decoded_instruction);
#endif
#endif
    set_output(decoded_instruction,
               target_pc,
              *decode_status,
               decode_out_to_fetch,
               decode_out_to_issue);
  }
  *decode_status_is_full = decode_status->is_full;
  wait_for_target_address       =
     decode_out_to_issue->is_valid &&
    (decoded_instruction.is_branch ||
     decoded_instruction.is_jalr);
  decode_out_to_fetch->is_valid =
    decode_out_to_issue->is_valid &&
   !wait_for_target_address;
}
