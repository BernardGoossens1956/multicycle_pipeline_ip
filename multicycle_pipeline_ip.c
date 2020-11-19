#include "debug_multicycle_pipeline_ip.h"
#include "multicycle_pipeline_ip.h"
#include "new_cycle.h"
#include "fetch.h"
#include "decode.h"
#include "issue.h"
#include "execute_1.h"
#include "execute_2.h"
#include "mem_access.h"
#include "wb.h"
#include "lock_unlock_reg.h"
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
#include <stdio.h>
#endif
#endif

static void init_reg_file(int   *reg_file,
                   bit_t *is_reg_computed){
  reg_num_p1_t r;
  for (r=0; r<NB_REGISTER; r++){
    is_reg_computed[r] = 0;
    reg_file       [r] = 0;
  }
}
static void stat_and_exit_cond(
  execute_1_in_from_issue_t execute_1_in_from_issue,
  wb_in_from_execute_2_t    wb_in_from_execute_2,
  counter_t                *nbi,
  counter_t                *nbc,
  bit_t                    *exit_cond){
#pragma HLS INLINE off
  *nbc = *nbc + 1;
  if (execute_1_in_from_issue.is_valid)
    *nbi = *nbi + 1;
  *exit_cond =
    wb_in_from_execute_2.is_valid &&
    wb_in_from_execute_2.is_ret   &&
    wb_in_from_execute_2.result == 0;
}
void multicycle_pipeline_ip(
  unsigned int  pc,
  unsigned int  code_mem  [CODE_MEM_SIZE/sizeof(instruction_t)],
  unsigned char data_mem_0[DATA_MEM_SIZE/sizeof(int)],
  unsigned char data_mem_1[DATA_MEM_SIZE/sizeof(int)],
  unsigned char data_mem_2[DATA_MEM_SIZE/sizeof(int)],
  unsigned char data_mem_3[DATA_MEM_SIZE/sizeof(int)],
  unsigned int *end_signal){
#pragma HLS INTERFACE s_axilite port=pc
#pragma HLS INTERFACE s_axilite port=code_mem
#pragma HLS INTERFACE s_axilite port=data_mem_0
#pragma HLS INTERFACE s_axilite port=data_mem_1
#pragma HLS INTERFACE s_axilite port=data_mem_2
#pragma HLS INTERFACE s_axilite port=data_mem_3
#pragma HLS INTERFACE s_axilite port=end_signal
#pragma HLS INTERFACE ap_ctrl_none port=return
  int                     reg_file       [NB_REGISTER];
#pragma HLS ARRAY_PARTITION variable=reg_file\
  dim=1 complete
  bit_t                   is_reg_computed[NB_REGISTER];
#pragma HLS ARRAY_PARTITION variable=is_reg_computed\
  dim=1 complete
  fetch_in_from_decode_t        fetch_in_from_decode;
  fetch_in_from_execute_1_t     fetch_in_from_execute_1;
  fetch_status_t                fetch_status;
  fetch_out_to_decode_t         fetch_out_to_decode;
  decode_in_from_fetch_t        decode_in_from_fetch;
  bit_t                         decode_status_is_full;
  decode_status_t               decode_status;
  decode_out_to_fetch_t         decode_out_to_fetch;
  decode_out_to_issue_t         decode_out_to_issue;
  issue_in_from_decode_t        issue_in_from_decode;
  bit_t                         issue_status_is_full;
  issue_status_t                issue_status;
  reg_num_t                     issue_destination;
  issue_out_to_execute_1_t      issue_out_to_execute_1;
  execute_1_in_from_issue_t     execute_1_in_from_issue;
  bit_t                         execute_1_status_is_full;
  execute_1_status_t            execute_1_status;
  execute_1_out_to_fetch_t      execute_1_out_to_fetch;
  execute_1_out_to_mem_t        execute_1_out_to_mem;
  execute_1_out_to_execute_2_t  execute_1_out_to_execute_2;
  execute_2_in_from_execute_1_t execute_2_in_from_execute_1;
  bit_t                         execute_2_status_is_full;
  execute_2_status_t            execute_2_status;
  execute_2_out_to_wb_t         execute_2_out_to_wb;
  mem_in_from_execute_1_t       mem_in_from_execute_1;
  mem_status_t                  mem_status;
  mem_out_to_wb_t               mem_out_to_wb;
  wb_in_from_execute_2_t        wb_in_from_execute_2;
  wb_in_from_mem_t              wb_in_from_mem;
  wb_status_t                   wb_status;
  reg_num_t                     wb_destination;
  counter_t                     nbi;
  counter_t                     nbc;
  bit_t                         exit_cond;
  init_fetch_status    (&fetch_status);
  init_decode_status   (&decode_status);
  init_issue_status    (&issue_status);
  init_execute_1_status(&execute_1_status);
  init_execute_2_status(&execute_2_status);
  init_reg_file        (reg_file, is_reg_computed);
  issue_destination                   = 0;
  wb_destination                      = 0;
  fetch_out_to_decode.is_valid        = 0;
  decode_out_to_fetch.target_pc       = pc;
  decode_out_to_fetch.is_valid        = 1;
  decode_out_to_issue.is_valid        = 0;
  decode_status_is_full               = 0;
  issue_out_to_execute_1.is_valid     = 0;
  issue_status_is_full                = 0;
  execute_1_out_to_fetch.is_valid     = 0;
  execute_1_out_to_mem.is_valid       = 0;
  execute_1_out_to_execute_2.is_valid = 0;
  execute_1_status_is_full            = 0;
  execute_2_out_to_wb.is_valid        = 0;
  execute_2_status_is_full            = 0;
  mem_out_to_wb.is_valid              = 0;
  nbi                                 = 0;
  nbc                                 = 0;
  exit_cond                           = 0;
  do {
#pragma HLS PIPELINE II=2
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
    printf("==============================================\n");
    printf("cycle %d\n", (unsigned int)nbc);
#endif
#endif
    new_cycle(fetch_out_to_decode,
              decode_out_to_fetch,
              decode_out_to_issue,
              issue_out_to_execute_1,
              execute_1_out_to_fetch,
              execute_1_out_to_execute_2,
              execute_1_out_to_mem,
              execute_2_out_to_wb,
              mem_out_to_wb,
             &fetch_in_from_decode,
             &fetch_in_from_execute_1,
             &decode_in_from_fetch,
             &issue_in_from_decode,
             &execute_1_in_from_issue,
             &execute_2_in_from_execute_1,
             &mem_in_from_execute_1,
             &wb_in_from_execute_2,
             &wb_in_from_mem);
    fetch(fetch_in_from_decode,
          fetch_in_from_execute_1,
          decode_status_is_full,
          code_mem,
         &fetch_status,
         &fetch_out_to_decode);
    decode(decode_in_from_fetch,
           issue_status_is_full,
          &decode_status,
          &decode_out_to_fetch,
          &decode_out_to_issue,
          &decode_status_is_full);
    issue(issue_in_from_decode,
          execute_1_status_is_full,
         &issue_destination,
          reg_file,
          is_reg_computed,
         &issue_status,
         &issue_out_to_execute_1,
         &issue_status_is_full);
    execute_1(execute_1_in_from_issue,
              execute_2_status_is_full,
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
              reg_file,
#endif
#endif
             &execute_1_status,
             &execute_1_out_to_fetch,
             &execute_1_out_to_mem,
             &execute_1_out_to_execute_2,
             &execute_1_status_is_full);
    execute_2(mem_in_from_execute_1.is_valid,
              execute_2_in_from_execute_1,
             &execute_2_status,
             &execute_2_out_to_wb,
             &execute_2_status_is_full);
    mem_access(mem_in_from_execute_1,
               data_mem_0,
               data_mem_1,
               data_mem_2,
               data_mem_3,
              &mem_status,
              &mem_out_to_wb);
    write_back(wb_in_from_execute_2,
               wb_in_from_mem,
               reg_file,
              &wb_destination,
              &wb_status);
    lock_unlock_reg(issue_destination,
                    wb_destination,
                    is_reg_computed);
    stat_and_exit_cond(execute_1_in_from_issue,
                       wb_in_from_execute_2,
                      &nbi,
                      &nbc,
                      &exit_cond);
  } while (!exit_cond);
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
  printf("==============================================\n");
#endif
#endif
  data_mem_0[64] = nbc;
  data_mem_1[64] = nbc>>8;
  data_mem_2[64] = nbc>>16;
  data_mem_3[64] = nbc>>24;
  data_mem_0[65] = nbi;
  data_mem_1[65] = nbi>>8;
  data_mem_2[65] = nbi>>16;
  data_mem_3[65] = nbi>>24;
  *end_signal    = 1;
}
