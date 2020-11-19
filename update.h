#ifndef __UPDATE
#define __UPDATE
#include "multicycle_pipeline_ip.h"

void update(
  reg_num_t issue_destination,
  reg_num_t wb_destination,
  bit_t    *is_reg_computed);

#endif
