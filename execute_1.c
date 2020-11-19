#include "debug_multicycle_pipeline_ip.h"
#include "multicycle_pipeline_ip.h"
#include "compute.h"
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
#include "stdio.h"
#endif
#endif

void init_execute_1_status(execute_1_status_t *execute_1_status){
  execute_1_status->is_full = 0;
}
static void get_input(
  execute_1_in_from_issue_t execute_1_in_from_issue,
  execute_1_status_t       *execute_1_status){
#pragma HLS INLINE
  execute_1_status->rv1                 =
    execute_1_in_from_issue.rv1;
  execute_1_status->rv2                 =
    execute_1_in_from_issue.rv2;
  execute_1_status->pc                  =
    execute_1_in_from_issue.pc;
  execute_1_status->decoded_instruction =
    execute_1_in_from_issue.decoded_instruction;
#ifndef __SYNTHESIS__
  execute_1_status->target_pc           =
    execute_1_in_from_issue.target_pc;
  execute_1_status->instruction         =
    execute_1_in_from_issue.instruction;
#endif
}
static void compute(
  execute_1_status_t *execute_1_status,
  bit_t              *bcond,
  int                *result1,
  int                *result2,
  code_address_t     *target_pc){
#pragma HLS INLINE
  *bcond = compute_branch_result(
    execute_1_status->rv1,
    execute_1_status->rv2,
    execute_1_status->decoded_instruction.func3);
  *result1 = compute_op_result(
    execute_1_status->rv1,
    execute_1_status->rv2,
    execute_1_status->decoded_instruction);
  *result2 = compute_result(
    execute_1_status->rv1,
    execute_1_status->pc,
    execute_1_status->decoded_instruction);
  *target_pc = compute_next_pc(
    execute_1_status->pc,
    execute_1_status->decoded_instruction,
    execute_1_status->rv1);
}
static void stage_job(
  execute_1_status_t  execute_1_status,
  bit_t               bcond,
  code_address_t      tpc,
  code_address_t     *target_pc){
#pragma HLS INLINE
  if (!execute_1_status.decoded_instruction.is_jal)
    *target_pc =
     (bcond ||
      execute_1_status.decoded_instruction.is_jalr) ?
      tpc                                      :
      execute_1_status.pc + 4;
}
static void set_output(
  int                           result1,
  int                           result2,
  code_address_t                target_pc,
  execute_1_status_t            execute_1_status,
  execute_1_out_to_fetch_t     *execute_1_out_to_fetch,
  execute_1_out_to_mem_t       *execute_1_out_to_mem,
  execute_1_out_to_execute_2_t *execute_1_out_to_execute_2){
#pragma HLS INLINE
  execute_1_out_to_fetch->target_pc         =
    target_pc;
  execute_1_out_to_mem->address             =
    result2;
  execute_1_out_to_mem->value_0             =
    execute_1_status.rv2;
  execute_1_out_to_mem->value_1             =
    execute_1_status.rv2 >> 8;
  execute_1_out_to_mem->value_2             =
    execute_1_status.rv2 >> 16;
  execute_1_out_to_mem->value_3             =
    execute_1_status.rv2 >> 24;
  execute_1_out_to_mem->rd                  =
    execute_1_status.decoded_instruction.rd;
  execute_1_out_to_mem->has_no_dest         =
    execute_1_status.decoded_instruction.has_no_dest;
  execute_1_out_to_mem->is_load             =
    execute_1_status.decoded_instruction.is_load;
  execute_1_out_to_mem->is_store            =
    execute_1_status.decoded_instruction.is_store;
  execute_1_out_to_mem->func3               =
    execute_1_status.decoded_instruction.func3;
#ifndef __SYNTHESIS__
  execute_1_out_to_mem->pc                  =
    execute_1_status.pc;
  execute_1_out_to_mem->instruction         =
    execute_1_status.instruction;
  execute_1_out_to_mem->decoded_instruction =
    execute_1_status.decoded_instruction;
#endif
  execute_1_out_to_execute_2->result1             =
    result1;
  execute_1_out_to_execute_2->result2             =
    (execute_1_status.decoded_instruction.is_ret)?
    target_pc                                    :
    result2;
  execute_1_out_to_execute_2->rd                  =
    execute_1_status.decoded_instruction.rd;
  execute_1_out_to_execute_2->has_no_dest         =
    execute_1_status.decoded_instruction.has_no_dest;
  execute_1_out_to_execute_2->is_r_type           =
    execute_1_status.decoded_instruction.is_r_type;
  execute_1_out_to_execute_2->is_op_imm           =
    execute_1_status.decoded_instruction.is_op_imm;
  execute_1_out_to_execute_2->is_ret              =
    execute_1_status.decoded_instruction.is_ret;
#ifndef __SYNTHESIS__
  execute_1_out_to_execute_2->pc                  =
    execute_1_status.pc;
  execute_1_out_to_execute_2->instruction         =
    execute_1_status.instruction;
  execute_1_out_to_execute_2->decoded_instruction =
    execute_1_status.decoded_instruction;
  execute_1_out_to_execute_2->target_pc           =
    target_pc;
#endif
}
void execute_1(
  execute_1_in_from_issue_t     execute_1_in_from_issue,
  bit_t                         execute_2_status_is_full,
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
  int                          *reg_file,
#endif
#endif
  execute_1_status_t           *execute_1_status,
  execute_1_out_to_fetch_t     *execute_1_out_to_fetch,
  execute_1_out_to_mem_t       *execute_1_out_to_mem,
  execute_1_out_to_execute_2_t *execute_1_out_to_execute_2,
  bit_t                        *execute_1_status_is_full){
#pragma HLS INLINE off
  bit_t          save_input, bcond;
  int            result1, result2;
  code_address_t tpc, target_pc;
  save_input =
    execute_1_in_from_issue.is_valid &&
   !execute_1_status->is_full;
  if (save_input)
    get_input(execute_1_in_from_issue, execute_1_status);
  compute(
    execute_1_status,
   &bcond,
   &result1,
   &result2,
   &tpc);
  execute_1_out_to_fetch->is_valid =
   (execute_1_status->is_full                       ||
    execute_1_in_from_issue.is_valid)               &&
   (execute_1_status->decoded_instruction.is_branch ||
    execute_1_status->decoded_instruction.is_jalr);
  execute_1_out_to_execute_2->is_valid =
   !execute_2_status_is_full                        &&
   (execute_1_status->is_full                       ||
    execute_1_in_from_issue.is_valid)               &&
   !execute_1_status->decoded_instruction.is_branch &&
   !execute_1_status->decoded_instruction.is_load   &&
   !execute_1_status->decoded_instruction.is_store;
  execute_1_out_to_mem->is_valid   =
   (execute_1_status->is_full                     ||
    execute_1_in_from_issue.is_valid)             &&
   (execute_1_status->decoded_instruction.is_load ||
    execute_1_status->decoded_instruction.is_store);
  execute_1_status->is_full =
    execute_2_status_is_full         &&
   (execute_1_in_from_issue.is_valid ||
    execute_1_status->is_full)       &&
   !execute_1_status->decoded_instruction.is_branch;
  *execute_1_status_is_full = execute_1_status->is_full;
  if (execute_1_out_to_execute_2->is_valid ||
      execute_1_out_to_mem->is_valid       ||
      execute_1_out_to_fetch->is_valid){
    stage_job(*execute_1_status, bcond, tpc, &target_pc);
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
    printf("exe_ed_1 ");
    printf("%04d\n", execute_1_status->pc);
    if (execute_1_status->decoded_instruction.is_branch ||
        execute_1_status->decoded_instruction.is_jalr)
      emulate(reg_file,
              execute_1_status->decoded_instruction,
              target_pc);
#endif
#endif
    set_output(result1,
               result2,
               target_pc,
              *execute_1_status,
               execute_1_out_to_fetch,
               execute_1_out_to_mem,
               execute_1_out_to_execute_2);
  }
}
