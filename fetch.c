#include "debug_multicycle_pipeline_ip.h"
#include "multicycle_pipeline_ip.h"
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
#include <stdio.h>
#endif
#endif

void init_fetch_status(fetch_status_t *fetch_status){
  fetch_status->is_full     = 0;
}
static void get_input(
  fetch_in_from_decode_t    fetch_in_from_decode,
  fetch_in_from_execute_1_t fetch_in_from_execute_1,
  fetch_status_t           *fetch_status){
#pragma HLS INLINE
  if (fetch_in_from_execute_1.is_valid)
    fetch_status->pc = fetch_in_from_execute_1.target_pc;
  else //if (fetch_in_from_decode.is_valid)
    fetch_status->pc = fetch_in_from_decode.target_pc;
}
static void stage_job(
  fetch_status_t  fetch_status,
  unsigned int   *code_mem,
  instruction_t  *instruction){
#pragma HLS INLINE
  *instruction =
    code_mem[fetch_status.pc >> 2];
}
static void set_output(
  instruction_t          instruction,
  fetch_status_t        *fetch_status,
  fetch_out_to_decode_t *fetch_out_to_decode){
#pragma HLS INLINE
  fetch_out_to_decode->pc          =
    fetch_status->pc;
  fetch_out_to_decode->instruction =
    instruction;
}
void fetch(
  fetch_in_from_decode_t    fetch_in_from_decode,
  fetch_in_from_execute_1_t fetch_in_from_execute_1,
  bit_t                     decode_status_is_full,
  unsigned int             *code_mem,
  fetch_status_t           *fetch_status,
  fetch_out_to_decode_t    *fetch_out_to_decode){
#pragma HLS INLINE off
  bit_t         save_input;
  instruction_t instruction;
  save_input = 
    ( fetch_in_from_decode.is_valid     ||
      fetch_in_from_execute_1.is_valid) &&
     !fetch_status->is_full;
  if (save_input)
    get_input(fetch_in_from_decode,
              fetch_in_from_execute_1,
              fetch_status);
  fetch_out_to_decode->is_valid =
   !decode_status_is_full            &&
   (fetch_in_from_decode.is_valid    ||
    fetch_in_from_execute_1.is_valid ||
    fetch_status->is_full);
  fetch_status->is_full =
     decode_status_is_full &&
    (fetch_in_from_decode.is_valid    ||
     fetch_in_from_execute_1.is_valid ||
     fetch_status->is_full);
  if (fetch_out_to_decode->is_valid){
    stage_job(*fetch_status, code_mem, &instruction);
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
    printf("fetched  ");
    printf("%04d: %08x      \n",
      (unsigned int)fetch_status->pc,
      (unsigned int)instruction);
#endif
#endif
    set_output(instruction, fetch_status, fetch_out_to_decode);
  }
}
