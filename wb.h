#ifndef __WB
#define __WB

#include "multicycle_pipeline_ip.h"

void write_back(
  wb_in_from_execute_2_t wb_in_from_execute_2,
  wb_in_from_mem_t       wb_in_from_mem,
  int                   *reg_file,
  reg_num_t             *wb_destination,
  wb_status_t           *wb_status);

#endif
