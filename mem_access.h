#ifndef __MEM_ACCESS
#define __MEM_ACCESS

#include "multicycle_pipeline_ip.h"

void mem_access(
  mem_in_from_execute_1_t mem_in_from_execute_1,
  unsigned char          *data_mem_0,
  unsigned char          *data_mem_1,
  unsigned char          *data_mem_2,
  unsigned char          *data_mem_3,
  mem_status_t           *mem_status,
  mem_out_to_wb_t        *mem_out_to_wb);

#endif
