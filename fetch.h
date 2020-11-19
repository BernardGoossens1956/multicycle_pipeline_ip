#ifndef __FETCH
#define __FETCH

#include "multicycle_pipeline_ip.h"

void init_fetch_status(fetch_status_t *fetch_status);
void fetch(
  fetch_in_from_decode_t    fetch_in_from_decode,
  fetch_in_from_execute_1_t fetch_in_from_execute_1,
  bit_t                     decode_status_is_full,
  unsigned int             *code_mem,
  fetch_status_t           *fetch_status,
  fetch_out_to_decode_t    *fetch_out_to_decode);

#endif
