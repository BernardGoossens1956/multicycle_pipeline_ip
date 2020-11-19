#ifndef __EXECUTE_1
#define __EXECUTE_1

#include "multicycle_pipeline_ip.h"

void init_execute_1_status(execute_1_status_t *execute_1_status);
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
  bit_t                        *execute_1_status_is_full);

#endif
