#ifndef __EXECUTE_2
#define __EXECUTE_2

#include "multicycle_pipeline_ip.h"

void init_execute_2_status(execute_2_status_t *execute_2_status);
void execute_2(
  bit_t                         mem_in_from_execute_1_is_valid,
  execute_2_in_from_execute_1_t execute_2_in_from_execute_1,
  execute_2_status_t           *execute_2_status,
  execute_2_out_to_wb_t        *execute_2_out_to_wb,
  bit_t                        *execute_2_status_is_full);

#endif
