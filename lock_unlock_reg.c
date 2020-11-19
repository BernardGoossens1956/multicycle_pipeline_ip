#include "multicycle_pipeline_ip.h"

void lock_unlock_reg(
  reg_num_t issue_destination,
  reg_num_t wb_destination,
  bit_t    *is_reg_computed){
#pragma HLS INLINE off
  uint6 r;
  for (r=1; r<NB_REGISTER; r++){
#pragma HLS UNROLL skip_exit_check
    if (wb_destination == r)
      is_reg_computed[wb_destination]    = 0;
    else if (issue_destination == r)
      is_reg_computed[issue_destination] = 1;
  }
}
