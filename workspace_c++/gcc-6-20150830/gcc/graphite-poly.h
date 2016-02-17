/* Graphite polyhedral representation.
   Copyright (C) 2009-2015 Free Software Foundation, Inc.
   Contributed by Sebastian Pop <sebastian.pop@amd.com> and
   Tobias Grosser <grosser@fim.uni-passau.de>.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_GRAPHITE_POLY_H
#define GCC_GRAPHITE_POLY_H

#include "sese.h"

#ifndef HAVE_ISL_OPTIONS_SET_SCHEDULE_SERIALIZE_SCCS
# define isl_stat int
# define isl_stat_ok 0
#endif

typedef struct poly_dr *poly_dr_p;

typedef struct poly_bb *poly_bb_p;

typedef struct scop *scop_p;

typedef unsigned graphite_dim_t;

static inline graphite_dim_t scop_nb_params (scop_p);

/* A data reference can write or read some memory or we
   just know it may write some memory.  */
enum poly_dr_type
{
  PDR_READ,
  /* PDR_MAY_READs are represented using PDR_READS.  This does not
     limit the expressiveness.  */
  PDR_WRITE,
  PDR_MAY_WRITE
};

struct poly_dr
{
  /* An identifier for this PDR.  */
  int id;

  /* The number of data refs identical to this one in the PBB.  */
  int nb_refs;

  /* A pointer to compiler's data reference description.  */
  void *compiler_dr;

  /* A pointer to the PBB that contains this data reference.  */
  poly_bb_p pbb;

  enum poly_dr_type type;

  /* The access polyhedron contains the polyhedral space this data
     reference will access.

     The polyhedron contains these dimensions:

     - The alias set (a):
     Every memory access is classified in at least one alias set.

     - The subscripts (s_0, ..., s_n):
     The memory is accessed using zero or more subscript dimensions.

     - The iteration domain (variables and parameters)

     Do not hardcode the dimensions.  Use the following accessor functions:
     - pdr_alias_set_dim
     - pdr_subscript_dim
     - pdr_iterator_dim
     - pdr_parameter_dim

     Example:

     | int A[1335][123];
     | int *p = malloc ();
     |
     | k = ...
     | for i
     |   {
     |     if (unknown_function ())
     |       p = A;
     |       ... = p[?][?];
     | 	   for j
     |       A[i][j+k] = m;
     |   }

     The data access A[i][j+k] in alias set "5" is described like this:

     | i   j   k   a  s0  s1   1
     | 0   0   0   1   0   0  -5     =  0
     |-1   0   0   0   1   0   0     =  0
     | 0  -1  -1   0   0   1   0     =  0
     | 0   0   0   0   1   0   0     >= 0  # The last four lines describe the
     | 0   0   0   0   0   1   0     >= 0  # array size.
     | 0   0   0   0  -1   0 1335    >= 0
     | 0   0   0   0   0  -1 123     >= 0

     The pointer "*p" in alias set "5" and "7" is described as a union of
     polyhedron:


     | i   k   a  s0   1
     | 0   0   1   0  -5   =  0
     | 0   0   0   1   0   >= 0

     "or"

     | i   k   a  s0   1
     | 0   0   1   0  -7   =  0
     | 0   0   0   1   0   >= 0

     "*p" accesses all of the object allocated with 'malloc'.

     The scalar data access "m" is represented as an array with zero subscript
     dimensions.

     | i   j   k   a   1
     | 0   0   0  -1   15  = 0

     The difference between the graphite internal format for access data and
     the OpenSop format is in the order of columns.
     Instead of having:

     | i   j   k   a  s0  s1   1
     | 0   0   0   1   0   0  -5     =  0
     |-1   0   0   0   1   0   0     =  0
     | 0  -1  -1   0   0   1   0     =  0
     | 0   0   0   0   1   0   0     >= 0  # The last four lines describe the
     | 0   0   0   0   0   1   0     >= 0  # array size.
     | 0   0   0   0  -1   0 1335    >= 0
     | 0   0   0   0   0  -1 123     >= 0

     In OpenScop we have:

     | a  s0  s1   i   j   k   1
     | 1   0   0   0   0   0  -5     =  0
     | 0   1   0  -1   0   0   0     =  0
     | 0   0   1   0  -1  -1   0     =  0
     | 0   1   0   0   0   0   0     >= 0  # The last four lines describe the
     | 0   0   1   0   0   0   0     >= 0  # array size.
     | 0  -1   0   0   0   0 1335    >= 0
     | 0   0  -1   0   0   0 123     >= 0

     The OpenScop access function is printed as follows:

     | 1  # The number of disjunct components in a union of access functions.
     | R C O I L P  # Described bellow.
     | a  s0  s1   i   j   k   1
     | 1   0   0   0   0   0  -5     =  0
     | 0   1   0  -1   0   0   0     =  0
     | 0   0   1   0  -1  -1   0     =  0
     | 0   1   0   0   0   0   0     >= 0  # The last four lines describe the
     | 0   0   1   0   0   0   0     >= 0  # array size.
     | 0  -1   0   0   0   0 1335    >= 0
     | 0   0  -1   0   0   0 123     >= 0

     Where:
     - R: Number of rows.
     - C: Number of columns.
     - O: Number of output dimensions = alias set + number of subscripts.
     - I: Number of input dimensions (iterators).
     - L: Number of local (existentially quantified) dimensions.
     - P: Number of parameters.

     In the example, the vector "R C O I L P" is "7 7 3 2 0 1".  */
  isl_map *accesses;
  isl_set *subscript_sizes;

  /* Data reference's base object set number, we must assure 2 pdrs are in the
     same base object set before dependency checking.  */
  int dr_base_object_set;

  /* The number of subscripts.  */
  graphite_dim_t nb_subscripts;
};

#define PDR_ID(PDR) (PDR->id)
#define PDR_NB_REFS(PDR) (PDR->nb_refs)
#define PDR_CDR(PDR) (PDR->compiler_dr)
#define PDR_PBB(PDR) (PDR->pbb)
#define PDR_TYPE(PDR) (PDR->type)
#define PDR_ACCESSES(PDR) (NULL)
#define PDR_BASE_OBJECT_SET(PDR) (PDR->dr_base_object_set)
#define PDR_NB_SUBSCRIPTS(PDR) (PDR->nb_subscripts)

void new_poly_dr (poly_bb_p, int, enum poly_dr_type, void *,
		  graphite_dim_t, isl_map *, isl_set *);
void free_poly_dr (poly_dr_p);
void debug_pdr (poly_dr_p, int);
void print_pdr (FILE *, poly_dr_p, int);

static inline bool
pdr_read_p (poly_dr_p pdr)
{
  return PDR_TYPE (pdr) == PDR_READ;
}

/* Returns true when PDR is a "write".  */

static inline bool
pdr_write_p (poly_dr_p pdr)
{
  return PDR_TYPE (pdr) == PDR_WRITE;
}

/* Returns true when PDR is a "may write".  */

static inline bool
pdr_may_write_p (poly_dr_p pdr)
{
  return PDR_TYPE (pdr) == PDR_MAY_WRITE;
}

/* POLY_BB represents a blackbox in the polyhedral model.  */

struct poly_bb
{
  /* Pointer to a basic block or a statement in the compiler.  */
  void *black_box;

  /* Pointer to the SCOP containing this PBB.  */
  scop_p scop;

  /* The iteration domain of this bb.  The layout of this polyhedron
     is I|G with I the iteration domain, G the context parameters.

     Example:

     for (i = a - 7*b + 8; i <= 3*a + 13*b + 20; i++)
       for (j = 2; j <= 2*i + 5; j++)
         for (k = 0; k <= 5; k++)
           S (i,j,k)

     Loop iterators: i, j, k
     Parameters: a, b

     | i >=  a -  7b +  8
     | i <= 3a + 13b + 20
     | j >= 2
     | j <= 2i + 5
     | k >= 0
     | k <= 5

     The number of variables in the DOMAIN may change and is not
     related to the number of loops in the original code.  */
  isl_set *domain;

  /* The data references we access.  */
  vec<poly_dr_p> drs;

  /* The original scattering.  */
  isl_map *schedule;

  /* The transformed scattering.  */
  isl_map *transformed;

  /* A copy of the transformed scattering.  */
  isl_map *saved;

  /* True when this PBB contains only a reduction statement.  */
  bool is_reduction;
};

#define PBB_BLACK_BOX(PBB) ((gimple_bb_p) PBB->black_box)
#define PBB_SCOP(PBB) (PBB->scop)
#define PBB_DRS(PBB) (PBB->drs)
#define PBB_IS_REDUCTION(PBB) (PBB->is_reduction)

extern poly_bb_p new_poly_bb (scop_p, void *);
extern void free_poly_bb (poly_bb_p);
extern void debug_loop_vec (poly_bb_p);
extern void print_pbb_domain (FILE *, poly_bb_p, int);
extern void print_pbb (FILE *, poly_bb_p, int);
extern void print_scop_context (FILE *, scop_p, int);
extern void print_scop (FILE *, scop_p, int);
extern void debug_pbb_domain (poly_bb_p, int);
extern void debug_pbb (poly_bb_p, int);
extern void print_pdrs (FILE *, poly_bb_p, int);
extern void debug_pdrs (poly_bb_p, int);
extern void debug_scop_context (scop_p, int);
extern void debug_scop (scop_p, int);
extern void print_scop_params (FILE *, scop_p, int);
extern void debug_scop_params (scop_p, int);
extern void print_iteration_domain (FILE *, poly_bb_p, int);
extern void print_iteration_domains (FILE *, scop_p, int);
extern void debug_iteration_domain (poly_bb_p, int);
extern void debug_iteration_domains (scop_p, int);
extern void print_isl_set (FILE *, isl_set *);
extern void print_isl_map (FILE *, isl_map *);
extern void print_isl_aff (FILE *, isl_aff *);
extern void print_isl_constraint (FILE *, isl_constraint *);
extern void debug_isl_set (isl_set *);
extern void debug_isl_map (isl_map *);
extern void debug_isl_aff (isl_aff *);
extern void debug_isl_constraint (isl_constraint *);
extern int scop_do_interchange (scop_p);
extern int scop_do_strip_mine (scop_p, int);
extern bool scop_do_block (scop_p);
extern bool flatten_all_loops (scop_p);
extern bool optimize_isl (scop_p);
extern void pbb_number_of_iterations_at_time (poly_bb_p, graphite_dim_t, mpz_t);
extern void debug_gmp_value (mpz_t);

/* Returns a gimple_bb from BB.  */

static inline gimple_bb_p
gbb_from_bb (basic_block bb)
{
  return (gimple_bb_p) bb->aux;
}

/* The poly_bb of the BB.  */

static inline poly_bb_p
pbb_from_bb (basic_block bb)
{
  return GBB_PBB (gbb_from_bb (bb));
}

/* The basic block of the PBB.  */

static inline basic_block
pbb_bb (poly_bb_p pbb)
{
  return GBB_BB (PBB_BLACK_BOX (pbb));
}

static inline int
pbb_index (poly_bb_p pbb)
{
  return pbb_bb (pbb)->index;
}

/* The loop of the PBB.  */

static inline loop_p
pbb_loop (poly_bb_p pbb)
{
  return gbb_loop (PBB_BLACK_BOX (pbb));
}

/* The scop that contains the PDR.  */

static inline scop_p
pdr_scop (poly_dr_p pdr)
{
  return PBB_SCOP (PDR_PBB (pdr));
}

/* Set black box of PBB to BLACKBOX.  */

static inline void
pbb_set_black_box (poly_bb_p pbb, void *black_box)
{
  pbb->black_box = black_box;
}

/* A SCOP is a Static Control Part of the program, simple enough to be
   represented in polyhedral form.  */
struct scop
{
  /* A SCOP is defined as a SESE region.  */
  sese region;

  /* Number of parameters in SCoP.  */
  graphite_dim_t nb_params;

  /* All the basic blocks in this scop that contain memory references
     and that will be represented as statements in the polyhedral
     representation.  */
  vec<poly_bb_p> bbs;

  /* The context describes known restrictions concerning the parameters
     and relations in between the parameters.

  void f (int8_t a, uint_16_t b) {
    c = 2 a + b;
    ...
  }

  Here we can add these restrictions to the context:

  -128 >= a >= 127
     0 >= b >= 65,535
     c = 2a + b  */
  isl_set *context;

  /* The context used internally by ISL.  */
  isl_ctx *ctx;

  /* The original dependence relations:
     RAW are read after write dependences,
     WAR are write after read dependences,
     WAW are write after write dependences.  */
  isl_union_map *must_raw, *may_raw, *must_raw_no_source, *may_raw_no_source,
    *must_war, *may_war, *must_war_no_source, *may_war_no_source,
    *must_waw, *may_waw, *must_waw_no_source, *may_waw_no_source;

  /* True when the scop has been converted to its polyhedral
     representation.  */
  bool poly_scop_p;
};

#define SCOP_BBS(S) (S->bbs)
#define SCOP_REGION(S) (S->region)
#define SCOP_CONTEXT(S) (NULL)
#define POLY_SCOP_P(S) (S->poly_scop_p)

extern scop_p new_scop (sese);
extern void free_scop (scop_p);
extern void free_scops (vec<scop_p> );
extern void print_generated_program (FILE *, scop_p);
extern void debug_generated_program (scop_p);
extern int unify_scattering_dimensions (scop_p);
extern bool apply_poly_transforms (scop_p);
extern bool graphite_legal_transform (scop_p);

/* Set the region of SCOP to REGION.  */

static inline void
scop_set_region (scop_p scop, sese region)
{
  scop->region = region;
}

/* Returns the number of parameters for SCOP.  */

static inline graphite_dim_t
scop_nb_params (scop_p scop)
{
  return scop->nb_params;
}

/* Set the number of params of SCOP to NB_PARAMS.  */

static inline void
scop_set_nb_params (scop_p scop, graphite_dim_t nb_params)
{
  scop->nb_params = nb_params;
}

bool graphite_legal_transform (scop_p);
__isl_give isl_union_map *extend_schedule (__isl_take isl_union_map *);

void
compute_deps (scop_p scop, vec<poly_bb_p> pbbs,
	      isl_union_map **must_raw,
	      isl_union_map **may_raw,
	      isl_union_map **must_raw_no_source,
	      isl_union_map **may_raw_no_source,
	      isl_union_map **must_war,
	      isl_union_map **may_war,
	      isl_union_map **must_war_no_source,
	      isl_union_map **may_war_no_source,
	      isl_union_map **must_waw,
	      isl_union_map **may_waw,
	      isl_union_map **must_waw_no_source,
	      isl_union_map **may_waw_no_source);

isl_union_map *
scop_get_dependences (scop_p scop);

bool
carries_deps (__isl_keep isl_union_map *schedule,
	      __isl_keep isl_union_map *deps,
	      int depth);

#endif