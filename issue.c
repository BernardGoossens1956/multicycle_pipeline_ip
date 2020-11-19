#include "debug_multicycle_pipeline_ip.h"
#include "multicycle_pipeline_ip.h"
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
#include "stdio.h"
#endif
#endif

void init_issue_status(issue_status_t *issue_status){
  issue_status->is_full     = 0;
}
static void get_input(
  issue_in_from_decode_t issue_in_from_decode,
  issue_status_t        *issue_status){
#pragma HLS INLINE
  issue_status->decoded_instruction =
    issue_in_from_decode.decoded_instruction;
  issue_status->pc                  =
    issue_in_from_decode.pc;
#ifndef __SYNTHESIS__
  issue_status->instruction         =
    issue_in_from_decode.instruction;
  issue_status->target_pc           =
    issue_in_from_decode.target_pc;
#endif
}
static void stage_job(
  issue_status_t  issue_status,
  int            *reg_file,
  int            *rv1,
  int            *rv2){
#pragma HLS INLINE
  *rv1 = reg_file[issue_status.decoded_instruction.rs1];
  *rv2 = reg_file[issue_status.decoded_instruction.rs2];
}
static void set_output(
  int                       rv1,
  int                       rv2,
  issue_status_t            issue_status,
  issue_out_to_execute_1_t *issue_out_to_execute_1){
#pragma HLS INLINE
  issue_out_to_execute_1->rv1                 = rv1;
  issue_out_to_execute_1->rv2                 = rv2;
  issue_out_to_execute_1->decoded_instruction =
    issue_status.decoded_instruction;
  issue_out_to_execute_1->pc                  =
    issue_status.pc;
#ifndef __SYNTHESIS__
  issue_out_to_execute_1->instruction         =
    issue_status.instruction;
  issue_out_to_execute_1->target_pc           =
    issue_status.target_pc;
#endif
}
void issue(
  issue_in_from_decode_t    issue_in_from_decode,
  bit_t                     execute_1_status_is_full,
  reg_num_t                *issue_destination,
  int                      *reg_file,
  bit_t                    *is_reg_computed,
  issue_status_t           *issue_status,
  issue_out_to_execute_1_t *issue_out_to_execute_1,
  bit_t                    *issue_status_is_full){
#pragma HLS INLINE off
  bit_t save_input, wait_for_source, is_locked_1, is_locked_2;
  int   rv1, rv2;
  is_locked_1 =
    issue_in_from_decode.decoded_instruction.is_rs1_reg &&
    is_reg_computed[
      issue_in_from_decode.decoded_instruction.rs1];
  is_locked_2 =
    issue_in_from_decode.decoded_instruction.is_rs2_reg &&
    is_reg_computed[
      issue_in_from_decode.decoded_instruction.rs2];
  wait_for_source = is_locked_1 || is_locked_2;
  issue_out_to_execute_1->is_valid =
    !execute_1_status_is_full      &&
    (issue_in_from_decode.is_valid ||
     issue_status->is_full)        &&
    !wait_for_source;
  if (issue_out_to_execute_1->is_valid)
    *issue_destination =
       issue_in_from_decode.decoded_instruction.rd;
  else
    *issue_destination = 0;
  save_input =
    issue_in_from_decode.is_valid &&
   !issue_status->is_full;
  if (save_input)
    get_input(issue_in_from_decode, issue_status);
  issue_status->is_full =
    (execute_1_status_is_full      ||
     wait_for_source)              &&
    (issue_in_from_decode.is_valid ||
     issue_status->is_full);
  *issue_status_is_full = issue_status->is_full;
  if (issue_out_to_execute_1->is_valid){
    stage_job(*issue_status, reg_file, &rv1, &rv2);
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
    if (!wait_for_source){
      printf("issued   ");
      printf("%04d\n", issue_status->pc);
    }
#endif
#endif
    set_output(rv1,
               rv2,
              *issue_status,
               issue_out_to_execute_1);
  }
}
