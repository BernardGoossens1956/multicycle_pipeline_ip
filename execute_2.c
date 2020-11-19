#include "debug_multicycle_pipeline_ip.h"
#include "multicycle_pipeline_ip.h"
#include "compute.h"
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
#include "stdio.h"
#endif
#endif

void init_execute_2_status(execute_2_status_t *execute_2_status){
  execute_2_status->is_full = 0;
}
static void get_input(
  execute_2_in_from_execute_1_t execute_2_in_from_execute_1,
  execute_2_status_t           *execute_2_status){
#pragma HLS INLINE
  execute_2_status->result1             =
    execute_2_in_from_execute_1.result1;
  execute_2_status->result2             =
    execute_2_in_from_execute_1.result2;
  execute_2_status->rd                  =
    execute_2_in_from_execute_1.rd;
  execute_2_status->has_no_dest         =
    execute_2_in_from_execute_1.has_no_dest;
  execute_2_status->is_r_type           =
    execute_2_in_from_execute_1.is_r_type;
  execute_2_status->is_op_imm           =
    execute_2_in_from_execute_1.is_op_imm;
  execute_2_status->is_ret              =
    execute_2_in_from_execute_1.is_ret;
#ifndef __SYNTHESIS__
  execute_2_status->pc                  =
    execute_2_in_from_execute_1.pc;
  execute_2_status->instruction         =
    execute_2_in_from_execute_1.instruction;
  execute_2_status->decoded_instruction =
    execute_2_in_from_execute_1.decoded_instruction;
  execute_2_status->target_pc           =
  execute_2_in_from_execute_1.target_pc;
#endif
}
static void stage_job(
  execute_2_status_t  execute_2_status,
  int                *result){
#pragma HLS INLINE
  *result =
   (execute_2_status.is_r_type ||
    execute_2_status.is_op_imm)?
    execute_2_status.result1   :
    execute_2_status.result2;
}
static void set_output(
  int                       result,
  execute_2_status_t        execute_2_status,
  execute_2_out_to_wb_t    *execute_2_out_to_wb){
#pragma HLS INLINE
  execute_2_out_to_wb->result               =
    (execute_2_status.is_ret) ?
    execute_2_status.result2:
    result;
  execute_2_out_to_wb->rd                   =
    execute_2_status.rd;
  execute_2_out_to_wb->has_no_dest          =
    execute_2_status.has_no_dest;
  execute_2_out_to_wb->is_ret               =
    execute_2_status.is_ret;
#ifndef __SYNTHESIS__
  execute_2_out_to_wb->pc                   =
    execute_2_status.pc;
  execute_2_out_to_wb->instruction          =
    execute_2_status.instruction;
  execute_2_out_to_wb->decoded_instruction  =
    execute_2_status.decoded_instruction;
  execute_2_out_to_wb->target_pc            =
    execute_2_status.target_pc;
#endif
}
void execute_2(
  bit_t                         mem_in_from_execute_1_is_valid,
  execute_2_in_from_execute_1_t execute_2_in_from_execute_1,
  execute_2_status_t           *execute_2_status,
  execute_2_out_to_wb_t        *execute_2_out_to_wb,
  bit_t                        *execute_2_status_is_full){
#pragma HLS INLINE off
  bit_t save_input, execute_2_out_to_mem_is_valid;
  int   result;
  save_input =
    execute_2_in_from_execute_1.is_valid &&
   !execute_2_status->is_full;
  if (save_input)
    get_input(execute_2_in_from_execute_1, execute_2_status);
  execute_2_out_to_wb->is_valid    =
   (execute_2_status->is_full                     ||
    execute_2_in_from_execute_1.is_valid)/*         &&
   !execute_2_status->decoded_instruction.is_load &&
   !execute_2_status->decoded_instruction.is_store*/;
  execute_2_status->is_full =
     mem_in_from_execute_1_is_valid                &&
    (execute_2_in_from_execute_1.is_valid          ||
     execute_2_status->is_full)/*                    &&
    !execute_2_status->decoded_instruction.is_load &&
    !execute_2_status->decoded_instruction.is_store*/;
  *execute_2_status_is_full = execute_2_status->is_full;
  if (execute_2_out_to_wb->is_valid){
    stage_job(*execute_2_status, &result);
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
    printf("exe_ed_2 ");
    printf("%04d\n", execute_2_status->pc);
#endif
#endif
    set_output(result,
              *execute_2_status,
               execute_2_out_to_wb);
  }
}
