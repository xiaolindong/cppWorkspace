/* Test the `vrndpf32' ARM Neon intrinsic.  */
/* This file was autogenerated by neon-testgen.  */

/* { dg-do assemble } */
/* { dg-require-effective-target arm_v8_neon_ok } */
/* { dg-options "-save-temps -O0" } */
/* { dg-add-options arm_v8_neon } */

#include "arm_neon.h"

void test_vrndpf32 (void)
{
  float32x2_t out_float32x2_t;
  float32x2_t arg0_float32x2_t;

  out_float32x2_t = vrndp_f32 (arg0_float32x2_t);
}

/* { dg-final { scan-assembler "vrintp\.f32\[ 	\]+\[dD\]\[0-9\]+, \[dD\]\[0-9\]+!?\(\[ 	\]+@\[a-zA-Z0-9 \]+\)?\n" } } */
