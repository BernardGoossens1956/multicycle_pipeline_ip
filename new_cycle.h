#ifndef __NEW_CYCLE
#define __NEW_CYCLE

#include "multicycle_pipeline_ip.h"

void new_cycle(
  fetch_out_to_decode_t          fetch_out_to_decode,
  decode_out_to_fetch_t          decode_out_to_fetch,
  decode_out_to_issue_t          decode_out_to_issue,
  issue_out_to_execute_1_t       issue_out_to_execute_1,
  execute_1_out_to_fetch_t       execute_1_out_to_fetch,
  execute_1_out_to_execute_2_t   execute_1_out_to_execute_2,
  execute_1_out_to_mem_t         execute_1_out_to_mem,
  execute_2_out_to_wb_t          execute_2_out_to_wb,
  mem_out_to_wb_t                mem_out_to_wb,
  fetch_in_from_decode_t        *fetch_in_from_decode,
  fetch_in_from_execute_1_t     *fetch_in_from_execute_1,
  decode_in_from_fetch_t        *decode_in_from_fetch,
  issue_in_from_decode_t        *issue_in_from_decode,
  execute_1_in_from_issue_t     *execute_1_in_from_issue,
  execute_2_in_from_execute_1_t *execute_2_in_from_execute_1,
  mem_in_from_execute_1_t       *mem_in_from_execute_1,
  wb_in_from_execute_2_t        *wb_in_from_execute_2,
  wb_in_from_mem_t              *wb_in_from_mem);

#endif
