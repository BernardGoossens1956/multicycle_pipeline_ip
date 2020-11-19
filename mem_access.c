#include "debug_multicycle_pipeline_ip.h"
#include "multicycle_pipeline_ip.h"
#include "mem.h"
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
#include "stdio.h"
#endif
#endif

static void get_input(
  mem_in_from_execute_1_t mem_in_from_execute_1,
  mem_status_t           *mem_status){
#pragma HLS INLINE
  mem_status->value_0             =
    mem_in_from_execute_1.value_0;
  mem_status->value_1             =
    mem_in_from_execute_1.value_1;
  mem_status->value_2             =
    mem_in_from_execute_1.value_2;
  mem_status->value_3             =
    mem_in_from_execute_1.value_3;
  mem_status->address             =
    mem_in_from_execute_1.address;
  mem_status->rd                  =
    mem_in_from_execute_1.rd;
  mem_status->has_no_dest         =
    mem_in_from_execute_1.has_no_dest;
  mem_status->is_load             =
    mem_in_from_execute_1.is_load;
  mem_status->is_store            =
    mem_in_from_execute_1.is_store;
  mem_status->func3               =
    mem_in_from_execute_1.func3;
#ifndef __SYNTHESIS__
  mem_status->pc                  =
    mem_in_from_execute_1.pc;
  mem_status->instruction         =
    mem_in_from_execute_1.instruction;
  mem_status->decoded_instruction =
    mem_in_from_execute_1.decoded_instruction;
#endif
}
static void stage_job(
  uint2          a01,
  bit_t          a1,
  data_address_t a2,
  func3_t        msize,
  mem_status_t  *mem_status,
  unsigned char *data_mem_0,
  unsigned char *data_mem_1,
  unsigned char *data_mem_2,
  unsigned char *data_mem_3){
#pragma HLS INLINE
  if (mem_status->is_load)
    mem_load(
        data_mem_0,
        data_mem_1,
        data_mem_2,
        data_mem_3,
        a2,
       &(mem_status->value_0),
       &(mem_status->value_1),
       &(mem_status->value_2),
       &(mem_status->value_3));
  else //if (mem_status->is_store)
    mem_store(
      data_mem_0,
      data_mem_1,
      data_mem_2,
      data_mem_3,
      a01,
      a1,
      a2,
      mem_status->value_0,
      mem_status->value_1,
      mem_status->value_2,
      mem_status->value_3,
      (uint2)msize);
}
static void set_output(
  uint2            a01,
  bit_t            a1,
  func3_t          msize,
  mem_status_t     mem_status,
  mem_out_to_wb_t *mem_out_to_wb){
#pragma HLS INLINE
  mem_out_to_wb->value_0             =
    mem_status.value_0;
  mem_out_to_wb->value_1             =
    mem_status.value_1;
  mem_out_to_wb->value_2             =
    mem_status.value_2;
  mem_out_to_wb->value_3             =
    mem_status.value_3;
  mem_out_to_wb->a01                 =
    a01;
  mem_out_to_wb->a1                  =
    a1;
  mem_out_to_wb->msize               =
    msize;
  mem_out_to_wb->rd                  =
    mem_status.rd;
  mem_out_to_wb->has_no_dest         =
    mem_status.has_no_dest;
#ifndef __SYNTHESIS__
  mem_out_to_wb->pc                  =
    mem_status.pc;
  mem_out_to_wb->instruction         =
    mem_status.instruction;
  mem_out_to_wb->decoded_instruction =
    mem_status.decoded_instruction;
#endif
}
void mem_access(
  mem_in_from_execute_1_t mem_in_from_execute_1,
  unsigned char          *data_mem_0,
  unsigned char          *data_mem_1,
  unsigned char          *data_mem_2,
  unsigned char          *data_mem_3,
  mem_status_t           *mem_status,
  mem_out_to_wb_t        *mem_out_to_wb){
#pragma HLS INLINE off
  bit_t          save_input;
  bit_t          a1;
  uint2          a01;
  data_address_t a2;
  func3_t        msize;
  save_input =
    mem_in_from_execute_1.is_valid;
  if (save_input)
    get_input(mem_in_from_execute_1, mem_status);
  if (mem_in_from_execute_1.is_valid){
    a01   =  mem_status->address;
    a1    = (mem_status->address >> 1);
    a2    = (mem_status->address >> 2)&(DATA_MEM_SIZE/4 - 1);
    msize =  mem_status->func3;
    stage_job(a01,
              a1,
              a2,
              msize,
              mem_status,
              data_mem_0,
              data_mem_1,
              data_mem_2,
              data_mem_3);
#ifndef __SYNTHESIS__
#ifdef DEBUG_PIPELINE
    printf("mem      ");
    printf("%04d\n", mem_status->pc);
#endif
#endif
    set_output(a01, a1, msize, *mem_status, mem_out_to_wb);
  }
  mem_out_to_wb->is_valid =
#ifdef __SYNTHESIS__
   !mem_status->is_store &&
#endif
    mem_in_from_execute_1.is_valid;
}
