#ifndef __ISSUE
#define __ISSUE

#include "multicycle_pipeline_ip.h"

void init_issue_status(issue_status_t *issue_status);
void issue(
  issue_in_from_decode_t    issue_in_from_decode,
  bit_t                     execute_1_status_is_full,
  reg_num_t                *issue_destination,
  int                      *reg_file,
  bit_t                    *is_reg_computed,
  issue_status_t           *issue_status,
  issue_out_to_execute_1_t *issue_out_to_execute_1,
  bit_t                    *issue_status_is_full);

#endif
