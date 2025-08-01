/* Default target hook functions.
   Copyright (C) 2003-2025 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* The migration of target macros to target hooks works as follows:

   1. Create a target hook that uses the existing target macros to
      implement the same functionality.

   2. Convert all the MI files to use the hook instead of the macro.

   3. Repeat for a majority of the remaining target macros.  This will
      take some time.

   4. Tell target maintainers to start migrating.

   5. Eventually convert the backends to override the hook instead of
      defining the macros.  This will take some time too.

   6. TBD when, poison the macros.  Unmigrated targets will break at
      this point.

   Note that we expect steps 1-3 to be done by the people that
   understand what the MI does with each macro, and step 5 to be done
   by the target maintainers for their respective targets.

   Note that steps 1 and 2 don't have to be done together, but no
   target can override the new hook until step 2 is complete for it.

   Once the macros are poisoned, we will revert to the old migration
   rules - migrate the macro, callers, and targets all at once.  This
   comment can thus be removed at that point.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "target.h"
#include "function.h"
#include "rtl.h"
#include "tree.h"
#include "tree-ssa-alias.h"
#include "gimple-expr.h"
#include "memmodel.h"
#include "backend.h"
#include "emit-rtl.h"
#include "df.h"
#include "tm_p.h"
#include "stringpool.h"
#include "tree-vrp.h"
#include "tree-ssanames.h"
#include "profile-count.h"
#include "optabs.h"
#include "regs.h"
#include "recog.h"
#include "diagnostic-core.h"
#include "fold-const.h"
#include "stor-layout.h"
#include "varasm.h"
#include "flags.h"
#include "explow.h"
#include "expmed.h"
#include "calls.h"
#include "expr.h"
#include "output.h"
#include "common/common-target.h"
#include "reload.h"
#include "intl.h"
#include "opts.h"
#include "gimplify.h"
#include "predict.h"
#include "real.h"
#include "langhooks.h"
#include "sbitmap.h"
#include "function-abi.h"
#include "attribs.h"
#include "asan.h"
#include "emit-rtl.h"
#include "gimple.h"
#include "cfgloop.h"
#include "tree-vectorizer.h"
#include "options.h"
#include "case-cfn-macros.h"
#include "avoid-store-forwarding.h"

bool
default_legitimate_address_p (machine_mode mode ATTRIBUTE_UNUSED,
			      rtx addr ATTRIBUTE_UNUSED,
			      bool strict ATTRIBUTE_UNUSED,
			      code_helper ATTRIBUTE_UNUSED)
{
#ifdef GO_IF_LEGITIMATE_ADDRESS
  /* Defer to the old implementation using a goto.  */
  if (strict)
    return strict_memory_address_p (mode, addr);
  else
    return memory_address_p (mode, addr);
#else
  gcc_unreachable ();
#endif
}

void
default_external_libcall (rtx fun ATTRIBUTE_UNUSED)
{
#ifdef ASM_OUTPUT_EXTERNAL_LIBCALL
  ASM_OUTPUT_EXTERNAL_LIBCALL (asm_out_file, fun);
#endif
}

int
default_unspec_may_trap_p (const_rtx x, unsigned flags)
{
  int i;

  /* Any floating arithmetic may trap.  */
  if ((SCALAR_FLOAT_MODE_P (GET_MODE (x)) && flag_trapping_math))
    return 1;

  for (i = 0; i < XVECLEN (x, 0); ++i)
    {
      if (may_trap_p_1 (XVECEXP (x, 0, i), flags))
	return 1;
    }

  return 0;
}

machine_mode
default_promote_function_mode (const_tree type ATTRIBUTE_UNUSED,
			       machine_mode mode,
			       int *punsignedp ATTRIBUTE_UNUSED,
			       const_tree funtype ATTRIBUTE_UNUSED,
			       int for_return ATTRIBUTE_UNUSED)
{
  if (type != NULL_TREE && for_return == 2)
    return promote_mode (type, mode, punsignedp);
  return mode;
}

machine_mode
default_promote_function_mode_always_promote (const_tree type,
					      machine_mode mode,
					      int *punsignedp,
					      const_tree funtype ATTRIBUTE_UNUSED,
					      int for_return ATTRIBUTE_UNUSED)
{
  return promote_mode (type, mode, punsignedp);
}

machine_mode
default_cc_modes_compatible (machine_mode m1, machine_mode m2)
{
  if (m1 == m2)
    return m1;
  return VOIDmode;
}

bool
default_return_in_memory (const_tree type,
			  const_tree fntype ATTRIBUTE_UNUSED)
{
  return (TYPE_MODE (type) == BLKmode);
}

rtx
default_legitimize_address (rtx x, rtx orig_x ATTRIBUTE_UNUSED,
			    machine_mode mode ATTRIBUTE_UNUSED)
{
  return x;
}

bool
default_legitimize_address_displacement (rtx *, rtx *, poly_int64,
					 machine_mode)
{
  return false;
}

bool
default_const_not_ok_for_debug_p (rtx x)
{
  if (GET_CODE (x) == UNSPEC)
    return true;
  return false;
}

rtx
default_expand_builtin_saveregs (void)
{
  error ("%<__builtin_saveregs%> not supported by this target");
  return const0_rtx;
}

void
default_setup_incoming_varargs (cumulative_args_t,
				const function_arg_info &, int *, int)
{
}

/* The default implementation of TARGET_BUILTIN_SETJMP_FRAME_VALUE.  */

rtx
default_builtin_setjmp_frame_value (void)
{
  return virtual_stack_vars_rtx;
}

/* Generic hook that takes a CUMULATIVE_ARGS pointer and returns false.  */

bool
hook_bool_CUMULATIVE_ARGS_false (cumulative_args_t ca ATTRIBUTE_UNUSED)
{
  return false;
}

bool
default_pretend_outgoing_varargs_named (cumulative_args_t ca ATTRIBUTE_UNUSED)
{
  return (targetm.calls.setup_incoming_varargs
	  != default_setup_incoming_varargs);
}

scalar_int_mode
default_eh_return_filter_mode (void)
{
  return targetm.unwind_word_mode ();
}

scalar_int_mode
default_libgcc_cmp_return_mode (void)
{
  return word_mode;
}

scalar_int_mode
default_libgcc_shift_count_mode (void)
{
  return word_mode;
}

scalar_int_mode
default_unwind_word_mode (void)
{
  return word_mode;
}

/* The default implementation of TARGET_SHIFT_TRUNCATION_MASK.  */

unsigned HOST_WIDE_INT
default_shift_truncation_mask (machine_mode mode)
{
  return SHIFT_COUNT_TRUNCATED ? GET_MODE_UNIT_BITSIZE (mode) - 1 : 0;
}

/* The default implementation of TARGET_MIN_DIVISIONS_FOR_RECIP_MUL.  */

unsigned int
default_min_divisions_for_recip_mul (machine_mode mode ATTRIBUTE_UNUSED)
{
  return have_insn_for (DIV, mode) ? 3 : 2;
}

/* The default implementation of TARGET_MODE_REP_EXTENDED.  */

int
default_mode_rep_extended (scalar_int_mode, scalar_int_mode)
{
  return UNKNOWN;
}

/* Generic hook that takes a CUMULATIVE_ARGS pointer and returns true.  */

bool
hook_bool_CUMULATIVE_ARGS_true (cumulative_args_t a ATTRIBUTE_UNUSED)
{
  return true;
}

/* Return machine mode for non-standard suffix
   or VOIDmode if non-standard suffixes are unsupported.  */
machine_mode
default_mode_for_suffix (char suffix ATTRIBUTE_UNUSED)
{
  return VOIDmode;
}

/* Return machine mode for a floating type which is indicated
   by the given enum tree_index.  */

machine_mode
default_mode_for_floating_type (enum tree_index ti)
{
  if (ti == TI_FLOAT_TYPE)
    return SFmode;
  gcc_assert (ti == TI_DOUBLE_TYPE || ti == TI_LONG_DOUBLE_TYPE);
  return DFmode;
}

/* The generic C++ ABI specifies this is a 64-bit value.  */
tree
default_cxx_guard_type (void)
{
  return long_long_integer_type_node;
}

/* Returns the size of the cookie to use when allocating an array
   whose elements have the indicated TYPE.  Assumes that it is already
   known that a cookie is needed.  */

tree
default_cxx_get_cookie_size (tree type)
{
  tree cookie_size;

  /* We need to allocate an additional max (sizeof (size_t), alignof
     (true_type)) bytes.  */
  tree sizetype_size;
  tree type_align;

  sizetype_size = size_in_bytes (sizetype);
  type_align = size_int (TYPE_ALIGN_UNIT (type));
  if (tree_int_cst_lt (type_align, sizetype_size))
    cookie_size = sizetype_size;
  else
    cookie_size = type_align;

  return cookie_size;
}

/* Returns modified FUNCTION_TYPE for cdtor callabi.  */

tree
default_cxx_adjust_cdtor_callabi_fntype (tree fntype)
{
  return fntype;
}

/* Return true if a parameter must be passed by reference.  This version
   of the TARGET_PASS_BY_REFERENCE hook uses just MUST_PASS_IN_STACK.  */

bool
hook_pass_by_reference_must_pass_in_stack (cumulative_args_t,
					   const function_arg_info &arg)
{
  return targetm.calls.must_pass_in_stack (arg);
}

/* Return true if a parameter follows callee copies conventions.  This
   version of the hook is true for all named arguments.  */

bool
hook_callee_copies_named (cumulative_args_t, const function_arg_info &arg)
{
  return arg.named;
}

/* Emit to STREAM the assembler syntax for insn operand X.  */

void
default_print_operand (FILE *stream ATTRIBUTE_UNUSED, rtx x ATTRIBUTE_UNUSED,
		       int code ATTRIBUTE_UNUSED)
{
#ifdef PRINT_OPERAND
  PRINT_OPERAND (stream, x, code);
#else
  gcc_unreachable ();
#endif
}

/* Emit to STREAM the assembler syntax for an insn operand whose memory
   address is X.  */

void
default_print_operand_address (FILE *stream ATTRIBUTE_UNUSED,
			       machine_mode /*mode*/,
			       rtx x ATTRIBUTE_UNUSED)
{
#ifdef PRINT_OPERAND_ADDRESS
  PRINT_OPERAND_ADDRESS (stream, x);
#else
  gcc_unreachable ();
#endif
}

/* Return true if CODE is a valid punctuation character for the
   `print_operand' hook.  */

bool
default_print_operand_punct_valid_p (unsigned char code ATTRIBUTE_UNUSED)
{
#ifdef PRINT_OPERAND_PUNCT_VALID_P
  return PRINT_OPERAND_PUNCT_VALID_P (code);
#else
  return false;
#endif
}

/* The default implementation of TARGET_MANGLE_ASSEMBLER_NAME.  */
tree
default_mangle_assembler_name (const char *name ATTRIBUTE_UNUSED)
{
  const char *skipped = name + (*name == '*' ? 1 : 0);
  const char *stripped = targetm.strip_name_encoding (skipped);
  if (*name != '*' && user_label_prefix[0])
    stripped = ACONCAT ((user_label_prefix, stripped, NULL));
  return get_identifier (stripped);
}

/* The default implementation of TARGET_TRANSLATE_MODE_ATTRIBUTE.  */

machine_mode
default_translate_mode_attribute (machine_mode mode)
{
  return mode;
}

/* True if MODE is valid for the target.  By "valid", we mean able to
   be manipulated in non-trivial ways.  In particular, this means all
   the arithmetic is supported.

   By default we guess this means that any C type is supported.  If
   we can't map the mode back to a type that would be available in C,
   then reject it.  Special case, here, is the double-word arithmetic
   supported by optabs.cc.  */

bool
default_scalar_mode_supported_p (scalar_mode mode)
{
  int precision = GET_MODE_PRECISION (mode);

  switch (GET_MODE_CLASS (mode))
    {
    case MODE_PARTIAL_INT:
    case MODE_INT:
      if (precision == CHAR_TYPE_SIZE)
	return true;
      if (precision == SHORT_TYPE_SIZE)
	return true;
      if (precision == INT_TYPE_SIZE)
	return true;
      if (precision == LONG_TYPE_SIZE)
	return true;
      if (precision == LONG_LONG_TYPE_SIZE)
	return true;
      if (precision == 2 * BITS_PER_WORD)
	return true;
      return false;

    case MODE_FLOAT:
      if (mode == targetm.c.mode_for_floating_type (TI_FLOAT_TYPE))
	return true;
      if (mode == targetm.c.mode_for_floating_type (TI_DOUBLE_TYPE))
	return true;
      if (mode == targetm.c.mode_for_floating_type (TI_LONG_DOUBLE_TYPE))
	return true;
      return false;

    case MODE_DECIMAL_FLOAT:
    case MODE_FRACT:
    case MODE_UFRACT:
    case MODE_ACCUM:
    case MODE_UACCUM:
      return false;

    default:
      gcc_unreachable ();
    }
}

/* Return true if libgcc supports floating-point mode MODE (known to
   be supported as a scalar mode).  */

bool
default_libgcc_floating_mode_supported_p (scalar_float_mode mode)
{
  switch (mode)
    {
#ifdef HAVE_SFmode
    case E_SFmode:
#endif
#ifdef HAVE_DFmode
    case E_DFmode:
#endif
#ifdef HAVE_XFmode
    case E_XFmode:
#endif
#ifdef HAVE_TFmode
    case E_TFmode:
#endif
      return true;

    default:
      return false;
    }
}

/* Return the machine mode to use for the type _FloatN, if EXTENDED is
   false, or _FloatNx, if EXTENDED is true, or VOIDmode if not
   supported.  */
opt_scalar_float_mode
default_floatn_mode (int n, bool extended)
{
  if (extended)
    {
      opt_scalar_float_mode cand1, cand2;
      scalar_float_mode mode;
      switch (n)
	{
	case 32:
#ifdef HAVE_DFmode
	  cand1 = DFmode;
#endif
	  break;

	case 64:
#ifdef HAVE_XFmode
	  cand1 = XFmode;
#endif
#ifdef HAVE_TFmode
	  cand2 = TFmode;
#endif
	  break;

	case 128:
	  break;

	default:
	  /* Those are the only valid _FloatNx types.  */
	  gcc_unreachable ();
	}
      if (cand1.exists (&mode)
	  && REAL_MODE_FORMAT (mode)->ieee_bits > n
	  && targetm.scalar_mode_supported_p (mode)
	  && targetm.libgcc_floating_mode_supported_p (mode))
	return cand1;
      if (cand2.exists (&mode)
	  && REAL_MODE_FORMAT (mode)->ieee_bits > n
	  && targetm.scalar_mode_supported_p (mode)
	  && targetm.libgcc_floating_mode_supported_p (mode))
	return cand2;
    }
  else
    {
      opt_scalar_float_mode cand;
      scalar_float_mode mode;
      switch (n)
	{
	case 16:
	  /* Always enable _Float16 if we have basic support for the mode.
	     Targets can control the range and precision of operations on
	     the _Float16 type using TARGET_C_EXCESS_PRECISION.  */
#ifdef HAVE_HFmode
	  cand = HFmode;
#endif
	  break;

	case 32:
#ifdef HAVE_SFmode
	  cand = SFmode;
#endif
	  break;

	case 64:
#ifdef HAVE_DFmode
	  cand = DFmode;
#endif
	  break;

	case 128:
#ifdef HAVE_TFmode
	  cand = TFmode;
#endif
	  break;

	default:
	  break;
	}
      if (cand.exists (&mode)
	  && REAL_MODE_FORMAT (mode)->ieee_bits == n
	  && targetm.scalar_mode_supported_p (mode)
	  && targetm.libgcc_floating_mode_supported_p (mode))
	return cand;
    }
  return opt_scalar_float_mode ();
}

/* Define this to return true if the _Floatn and _Floatnx built-in functions
   should implicitly enable the built-in function without the __builtin_ prefix
   in addition to the normal built-in function with the __builtin_ prefix.  The
   default is to only enable built-in functions without the __builtin_ prefix
   for the GNU C langauge.  The argument FUNC is the enum builtin_in_function
   id of the function to be enabled.  */

bool
default_floatn_builtin_p (int func ATTRIBUTE_UNUSED)
{
  static bool first_time_p = true;
  static bool c_or_objective_c;

  if (first_time_p)
    {
      first_time_p = false;
      c_or_objective_c = lang_GNU_C () || lang_GNU_OBJC ();
    }

  return c_or_objective_c;
}

/* Make some target macros useable by target-independent code.  */
bool
targhook_words_big_endian (void)
{
  return !!WORDS_BIG_ENDIAN;
}

bool
targhook_float_words_big_endian (void)
{
  return !!FLOAT_WORDS_BIG_ENDIAN;
}

/* True if the target supports floating-point exceptions and rounding
   modes.  */

bool
default_float_exceptions_rounding_supported_p (void)
{
#ifdef HAVE_adddf3
  return HAVE_adddf3;
#else
  return false;
#endif
}

/* True if the target supports decimal floating point.  */

bool
default_decimal_float_supported_p (void)
{
  return ENABLE_DECIMAL_FLOAT;
}

/* True if the target supports fixed-point arithmetic.  */

bool
default_fixed_point_supported_p (void)
{
  return ENABLE_FIXED_POINT;
}

/* True if the target supports GNU indirect functions.  */

bool
default_has_ifunc_p (void)
{
  return HAVE_GNU_INDIRECT_FUNCTION;
}

/* Return true if we predict the loop LOOP will be transformed to a
   low-overhead loop, otherwise return false.

   By default, false is returned, as this hook's applicability should be
   verified for each target.  Target maintainers should re-define the hook
   if the target can take advantage of it.  */

bool
default_predict_doloop_p (class loop *loop ATTRIBUTE_UNUSED)
{
  return false;
}

/* By default, just use the input MODE itself.  */

machine_mode
default_preferred_doloop_mode (machine_mode mode)
{
  return mode;
}

/* NULL if INSN insn is valid within a low-overhead loop, otherwise returns
   an error message.

   This function checks whether a given INSN is valid within a low-overhead
   loop.  If INSN is invalid it returns the reason for that, otherwise it
   returns NULL. A called function may clobber any special registers required
   for low-overhead looping. Additionally, some targets (eg, PPC) use the count
   register for branch on table instructions. We reject the doloop pattern in
   these cases.  */

const char *
default_invalid_within_doloop (const rtx_insn *insn)
{
  if (CALL_P (insn))
    return "Function call in loop.";

  if (tablejump_p (insn, NULL, NULL) || computed_jump_p (insn))
    return "Computed branch in the loop.";

  return NULL;
}

/* Mapping of builtin functions to vectorized variants.  */

tree
default_builtin_vectorized_function (unsigned int, tree, tree)
{
  return NULL_TREE;
}

/* Mapping of target builtin functions to vectorized variants.  */

tree
default_builtin_md_vectorized_function (tree, tree, tree)
{
  return NULL_TREE;
}

/* Default vectorizer cost model values.  */

int
default_builtin_vectorization_cost (enum vect_cost_for_stmt type_of_cost,
                                    tree vectype,
                                    int misalign ATTRIBUTE_UNUSED)
{
  switch (type_of_cost)
    {
      case scalar_stmt:
      case scalar_load:
      case scalar_store:
      case vector_stmt:
      case vector_load:
      case vector_store:
      case vec_to_scalar:
      case scalar_to_vec:
      case cond_branch_not_taken:
      case vec_perm:
      case vec_promote_demote:
        return 1;

      case unaligned_load:
      case unaligned_store:
        return 2;

      case cond_branch_taken:
        return 3;

      case vec_construct:
	return estimated_poly_value (TYPE_VECTOR_SUBPARTS (vectype)) - 1;

      default:
        gcc_unreachable ();
    }
}

/* Reciprocal.  */

tree
default_builtin_reciprocal (tree)
{
  return NULL_TREE;
}

void
default_emit_support_tinfos (emit_support_tinfos_callback)
{
}

bool
hook_bool_CUMULATIVE_ARGS_arg_info_false (cumulative_args_t,
					  const function_arg_info &)
{
  return false;
}

bool
hook_bool_CUMULATIVE_ARGS_arg_info_true (cumulative_args_t,
					 const function_arg_info &)
{
  return true;
}

int
hook_int_CUMULATIVE_ARGS_arg_info_0 (cumulative_args_t,
				     const function_arg_info &)
{
  return 0;
}

void
hook_void_CUMULATIVE_ARGS (cumulative_args_t)
{
}

void
hook_void_CUMULATIVE_ARGS_tree (cumulative_args_t ca ATTRIBUTE_UNUSED,
				tree ATTRIBUTE_UNUSED)
{
}

void
hook_void_CUMULATIVE_ARGS_rtx_tree (cumulative_args_t, rtx, tree)
{
}

/* Default implementation of TARGET_PUSH_ARGUMENT.  */

bool
default_push_argument (unsigned int)
{
#ifdef PUSH_ROUNDING
  return !ACCUMULATE_OUTGOING_ARGS;
#else
  return false;
#endif
}

void
default_function_arg_advance (cumulative_args_t, const function_arg_info &)
{
  gcc_unreachable ();
}

/* Default implementation of TARGET_FUNCTION_ARG_OFFSET.  */

HOST_WIDE_INT
default_function_arg_offset (machine_mode, const_tree)
{
  return 0;
}

/* Default implementation of TARGET_FUNCTION_ARG_PADDING: usually pad
   upward, but pad short args downward on big-endian machines.  */

pad_direction
default_function_arg_padding (machine_mode mode, const_tree type)
{
  if (!BYTES_BIG_ENDIAN)
    return PAD_UPWARD;

  unsigned HOST_WIDE_INT size;
  if (mode == BLKmode)
    {
      if (!type || TREE_CODE (TYPE_SIZE (type)) != INTEGER_CST)
	return PAD_UPWARD;
      size = int_size_in_bytes (type);
    }
  else
    /* Targets with variable-sized modes must override this hook
       and handle variable-sized modes explicitly.  */
    size = GET_MODE_SIZE (mode).to_constant ();

  if (size < (PARM_BOUNDARY / BITS_PER_UNIT))
    return PAD_DOWNWARD;

  return PAD_UPWARD;
}

rtx
default_function_arg (cumulative_args_t, const function_arg_info &)
{
  gcc_unreachable ();
}

rtx
default_function_incoming_arg (cumulative_args_t, const function_arg_info &)
{
  gcc_unreachable ();
}

unsigned int
default_function_arg_boundary (machine_mode mode ATTRIBUTE_UNUSED,
			       const_tree type ATTRIBUTE_UNUSED)
{
  return PARM_BOUNDARY;
}

unsigned int
default_function_arg_round_boundary (machine_mode mode ATTRIBUTE_UNUSED,
				     const_tree type ATTRIBUTE_UNUSED)
{
  return PARM_BOUNDARY;
}

void
hook_void_bitmap (bitmap regs ATTRIBUTE_UNUSED)
{
}

const char *
hook_invalid_arg_for_unprototyped_fn (
	const_tree typelist ATTRIBUTE_UNUSED,
	const_tree funcdecl ATTRIBUTE_UNUSED,
	const_tree val ATTRIBUTE_UNUSED)
{
  return NULL;
}

/* Initialize the stack protection decls.  */

/* Stack protection related decls living in libgcc.  */
static GTY(()) tree stack_chk_guard_decl;

tree
default_stack_protect_guard (void)
{
  tree t = stack_chk_guard_decl;

  if (t == NULL)
    {
      rtx x;

      t = build_decl (UNKNOWN_LOCATION,
		      VAR_DECL, get_identifier ("__stack_chk_guard"),
		      ptr_type_node);
      TREE_STATIC (t) = 1;
      TREE_PUBLIC (t) = 1;
      DECL_EXTERNAL (t) = 1;
      TREE_USED (t) = 1;
      TREE_THIS_VOLATILE (t) = 1;
      DECL_ARTIFICIAL (t) = 1;
      DECL_IGNORED_P (t) = 1;

      /* Do not share RTL as the declaration is visible outside of
	 current function.  */
      x = DECL_RTL (t);
      RTX_FLAG (x, used) = 1;

      stack_chk_guard_decl = t;
    }

  return t;
}

static GTY(()) tree stack_chk_fail_decl;

tree
default_external_stack_protect_fail (void)
{
  tree t = stack_chk_fail_decl;

  if (t == NULL_TREE)
    {
      t = build_function_type_list (void_type_node, NULL_TREE);
      t = build_decl (UNKNOWN_LOCATION,
		      FUNCTION_DECL, get_identifier ("__stack_chk_fail"), t);
      TREE_STATIC (t) = 1;
      TREE_PUBLIC (t) = 1;
      DECL_EXTERNAL (t) = 1;
      TREE_USED (t) = 1;
      TREE_THIS_VOLATILE (t) = 1;
      TREE_NOTHROW (t) = 1;
      DECL_ARTIFICIAL (t) = 1;
      DECL_IGNORED_P (t) = 1;
      DECL_VISIBILITY (t) = VISIBILITY_DEFAULT;
      DECL_VISIBILITY_SPECIFIED (t) = 1;

      stack_chk_fail_decl = t;
    }

  return build_call_expr (t, 0);
}

tree
default_hidden_stack_protect_fail (void)
{
#ifndef HAVE_GAS_HIDDEN
  return default_external_stack_protect_fail ();
#else
  tree t = stack_chk_fail_decl;

  if (!flag_pic)
    return default_external_stack_protect_fail ();

  if (t == NULL_TREE)
    {
      t = build_function_type_list (void_type_node, NULL_TREE);
      t = build_decl (UNKNOWN_LOCATION, FUNCTION_DECL,
		      get_identifier ("__stack_chk_fail_local"), t);
      TREE_STATIC (t) = 1;
      TREE_PUBLIC (t) = 1;
      DECL_EXTERNAL (t) = 1;
      TREE_USED (t) = 1;
      TREE_THIS_VOLATILE (t) = 1;
      TREE_NOTHROW (t) = 1;
      DECL_ARTIFICIAL (t) = 1;
      DECL_IGNORED_P (t) = 1;
      DECL_VISIBILITY_SPECIFIED (t) = 1;
      DECL_VISIBILITY (t) = VISIBILITY_HIDDEN;

      stack_chk_fail_decl = t;
    }

  return build_call_expr (t, 0);
#endif
}

bool
hook_bool_const_rtx_commutative_p (const_rtx x,
				   int outer_code ATTRIBUTE_UNUSED)
{
  return COMMUTATIVE_P (x);
}

rtx
default_function_value (const_tree ret_type ATTRIBUTE_UNUSED,
			const_tree fn_decl_or_type,
			bool outgoing ATTRIBUTE_UNUSED)
{
  /* The old interface doesn't handle receiving the function type.  */
  if (fn_decl_or_type
      && !DECL_P (fn_decl_or_type))
    fn_decl_or_type = NULL;

#ifdef FUNCTION_VALUE
  return FUNCTION_VALUE (ret_type, fn_decl_or_type);
#else
  gcc_unreachable ();
#endif
}

rtx
default_libcall_value (machine_mode mode ATTRIBUTE_UNUSED,
		       const_rtx fun ATTRIBUTE_UNUSED)
{
#ifdef LIBCALL_VALUE
  return LIBCALL_VALUE (MACRO_MODE (mode));
#else
  gcc_unreachable ();
#endif
}

/* The default hook for TARGET_FUNCTION_VALUE_REGNO_P.  */

bool
default_function_value_regno_p (const unsigned int regno ATTRIBUTE_UNUSED)
{
#ifdef FUNCTION_VALUE_REGNO_P
  return FUNCTION_VALUE_REGNO_P (regno);
#else
  gcc_unreachable ();
#endif
}

/* Choose the mode and rtx to use to zero REGNO, storing tem in PMODE and
   PREGNO_RTX and returning TRUE if successful, otherwise returning FALSE.  If
   the natural mode for REGNO doesn't work, attempt to group it with subsequent
   adjacent registers set in TOZERO.  */

static inline bool
zcur_select_mode_rtx (unsigned int regno, machine_mode *pmode,
		      rtx *pregno_rtx, HARD_REG_SET tozero)
{
  rtx regno_rtx = regno_reg_rtx[regno];
  machine_mode mode = GET_MODE (regno_rtx);

  /* If the natural mode doesn't work, try some wider mode.  */
  if (!targetm.hard_regno_mode_ok (regno, mode))
    {
      bool found = false;
      for (int nregs = 2;
	   !found && nregs <= hard_regno_max_nregs
	     && regno + nregs <= FIRST_PSEUDO_REGISTER
	     && TEST_HARD_REG_BIT (tozero,
				   regno + nregs - 1);
	   nregs++)
	{
	  mode = choose_hard_reg_mode (regno, nregs, 0);
	  if (mode == E_VOIDmode)
	    continue;
	  gcc_checking_assert (targetm.hard_regno_mode_ok (regno, mode));
	  regno_rtx = gen_rtx_REG (mode, regno);
	  found = true;
	}
      if (!found)
	return false;
    }

  *pmode = mode;
  *pregno_rtx = regno_rtx;
  return true;
}

/* The default hook for TARGET_ZERO_CALL_USED_REGS.  */

HARD_REG_SET
default_zero_call_used_regs (HARD_REG_SET need_zeroed_hardregs)
{
  gcc_assert (!hard_reg_set_empty_p (need_zeroed_hardregs));

  HARD_REG_SET failed;
  CLEAR_HARD_REG_SET (failed);
  bool progress = false;

  /* First, try to zero each register in need_zeroed_hardregs by
     loading a zero into it, taking note of any failures in
     FAILED.  */
  for (unsigned int regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    if (TEST_HARD_REG_BIT (need_zeroed_hardregs, regno))
      {
	rtx_insn *last_insn = get_last_insn ();
	rtx regno_rtx;
	machine_mode mode;

	if (!zcur_select_mode_rtx (regno, &mode, &regno_rtx,
				   need_zeroed_hardregs))
	  {
	    SET_HARD_REG_BIT (failed, regno);
	    continue;
	  }

	rtx zero = CONST0_RTX (mode);
	rtx_insn *insn = emit_move_insn (regno_rtx, zero);
	if (!valid_insn_p (insn))
	  {
	    SET_HARD_REG_BIT (failed, regno);
	    delete_insns_since (last_insn);
	  }
	else
	  {
	    progress = true;
	    regno += hard_regno_nregs (regno, mode) - 1;
	  }
      }

  /* Now retry with copies from zeroed registers, as long as we've
     made some PROGRESS, and registers remain to be zeroed in
     FAILED.  */
  while (progress && !hard_reg_set_empty_p (failed))
    {
      HARD_REG_SET retrying = failed;

      CLEAR_HARD_REG_SET (failed);
      progress = false;

      for (unsigned int regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
	if (TEST_HARD_REG_BIT (retrying, regno))
	  {
	    rtx regno_rtx;
	    machine_mode mode;

	    /* This might select registers we've already zeroed.  If grouping
	       with them is what it takes to get regno zeroed, so be it.  */
	    if (!zcur_select_mode_rtx (regno, &mode, &regno_rtx,
				       need_zeroed_hardregs))
	      {
		SET_HARD_REG_BIT (failed, regno);
		continue;
	      }

	    bool success = false;
	    /* Look for a source.  */
	    for (unsigned int src = 0; src < FIRST_PSEUDO_REGISTER; src++)
	      {
		/* If SRC hasn't been zeroed (yet?), skip it.  */
		if (! TEST_HARD_REG_BIT (need_zeroed_hardregs, src))
		  continue;
		if (TEST_HARD_REG_BIT (retrying, src))
		  continue;

		/* Check that SRC can hold MODE, and that any other
		   registers needed to hold MODE in SRC have also been
		   zeroed.  */
		if (!targetm.hard_regno_mode_ok (src, mode))
		  continue;
		unsigned n = targetm.hard_regno_nregs (src, mode);
		bool ok = true;
		for (unsigned i = 1; ok && i < n; i++)
		  ok = (TEST_HARD_REG_BIT (need_zeroed_hardregs, src + i)
			&& !TEST_HARD_REG_BIT (retrying, src + i));
		if (!ok)
		  continue;

		/* SRC is usable, try to copy from it.  */
		rtx_insn *last_insn = get_last_insn ();
		rtx src_rtx = gen_rtx_REG (mode, src);
		rtx_insn *insn = emit_move_insn (regno_rtx, src_rtx);
		if (!valid_insn_p (insn))
		  /* It didn't work, remove any inserts.  We'll look
		     for another SRC.  */
		  delete_insns_since (last_insn);
		else
		  {
		    /* We're done for REGNO.  */
		    success = true;
		    break;
		  }
	      }

	    /* If nothing worked for REGNO this round, mark it to be
	       retried if we get another round.  */
	    if (!success)
	      SET_HARD_REG_BIT (failed, regno);
	    else
	      {
		/* Take note so as to enable another round if needed.  */
		progress = true;
		regno += hard_regno_nregs (regno, mode) - 1;
	      }
	  }
    }

  /* If any register remained, report it.  */
  if (!progress)
    {
      static bool issued_error;
      if (!issued_error)
	{
	  const char *name = NULL;
	  for (unsigned int i = 0; zero_call_used_regs_opts[i].name != NULL;
	       ++i)
	    if (flag_zero_call_used_regs == zero_call_used_regs_opts[i].flag)
	      {
		name = zero_call_used_regs_opts[i].name;
		break;
	      }

	  if (!name)
	    name = "";

	  issued_error = true;
	  sorry ("argument %qs is not supported for %qs on this target",
		 name, "-fzero-call-used-regs");
	}
    }

  return need_zeroed_hardregs;
}

rtx
default_internal_arg_pointer (void)
{
  /* If the reg that the virtual arg pointer will be translated into is
     not a fixed reg or is the stack pointer, make a copy of the virtual
     arg pointer, and address parms via the copy.  The frame pointer is
     considered fixed even though it is not marked as such.  */
  if ((ARG_POINTER_REGNUM == STACK_POINTER_REGNUM
       || ! (fixed_regs[ARG_POINTER_REGNUM]
	     || ARG_POINTER_REGNUM == FRAME_POINTER_REGNUM)))
    return copy_to_reg (virtual_incoming_args_rtx);
  else
    return virtual_incoming_args_rtx;
}

rtx
default_static_chain (const_tree ARG_UNUSED (fndecl_or_type), bool incoming_p)
{
  if (incoming_p)
    {
#ifdef STATIC_CHAIN_INCOMING_REGNUM
      return gen_rtx_REG (Pmode, STATIC_CHAIN_INCOMING_REGNUM);
#endif
    }

#ifdef STATIC_CHAIN_REGNUM
  return gen_rtx_REG (Pmode, STATIC_CHAIN_REGNUM);
#endif

  {
    static bool issued_error;
    if (!issued_error)
      {
	issued_error = true;
	sorry ("nested functions not supported on this target");
      }

    /* It really doesn't matter what we return here, so long at it
       doesn't cause the rest of the compiler to crash.  */
    return gen_rtx_MEM (Pmode, stack_pointer_rtx);
  }
}

void
default_trampoline_init (rtx ARG_UNUSED (m_tramp), tree ARG_UNUSED (t_func),
			 rtx ARG_UNUSED (r_chain))
{
  sorry ("nested function trampolines not supported on this target");
}

poly_int64
default_return_pops_args (tree, tree, poly_int64)
{
  return 0;
}

reg_class_t
default_ira_change_pseudo_allocno_class (int regno ATTRIBUTE_UNUSED,
					 reg_class_t cl,
					 reg_class_t best_cl ATTRIBUTE_UNUSED)
{
  return cl;
}

int
default_ira_callee_saved_register_cost_scale (int)
{
  return (optimize_size
	  ? 1
	  : REG_FREQ_FROM_BB (ENTRY_BLOCK_PTR_FOR_FN (cfun)));
}

extern bool
default_lra_p (void)
{
  return true;
}

int
default_register_priority (int hard_regno ATTRIBUTE_UNUSED)
{
  return 0;
}

extern bool
default_register_usage_leveling_p (void)
{
  return false;
}

extern bool
default_different_addr_displacement_p (void)
{
  return false;
}

reg_class_t
default_secondary_reload (bool in_p ATTRIBUTE_UNUSED, rtx x ATTRIBUTE_UNUSED,
			  reg_class_t reload_class_i ATTRIBUTE_UNUSED,
			  machine_mode reload_mode ATTRIBUTE_UNUSED,
			  secondary_reload_info *sri)
{
  enum reg_class rclass = NO_REGS;
  enum reg_class reload_class = (enum reg_class) reload_class_i;

  if (sri->prev_sri && sri->prev_sri->t_icode != CODE_FOR_nothing)
    {
      sri->icode = sri->prev_sri->t_icode;
      return NO_REGS;
    }
#ifdef SECONDARY_INPUT_RELOAD_CLASS
  if (in_p)
    rclass = SECONDARY_INPUT_RELOAD_CLASS (reload_class,
					   MACRO_MODE (reload_mode), x);
#endif
#ifdef SECONDARY_OUTPUT_RELOAD_CLASS
  if (! in_p)
    rclass = SECONDARY_OUTPUT_RELOAD_CLASS (reload_class,
					    MACRO_MODE (reload_mode), x);
#endif
  if (rclass != NO_REGS)
    {
      enum insn_code icode
	= direct_optab_handler (in_p ? reload_in_optab : reload_out_optab,
				reload_mode);

      if (icode != CODE_FOR_nothing
	  && !insn_operand_matches (icode, in_p, x))
	icode = CODE_FOR_nothing;
      else if (icode != CODE_FOR_nothing)
	{
	  const char *insn_constraint, *scratch_constraint;
	  enum reg_class insn_class, scratch_class;

	  gcc_assert (insn_data[(int) icode].n_operands == 3);
	  insn_constraint = insn_data[(int) icode].operand[!in_p].constraint;
	  if (!*insn_constraint)
	    insn_class = ALL_REGS;
	  else
	    {
	      if (in_p)
		{
		  gcc_assert (*insn_constraint == '=');
		  insn_constraint++;
		}
	      insn_class = (reg_class_for_constraint
			    (lookup_constraint (insn_constraint)));
	      gcc_assert (insn_class != NO_REGS);
	    }

	  scratch_constraint = insn_data[(int) icode].operand[2].constraint;
	  /* The scratch register's constraint must start with "=&",
	     except for an input reload, where only "=" is necessary,
	     and where it might be beneficial to re-use registers from
	     the input.  */
	  gcc_assert (scratch_constraint[0] == '='
		      && (in_p || scratch_constraint[1] == '&'));
	  scratch_constraint++;
	  if (*scratch_constraint == '&')
	    scratch_constraint++;
	  scratch_class = (reg_class_for_constraint
			   (lookup_constraint (scratch_constraint)));

	  if (reg_class_subset_p (reload_class, insn_class))
	    {
	      gcc_assert (scratch_class == rclass);
	      rclass = NO_REGS;
	    }
	  else
	    rclass = insn_class;

        }
      if (rclass == NO_REGS)
	sri->icode = icode;
      else
	sri->t_icode = icode;
    }
  return rclass;
}

/* The default implementation of TARGET_SECONDARY_MEMORY_NEEDED_MODE.  */

machine_mode
default_secondary_memory_needed_mode (machine_mode mode)
{
  if (!targetm.lra_p ()
      && known_lt (GET_MODE_BITSIZE (mode), BITS_PER_WORD)
      && INTEGRAL_MODE_P (mode))
    return mode_for_size (BITS_PER_WORD, GET_MODE_CLASS (mode), 0).require ();
  return mode;
}

/* By default, if flag_pic is true, then neither local nor global relocs
   should be placed in readonly memory.  */

int
default_reloc_rw_mask (void)
{
  return flag_pic ? 3 : 0;
}

/* By default, address diff vectors are generated
for jump tables when flag_pic is true.  */

bool
default_generate_pic_addr_diff_vec (void)
{
  return flag_pic;
}

/* Record an element in the table of global constructors.  SYMBOL is
   a SYMBOL_REF of the function to be called; PRIORITY is a number
   between 0 and MAX_INIT_PRIORITY.  */

void
default_asm_out_constructor (rtx symbol ATTRIBUTE_UNUSED,
			     int priority ATTRIBUTE_UNUSED)
{
  sorry ("global constructors not supported on this target");
}

/* Likewise for global destructors.  */

void
default_asm_out_destructor (rtx symbol ATTRIBUTE_UNUSED,
			    int priority ATTRIBUTE_UNUSED)
{
  sorry ("global destructors not supported on this target");
}

/* By default, do no modification. */
tree default_mangle_decl_assembler_name (tree decl ATTRIBUTE_UNUSED,
					 tree id)
{
   return id;
}

/* The default implementation of TARGET_STATIC_RTX_ALIGNMENT.  */

HOST_WIDE_INT
default_static_rtx_alignment (machine_mode mode)
{
  return GET_MODE_ALIGNMENT (mode);
}

/* The default implementation of TARGET_CONSTANT_ALIGNMENT.  */

HOST_WIDE_INT
default_constant_alignment (const_tree, HOST_WIDE_INT align)
{
  return align;
}

/* An implementation of TARGET_CONSTANT_ALIGNMENT that aligns strings
   to at least BITS_PER_WORD but otherwise makes no changes.  */

HOST_WIDE_INT
constant_alignment_word_strings (const_tree exp, HOST_WIDE_INT align)
{
  if (TREE_CODE (exp) == STRING_CST)
    return MAX (align, BITS_PER_WORD);
  return align;
}

/* Default to natural alignment for vector types, bounded by
   MAX_OFILE_ALIGNMENT.  */

HOST_WIDE_INT
default_vector_alignment (const_tree type)
{
  unsigned HOST_WIDE_INT align = MAX_OFILE_ALIGNMENT;
  tree size = TYPE_SIZE (type);
  if (tree_fits_uhwi_p (size))
    align = tree_to_uhwi (size);
  if (align >= MAX_OFILE_ALIGNMENT)
    return MAX_OFILE_ALIGNMENT;
  return MAX (align, GET_MODE_ALIGNMENT (TYPE_MODE (type)));
}

/* The default implementation of
   TARGET_VECTORIZE_PREFERRED_VECTOR_ALIGNMENT.  */

poly_uint64
default_preferred_vector_alignment (const_tree type)
{
  return TYPE_ALIGN (type);
}

/* The default implementation of
   TARGET_VECTORIZE_PREFERRED_DIV_AS_SHIFTS_OVER_MULT.  */

bool
default_preferred_div_as_shifts_over_mult (const_tree type)
{
  return !can_mult_highpart_p (TYPE_MODE (type), TYPE_UNSIGNED (type));
}

/* By default assume vectors of element TYPE require a multiple of the natural
   alignment of TYPE.  TYPE is naturally aligned if IS_PACKED is false.  */
bool
default_builtin_vector_alignment_reachable (const_tree /*type*/, bool is_packed)
{
  return ! is_packed;
}

/* By default, assume that a target supports any factor of misalignment
   memory access if it supports movmisalign patten.
   is_packed is true if the memory access is defined in a packed struct.  */
bool
default_builtin_support_vector_misalignment (machine_mode mode,
					     const_tree type
					     ATTRIBUTE_UNUSED,
					     int misalignment
					     ATTRIBUTE_UNUSED,
					     bool is_packed
					     ATTRIBUTE_UNUSED,
					     bool is_gather_scatter
					     ATTRIBUTE_UNUSED)
{
  if (optab_handler (movmisalign_optab, mode) != CODE_FOR_nothing)
    return true;
  return false;
}

/* By default, only attempt to parallelize bitwise operations, and
   possibly adds/subtracts using bit-twiddling.  */

machine_mode
default_preferred_simd_mode (scalar_mode)
{
  return word_mode;
}

/* By default do not split reductions further.  */

machine_mode
default_split_reduction (machine_mode mode)
{
  return mode;
}

/* By default only the preferred vector mode is tried.  */

unsigned int
default_autovectorize_vector_modes (vector_modes *, bool)
{
  return 0;
}

/* The default implementation of TARGET_VECTORIZE_RELATED_MODE.  */

opt_machine_mode
default_vectorize_related_mode (machine_mode vector_mode,
				scalar_mode element_mode,
				poly_uint64 nunits)
{
  machine_mode result_mode;
  if ((maybe_ne (nunits, 0U)
       || multiple_p (GET_MODE_SIZE (vector_mode),
		      GET_MODE_SIZE (element_mode), &nunits))
      && mode_for_vector (element_mode, nunits).exists (&result_mode)
      && VECTOR_MODE_P (result_mode)
      && targetm.vector_mode_supported_p (result_mode))
    return result_mode;

  return opt_machine_mode ();
}

/* By default a vector of integers is used as a mask.  */

opt_machine_mode
default_get_mask_mode (machine_mode mode)
{
  return related_int_vector_mode (mode);
}

/* By default consider masked stores to be expensive.  */

bool
default_conditional_operation_is_expensive (unsigned ifn)
{
  return ifn == IFN_MASK_STORE;
}

/* By default consider masked stores to be expensive.  */

bool
default_empty_mask_is_expensive (unsigned ifn)
{
  return ifn == IFN_MASK_STORE;
}

/* By default, the cost model accumulates three separate costs (prologue,
   loop body, and epilogue) for a vectorized loop or block.  So allocate an
   array of three unsigned ints, set it to zero, and return its address.  */

vector_costs *
default_vectorize_create_costs (vec_info *vinfo, bool costing_for_scalar)
{
  return new vector_costs (vinfo, costing_for_scalar);
}

/* Determine whether or not a pointer mode is valid. Assume defaults
   of ptr_mode or Pmode - can be overridden.  */
bool
default_valid_pointer_mode (scalar_int_mode mode)
{
  return (mode == ptr_mode || mode == Pmode);
}

/* Determine whether the memory reference specified by REF may alias
   the C libraries errno location.  */
bool
default_ref_may_alias_errno (ao_ref *ref)
{
  tree base = ao_ref_base (ref);
  /* The default implementation assumes the errno location is
     a declaration of type int or is always accessed via a
     pointer to int.  We assume that accesses to errno are
     not deliberately obfuscated (even in conforming ways).  */
  if (TYPE_UNSIGNED (TREE_TYPE (base))
      || TYPE_MODE (TREE_TYPE (base)) != TYPE_MODE (integer_type_node))
    return false;
  /* The default implementation assumes an errno location declaration
     is never defined in the current compilation unit and may not be
     aliased by a local variable.  */
  if (DECL_P (base)
      && DECL_EXTERNAL (base)
      && !TREE_STATIC (base))
    return true;
  else if (TREE_CODE (base) == MEM_REF
	   && TREE_CODE (TREE_OPERAND (base, 0)) == SSA_NAME)
    {
      struct ptr_info_def *pi = SSA_NAME_PTR_INFO (TREE_OPERAND (base, 0));
      return !pi || pi->pt.anything || pi->pt.nonlocal;
    }
  return false;
}

/* Return the mode for a pointer to a given ADDRSPACE,
   defaulting to ptr_mode for all address spaces.  */

scalar_int_mode
default_addr_space_pointer_mode (addr_space_t addrspace ATTRIBUTE_UNUSED)
{
  return ptr_mode;
}

/* Return the mode for an address in a given ADDRSPACE,
   defaulting to Pmode for all address spaces.  */

scalar_int_mode
default_addr_space_address_mode (addr_space_t addrspace ATTRIBUTE_UNUSED)
{
  return Pmode;
}

/* Named address space version of valid_pointer_mode.
   To match the above, the same modes apply to all address spaces.  */

bool
default_addr_space_valid_pointer_mode (scalar_int_mode mode,
				       addr_space_t as ATTRIBUTE_UNUSED)
{
  return targetm.valid_pointer_mode (mode);
}

/* Some places still assume that all pointer or address modes are the
   standard Pmode and ptr_mode.  These optimizations become invalid if
   the target actually supports multiple different modes.  For now,
   we disable such optimizations on such targets, using this function.  */

bool
target_default_pointer_address_modes_p (void)
{
  if (targetm.addr_space.address_mode != default_addr_space_address_mode)
    return false;
  if (targetm.addr_space.pointer_mode != default_addr_space_pointer_mode)
    return false;

  return true;
}

/* Named address space version of legitimate_address_p.
   By default, all address spaces have the same form.  */

bool
default_addr_space_legitimate_address_p (machine_mode mode, rtx mem,
					 bool strict,
					 addr_space_t as ATTRIBUTE_UNUSED,
					 code_helper code)
{
  return targetm.legitimate_address_p (mode, mem, strict, code);
}

/* Named address space version of LEGITIMIZE_ADDRESS.
   By default, all address spaces have the same form.  */

rtx
default_addr_space_legitimize_address (rtx x, rtx oldx, machine_mode mode,
				       addr_space_t as ATTRIBUTE_UNUSED)
{
  return targetm.legitimize_address (x, oldx, mode);
}

/* The default hook for determining if one named address space is a subset of
   another and to return which address space to use as the common address
   space.  */

bool
default_addr_space_subset_p (addr_space_t subset, addr_space_t superset)
{
  return (subset == superset);
}

/* The default hook for determining if 0 within a named address
   space is a valid address.  */

bool
default_addr_space_zero_address_valid (addr_space_t as ATTRIBUTE_UNUSED)
{
  return false;
}

/* The default hook for debugging the address space is to return the
   address space number to indicate DW_AT_address_class.  */
int
default_addr_space_debug (addr_space_t as)
{
  return as;
}

/* The default hook implementation for TARGET_ADDR_SPACE_DIAGNOSE_USAGE.
   Don't complain about any address space.  */

void
default_addr_space_diagnose_usage (addr_space_t, location_t)
{
}


/* The default hook for TARGET_ADDR_SPACE_CONVERT. This hook should never be
   called for targets with only a generic address space.  */

rtx
default_addr_space_convert (rtx op ATTRIBUTE_UNUSED,
			    tree from_type ATTRIBUTE_UNUSED,
			    tree to_type ATTRIBUTE_UNUSED)
{
  gcc_unreachable ();
}

/* The defualt implementation of TARGET_HARD_REGNO_NREGS.  */

unsigned int
default_hard_regno_nregs (unsigned int, machine_mode mode)
{
  /* Targets with variable-sized modes must provide their own definition
     of this hook.  */
  return CEIL (GET_MODE_SIZE (mode).to_constant (), UNITS_PER_WORD);
}

bool
default_hard_regno_scratch_ok (unsigned int regno ATTRIBUTE_UNUSED)
{
  return true;
}

/* The default implementation of TARGET_MODE_DEPENDENT_ADDRESS_P.  */

bool
default_mode_dependent_address_p (const_rtx addr ATTRIBUTE_UNUSED,
				  addr_space_t addrspace ATTRIBUTE_UNUSED)
{
  return false;
}

extern bool default_new_address_profitable_p (rtx, rtx);


/* The default implementation of TARGET_NEW_ADDRESS_PROFITABLE_P.  */

bool
default_new_address_profitable_p (rtx memref ATTRIBUTE_UNUSED,
				  rtx_insn *insn ATTRIBUTE_UNUSED,
				  rtx new_addr ATTRIBUTE_UNUSED)
{
  return true;
}

bool
default_target_option_valid_attribute_p (tree ARG_UNUSED (fndecl),
					 tree ARG_UNUSED (name),
					 tree ARG_UNUSED (args),
					 int ARG_UNUSED (flags))
{
  warning (OPT_Wattributes,
	   "%<target%> attribute is not supported on this machine");

  return false;
}

bool
default_target_option_valid_version_attribute_p (tree ARG_UNUSED (fndecl),
						 tree ARG_UNUSED (name),
						 tree ARG_UNUSED (args),
						 int ARG_UNUSED (flags))
{
  warning (OPT_Wattributes,
	   "%<target_version%> attribute is not supported on this machine");

  return false;
}

bool
default_target_option_pragma_parse (tree ARG_UNUSED (args),
				    tree ARG_UNUSED (pop_target))
{
  /* If args is NULL the caller is handle_pragma_pop_options ().  In that case,
     emit no warning because "#pragma GCC pop_target" is valid on targets that
     do not have the "target" pragma.  */
  if (args)
    warning (OPT_Wpragmas,
	     "%<#pragma GCC target%> is not supported for this machine");

  return false;
}

bool
default_target_can_inline_p (tree caller, tree callee)
{
  tree callee_opts = DECL_FUNCTION_SPECIFIC_TARGET (callee);
  tree caller_opts = DECL_FUNCTION_SPECIFIC_TARGET (caller);
  if (! callee_opts)
    callee_opts = target_option_default_node;
  if (! caller_opts)
    caller_opts = target_option_default_node;

  /* If both caller and callee have attributes, assume that if the
     pointer is different, the two functions have different target
     options since build_target_option_node uses a hash table for the
     options.  */
  return callee_opts == caller_opts;
}

/* By default, return false to not need to collect any target information
   for inlining.  Target maintainer should re-define the hook if the
   target want to take advantage of it.  */

bool
default_need_ipa_fn_target_info (const_tree, unsigned int &)
{
  return false;
}

bool
default_update_ipa_fn_target_info (unsigned int &, const gimple *)
{
  return false;
}

/* If the machine does not have a case insn that compares the bounds,
   this means extra overhead for dispatch tables, which raises the
   threshold for using them.  */

unsigned int
default_case_values_threshold (void)
{
  return (targetm.have_casesi () ? 4 : 5);
}

bool
default_have_conditional_execution (void)
{
  return HAVE_conditional_execution;
}

bool
default_have_ccmp (void)
{
  return targetm.gen_ccmp_first != NULL;
}

/* By default we assume that c99 functions are present at the runtime,
   but sincos is not.  */
bool
default_libc_has_function (enum function_class fn_class,
			   tree type ATTRIBUTE_UNUSED)
{
  if (fn_class == function_c94
      || fn_class == function_c99_misc
      || fn_class == function_c99_math_complex)
    return true;

  return false;
}

/* By default assume that libc has not a fast implementation.  */

bool
default_libc_has_fast_function (int fcode ATTRIBUTE_UNUSED)
{
  return false;
}

bool
gnu_libc_has_function (enum function_class fn_class ATTRIBUTE_UNUSED,
		       tree type ATTRIBUTE_UNUSED)
{
  return true;
}

bool
no_c99_libc_has_function (enum function_class fn_class ATTRIBUTE_UNUSED,
			  tree type ATTRIBUTE_UNUSED)
{
  return false;
}

/* Assume some c99 functions are present at the runtime including sincos.  */
bool
bsd_libc_has_function (enum function_class fn_class,
		       tree type ATTRIBUTE_UNUSED)
{
  if (fn_class == function_c94
      || fn_class == function_c99_misc
      || fn_class == function_sincos)
    return true;

  return false;
}

/* By default, -fhardened will add -D_FORTIFY_SOURCE=2.  */

unsigned
default_fortify_source_default_level ()
{
  return 2;
}

unsigned
default_libm_function_max_error (unsigned, machine_mode, bool)
{
  return ~0U;
}

unsigned
glibc_linux_libm_function_max_error (unsigned cfn, machine_mode mode,
				     bool boundary_p)
{
  /* Let's use
     https://www.gnu.org/software/libc/manual/2.22/html_node/Errors-in-Math-Functions.html
     https://www.gnu.org/software/libc/manual/html_node/Errors-in-Math-Functions.html
     with usual values recorded here and significant outliers handled in
     target CPU specific overriders.  The tables only record default
     rounding to nearest, for -frounding-math let's add some extra ulps.
     For boundary_p values (say finite results outside of [-1.,1.] for
     sin/cos, or [-0.,+Inf] for sqrt etc. let's use custom random testers.  */
  int rnd = flag_rounding_math ? 4 : 0;
  bool sf = (REAL_MODE_FORMAT (mode) == &ieee_single_format
	     || REAL_MODE_FORMAT (mode) == &mips_single_format
	     || REAL_MODE_FORMAT (mode) == &motorola_single_format);
  bool df = (REAL_MODE_FORMAT (mode) == &ieee_double_format
	     || REAL_MODE_FORMAT (mode) == &mips_double_format
	     || REAL_MODE_FORMAT (mode) == &motorola_double_format);
  bool xf = (REAL_MODE_FORMAT (mode) == &ieee_extended_intel_96_format
	     || REAL_MODE_FORMAT (mode) == &ieee_extended_intel_128_format
	     || REAL_MODE_FORMAT (mode) == &ieee_extended_motorola_format);
  bool tf = (REAL_MODE_FORMAT (mode) == &ieee_quad_format
	     || REAL_MODE_FORMAT (mode) == &mips_quad_format);

  switch (cfn)
    {
    CASE_CFN_SQRT:
    CASE_CFN_SQRT_FN:
      if (boundary_p)
	/* https://gcc.gnu.org/pipermail/gcc-patches/2023-April/616595.html */
	return 0;
      if (sf || df || xf || tf)
	return 0 + rnd;
      break;
    CASE_CFN_COS:
    CASE_CFN_COS_FN:
      /* cos is generally errors like sin, but far more arches have 2ulps
	 for double.  */
      if (!boundary_p && df)
	return 2 + rnd;
      gcc_fallthrough ();
    CASE_CFN_SIN:
    CASE_CFN_SIN_FN:
      if (boundary_p)
	/* According to
	   https://sourceware.org/pipermail/gcc-patches/2023-April/616315.html
	   seems default rounding sin/cos stay strictly in [-1.,1.] range,
	   with rounding to infinity it can be 1ulp larger/smaller.  */
	return flag_rounding_math ? 1 : 0;
      if (sf || df)
	return 1 + rnd;
      if (xf || tf)
	return 2 + rnd;
      break;
    default:
      break;
    }

  return default_libm_function_max_error (cfn, mode, boundary_p);
}

tree
default_builtin_tm_load_store (tree ARG_UNUSED (type))
{
  return NULL_TREE;
}

/* Compute cost of moving registers to/from memory.  */

int
default_memory_move_cost (machine_mode mode ATTRIBUTE_UNUSED,
			  reg_class_t rclass ATTRIBUTE_UNUSED,
			  bool in ATTRIBUTE_UNUSED)
{
#ifndef MEMORY_MOVE_COST
    return (4 + memory_move_secondary_cost (mode, (enum reg_class) rclass, in));
#else
    return MEMORY_MOVE_COST (MACRO_MODE (mode), (enum reg_class) rclass, in);
#endif
}

/* Compute cost of moving data from a register of class FROM to one of
   TO, using MODE.  */

int
default_register_move_cost (machine_mode mode ATTRIBUTE_UNUSED,
                            reg_class_t from ATTRIBUTE_UNUSED,
                            reg_class_t to ATTRIBUTE_UNUSED)
{
#ifndef REGISTER_MOVE_COST
  return 2;
#else
  return REGISTER_MOVE_COST (MACRO_MODE (mode),
			     (enum reg_class) from, (enum reg_class) to);
#endif
}

/* The default implementation of TARGET_CALLEE_SAVE_COST.  */

int
default_callee_save_cost (spill_cost_type spill_type, unsigned int,
			  machine_mode, unsigned int, int mem_cost,
			  const HARD_REG_SET &callee_saved_regs,
			  bool existing_spills_p)
{
  if (!existing_spills_p)
    {
      auto frame_type = (spill_type == spill_cost_type::SAVE
			 ? frame_cost_type::ALLOCATION
			 : frame_cost_type::DEALLOCATION);
      mem_cost += targetm.frame_allocation_cost (frame_type,
						 callee_saved_regs);
    }
  return mem_cost;
}

/* The default implementation of TARGET_FRAME_ALLOCATION_COST.  */

int
default_frame_allocation_cost (frame_cost_type, const HARD_REG_SET &)
{
  return 0;
}

/* The default implementation of TARGET_SLOW_UNALIGNED_ACCESS.  */

bool
default_slow_unaligned_access (machine_mode, unsigned int)
{
  return STRICT_ALIGNMENT;
}

/* The default implementation of TARGET_ESTIMATED_POLY_VALUE.  */

HOST_WIDE_INT
default_estimated_poly_value (poly_int64 x, poly_value_estimate_kind)
{
  return x.coeffs[0];
}

/* For hooks which use the MOVE_RATIO macro, this gives the legacy default
   behavior.  SPEED_P is true if we are compiling for speed.  */

unsigned int
get_move_ratio (bool speed_p ATTRIBUTE_UNUSED)
{
  unsigned int move_ratio;
#ifdef MOVE_RATIO
  move_ratio = (unsigned int) MOVE_RATIO (speed_p);
#else
#if defined (HAVE_cpymemqi) || defined (HAVE_cpymemhi) || defined (HAVE_cpymemsi) || defined (HAVE_cpymemdi) || defined (HAVE_cpymemti)
  move_ratio = 2;
#else /* No cpymem patterns, pick a default.  */
  move_ratio = ((speed_p) ? 15 : 3);
#endif
#endif
  return move_ratio;
}

/* Return TRUE if the move_by_pieces/set_by_pieces infrastructure should be
   used; return FALSE if the cpymem/setmem optab should be expanded, or
   a call to memcpy emitted.  */

bool
default_use_by_pieces_infrastructure_p (unsigned HOST_WIDE_INT size,
					unsigned int alignment,
					enum by_pieces_operation op,
					bool speed_p)
{
  unsigned int max_size = 0;
  unsigned int ratio = 0;

  switch (op)
    {
    case CLEAR_BY_PIECES:
      max_size = STORE_MAX_PIECES;
      ratio = CLEAR_RATIO (speed_p);
      break;
    case MOVE_BY_PIECES:
      max_size = MOVE_MAX_PIECES;
      ratio = get_move_ratio (speed_p);
      break;
    case SET_BY_PIECES:
      max_size = STORE_MAX_PIECES;
      ratio = SET_RATIO (speed_p);
      break;
    case STORE_BY_PIECES:
      max_size = STORE_MAX_PIECES;
      ratio = get_move_ratio (speed_p);
      break;
    case COMPARE_BY_PIECES:
      max_size = COMPARE_MAX_PIECES;
      /* Pick a likely default, just as in get_move_ratio.  */
      ratio = speed_p ? 15 : 3;
      break;
    }

  return by_pieces_ninsns (size, alignment, max_size + 1, op) < ratio;
}

/* This hook controls code generation for expanding a memcmp operation by
   pieces.  Return 1 for the normal pattern of compare/jump after each pair
   of loads, or a higher number to reduce the number of branches.  */

int
default_compare_by_pieces_branch_ratio (machine_mode)
{
  return 1;
}

/* Write PATCH_AREA_SIZE NOPs into the asm outfile FILE around a function
   entry.  If RECORD_P is true and the target supports named sections,
   the location of the NOPs will be recorded in a special object section
   called "__patchable_function_entries".  This routine may be called
   twice per function to put NOPs before and after the function
   entry.  */

void
default_print_patchable_function_entry (FILE *file,
					unsigned HOST_WIDE_INT patch_area_size,
					bool record_p)
{
  const char *nop_templ = 0;
  int code_num;
  rtx_insn *my_nop = make_insn_raw (gen_nop ());

  /* We use the template alone, relying on the (currently sane) assumption
     that the NOP template does not have variable operands.  */
  code_num = recog_memoized (my_nop);
  nop_templ = get_insn_template (code_num, my_nop);

  if (record_p && targetm_common.have_named_sections)
    {
      char buf[256];
      section *previous_section = in_section;
      const char *asm_op = integer_asm_op (POINTER_SIZE_UNITS, false);

      gcc_assert (asm_op != NULL);
      /* If SECTION_LINK_ORDER is supported, this internal label will
	 be filled as the symbol for linked_to section.  */
      ASM_GENERATE_INTERNAL_LABEL (buf, "LPFE", current_function_funcdef_no);

      unsigned int flags = SECTION_WRITE | SECTION_RELRO;
      if (HAVE_GAS_SECTION_LINK_ORDER)
	flags |= SECTION_LINK_ORDER;

      section *sect = get_section ("__patchable_function_entries",
				  flags, current_function_decl);
      if (HAVE_COMDAT_GROUP && DECL_COMDAT_GROUP (current_function_decl))
	switch_to_comdat_section (sect, current_function_decl);
      else
	switch_to_section (sect);
      assemble_align (POINTER_SIZE);
      fputs (asm_op, file);
      assemble_name_raw (file, buf);
      fputc ('\n', file);

      switch_to_section (previous_section);
      ASM_OUTPUT_LABEL (file, buf);
    }

  unsigned i;
  for (i = 0; i < patch_area_size; ++i)
    output_asm_insn (nop_templ, NULL);
}

bool
default_profile_before_prologue (void)
{
#ifdef PROFILE_BEFORE_PROLOGUE
  return true;
#else
  return false;
#endif
}

/* The default implementation of TARGET_PREFERRED_RELOAD_CLASS.  */

reg_class_t
default_preferred_reload_class (rtx x ATTRIBUTE_UNUSED,
			        reg_class_t rclass)
{
#ifdef PREFERRED_RELOAD_CLASS
  return (reg_class_t) PREFERRED_RELOAD_CLASS (x, (enum reg_class) rclass);
#else
  return rclass;
#endif
}

/* The default implementation of TARGET_OUTPUT_PREFERRED_RELOAD_CLASS.  */

reg_class_t
default_preferred_output_reload_class (rtx x ATTRIBUTE_UNUSED,
				       reg_class_t rclass)
{
  return rclass;
}

/* The default implementation of TARGET_PREFERRED_RENAME_CLASS.  */
reg_class_t
default_preferred_rename_class (reg_class_t rclass ATTRIBUTE_UNUSED)
{
  return NO_REGS;
}

/* The default implementation of TARGET_CLASS_LIKELY_SPILLED_P.  */

bool
default_class_likely_spilled_p (reg_class_t rclass)
{
  return (reg_class_size[(int) rclass] == 1);
}

/* The default implementation of TARGET_CLASS_MAX_NREGS.  */

unsigned char
default_class_max_nregs (reg_class_t rclass ATTRIBUTE_UNUSED,
			 machine_mode mode ATTRIBUTE_UNUSED)
{
#ifdef CLASS_MAX_NREGS
  return (unsigned char) CLASS_MAX_NREGS ((enum reg_class) rclass,
					  MACRO_MODE (mode));
#else
  /* Targets with variable-sized modes must provide their own definition
     of this hook.  */
  unsigned int size = GET_MODE_SIZE (mode).to_constant ();
  return (size + UNITS_PER_WORD - 1) / UNITS_PER_WORD;
#endif
}

/* The default implementation of TARGET_AVOID_STORE_FORWARDING_P.  */

bool
default_avoid_store_forwarding_p (vec<store_fwd_info>, rtx, int total_cost,
				  bool)
{
  /* Use a simple cost heurstic base on param_store_forwarding_max_distance.
     In general the distance should be somewhat correlated to the store
     forwarding penalty; if the penalty is large then it is justified to
     increase the window size.  Use this to reject sequences that are clearly
     unprofitable.
     Skip the cost check if param_store_forwarding_max_distance is 0.  */
  int max_cost = COSTS_N_INSNS (param_store_forwarding_max_distance / 2);
  const bool unlimited_cost = (param_store_forwarding_max_distance == 0);
  if (!unlimited_cost && total_cost > max_cost && max_cost)
    {
      if (dump_file)
	fprintf (dump_file, "Not transformed due to cost: %d > %d.\n",
		 total_cost, max_cost);

      return false;
    }

  return true;
}

/* Determine the debugging unwind mechanism for the target.  */

enum unwind_info_type
default_debug_unwind_info (void)
{
  /* If the target wants to force the use of dwarf2 unwind info, let it.  */
  /* ??? Change all users to the hook, then poison this.  */
#ifdef DWARF2_FRAME_INFO
  if (DWARF2_FRAME_INFO)
    return UI_DWARF2;
#endif

  /* Otherwise, only turn it on if dwarf2 debugging is enabled.  */
#ifdef DWARF2_DEBUGGING_INFO
  if (dwarf_debuginfo_p ())
    return UI_DWARF2;
#endif

  return UI_NONE;
}

/* Targets that set NUM_POLY_INT_COEFFS to something greater than 1
   must define this hook.  */

unsigned int
default_dwarf_poly_indeterminate_value (unsigned int, unsigned int *, int *)
{
  gcc_unreachable ();
}

/* Determine the correct mode for a Dwarf frame register that represents
   register REGNO.  */

machine_mode
default_dwarf_frame_reg_mode (int regno)
{
  machine_mode save_mode = reg_raw_mode[regno];

  if (targetm.hard_regno_call_part_clobbered (eh_edge_abi.id (),
					      regno, save_mode))
    save_mode = choose_hard_reg_mode (regno, 1, &eh_edge_abi);
  return save_mode;
}

/* To be used by targets where reg_raw_mode doesn't return the right
   mode for registers used in apply_builtin_return and apply_builtin_arg.  */

fixed_size_mode
default_get_reg_raw_mode (int regno)
{
  /* Targets must override this hook if the underlying register is
     variable-sized.  */
  return as_a <fixed_size_mode> (reg_raw_mode[regno]);
}

/* Return true if a leaf function should stay leaf even with profiling
   enabled.  */

bool
default_keep_leaf_when_profiled ()
{
  return false;
}

/* Return true if the state of option OPTION should be stored in PCH files
   and checked by default_pch_valid_p.  Store the option's current state
   in STATE if so.  */

static inline bool
option_affects_pch_p (int option, struct cl_option_state *state)
{
  if ((cl_options[option].flags & CL_TARGET) == 0)
    return false;
  if ((cl_options[option].flags & CL_PCH_IGNORE) != 0)
    return false;
  if (option_flag_var (option, &global_options) == &target_flags)
    if (targetm.check_pch_target_flags)
      return false;
  return get_option_state (&global_options, option, state);
}

/* Default version of get_pch_validity.
   By default, every flag difference is fatal; that will be mostly right for
   most targets, but completely right for very few.  */

void *
default_get_pch_validity (size_t *sz)
{
  struct cl_option_state state;
  size_t i;
  char *result, *r;

  *sz = 2;
  if (targetm.check_pch_target_flags)
    *sz += sizeof (target_flags);
  for (i = 0; i < cl_options_count; i++)
    if (option_affects_pch_p (i, &state))
      *sz += state.size;

  result = r = XNEWVEC (char, *sz);
  r[0] = flag_pic;
  r[1] = flag_pie;
  r += 2;
  if (targetm.check_pch_target_flags)
    {
      memcpy (r, &target_flags, sizeof (target_flags));
      r += sizeof (target_flags);
    }

  for (i = 0; i < cl_options_count; i++)
    if (option_affects_pch_p (i, &state))
      {
	memcpy (r, state.data, state.size);
	r += state.size;
      }

  return result;
}

/* Return a message which says that a PCH file was created with a different
   setting of OPTION.  */

static const char *
pch_option_mismatch (const char *option)
{
  return xasprintf (_("created and used with differing settings of '%s'"),
		    option);
}

/* Default version of pch_valid_p.  */

const char *
default_pch_valid_p (const void *data_p, size_t len ATTRIBUTE_UNUSED)
{
  struct cl_option_state state;
  const char *data = (const char *)data_p;
  size_t i;

  /* -fpic and -fpie also usually make a PCH invalid.  */
  if (data[0] != flag_pic)
    return _("created and used with different settings of %<-fpic%>");
  if (data[1] != flag_pie)
    return _("created and used with different settings of %<-fpie%>");
  data += 2;

  /* Check target_flags.  */
  if (targetm.check_pch_target_flags)
    {
      int tf;
      const char *r;

      memcpy (&tf, data, sizeof (target_flags));
      data += sizeof (target_flags);
      r = targetm.check_pch_target_flags (tf);
      if (r != NULL)
	return r;
    }

  for (i = 0; i < cl_options_count; i++)
    if (option_affects_pch_p (i, &state))
      {
	if (memcmp (data, state.data, state.size) != 0)
	  return pch_option_mismatch (cl_options[i].opt_text);
	data += state.size;
      }

  return NULL;
}

/* Default version of cstore_mode.  */

scalar_int_mode
default_cstore_mode (enum insn_code icode)
{
  return as_a <scalar_int_mode> (insn_data[(int) icode].operand[0].mode);
}

/* Default version of member_type_forces_blk.  */

bool
default_member_type_forces_blk (const_tree, machine_mode)
{
  return false;
}

/* Default version of canonicalize_comparison.  */

void
default_canonicalize_comparison (int *, rtx *, rtx *, bool)
{
}

/* Default implementation of TARGET_ATOMIC_ASSIGN_EXPAND_FENV.  */

void
default_atomic_assign_expand_fenv (tree *, tree *, tree *)
{
}

#ifndef PAD_VARARGS_DOWN
#define PAD_VARARGS_DOWN BYTES_BIG_ENDIAN
#endif

/* Build an indirect-ref expression over the given TREE, which represents a
   piece of a va_arg() expansion.  */
tree
build_va_arg_indirect_ref (tree addr)
{
  addr = build_simple_mem_ref_loc (EXPR_LOCATION (addr), addr);
  return addr;
}

/* The "standard" implementation of va_arg: read the value from the
   current (padded) address and increment by the (padded) size.  */

tree
std_gimplify_va_arg_expr (tree valist, tree type, gimple_seq *pre_p,
			  gimple_seq *post_p)
{
  tree addr, t, type_size, rounded_size, valist_tmp;
  unsigned HOST_WIDE_INT align, boundary;
  bool indirect;

  /* All of the alignment and movement below is for args-grow-up machines.
     As of 2004, there are only 3 ARGS_GROW_DOWNWARD targets, and they all
     implement their own specialized gimplify_va_arg_expr routines.  */
  if (ARGS_GROW_DOWNWARD)
    gcc_unreachable ();

  indirect = pass_va_arg_by_reference (type);
  if (indirect)
    type = build_pointer_type (type);

  if (targetm.calls.split_complex_arg
      && TREE_CODE (type) == COMPLEX_TYPE
      && targetm.calls.split_complex_arg (type))
    {
      tree real_part, imag_part;

      real_part = std_gimplify_va_arg_expr (valist,
					    TREE_TYPE (type), pre_p, NULL);
      real_part = get_initialized_tmp_var (real_part, pre_p);

      imag_part = std_gimplify_va_arg_expr (unshare_expr (valist),
					    TREE_TYPE (type), pre_p, NULL);
      imag_part = get_initialized_tmp_var (imag_part, pre_p);

      return build2 (COMPLEX_EXPR, type, real_part, imag_part);
   }

  align = PARM_BOUNDARY / BITS_PER_UNIT;
  boundary = targetm.calls.function_arg_boundary (TYPE_MODE (type), type);

  /* When we align parameter on stack for caller, if the parameter
     alignment is beyond MAX_SUPPORTED_STACK_ALIGNMENT, it will be
     aligned at MAX_SUPPORTED_STACK_ALIGNMENT.  We will match callee
     here with caller.  */
  if (boundary > MAX_SUPPORTED_STACK_ALIGNMENT)
    boundary = MAX_SUPPORTED_STACK_ALIGNMENT;

  boundary /= BITS_PER_UNIT;

  /* Hoist the valist value into a temporary for the moment.  */
  valist_tmp = get_initialized_tmp_var (valist, pre_p);

  /* va_list pointer is aligned to PARM_BOUNDARY.  If argument actually
     requires greater alignment, we must perform dynamic alignment.  */
  if (boundary > align
      && !TYPE_EMPTY_P (type)
      && !integer_zerop (TYPE_SIZE (type)))
    {
      t = build2 (MODIFY_EXPR, TREE_TYPE (valist), valist_tmp,
		  fold_build_pointer_plus_hwi (valist_tmp, boundary - 1));
      gimplify_and_add (t, pre_p);

      t = build2 (MODIFY_EXPR, TREE_TYPE (valist), valist_tmp,
		  fold_build2 (BIT_AND_EXPR, TREE_TYPE (valist),
			       valist_tmp,
			       build_int_cst (TREE_TYPE (valist), -boundary)));
      gimplify_and_add (t, pre_p);
    }
  else
    boundary = align;

  /* If the actual alignment is less than the alignment of the type,
     adjust the type accordingly so that we don't assume strict alignment
     when dereferencing the pointer.  */
  boundary *= BITS_PER_UNIT;
  if (boundary < TYPE_ALIGN (type))
    {
      type = build_variant_type_copy (type);
      SET_TYPE_ALIGN (type, boundary);
    }

  /* Compute the rounded size of the type.  */
  type_size = arg_size_in_bytes (type);
  rounded_size = round_up (type_size, align);

  /* Reduce rounded_size so it's sharable with the postqueue.  */
  gimplify_expr (&rounded_size, pre_p, post_p, is_gimple_val, fb_rvalue);

  /* Get AP.  */
  addr = valist_tmp;
  if (PAD_VARARGS_DOWN && !integer_zerop (rounded_size))
    {
      /* Small args are padded downward.  */
      t = fold_build2_loc (input_location, GT_EXPR, sizetype,
		       rounded_size, size_int (align));
      t = fold_build3 (COND_EXPR, sizetype, t, size_zero_node,
		       size_binop (MINUS_EXPR, rounded_size, type_size));
      addr = fold_build_pointer_plus (addr, t);
    }

  /* Compute new value for AP.  */
  t = fold_build_pointer_plus (valist_tmp, rounded_size);
  t = build2 (MODIFY_EXPR, TREE_TYPE (valist), valist, t);
  gimplify_and_add (t, pre_p);

  addr = fold_convert (build_pointer_type (type), addr);

  if (indirect)
    addr = build_va_arg_indirect_ref (addr);

  return build_va_arg_indirect_ref (addr);
}

/* An implementation of TARGET_CAN_USE_DOLOOP_P for targets that do
   not support nested low-overhead loops.  */

bool
can_use_doloop_if_innermost (const widest_int &, const widest_int &,
			     unsigned int loop_depth, bool)
{
  return loop_depth == 1;
}

/* Default implementation of TARGET_OPTAB_SUPPORTED_P.  */

bool
default_optab_supported_p (int, machine_mode, machine_mode, optimization_type)
{
  return true;
}

/* Default implementation of TARGET_MAX_NOCE_IFCVT_SEQ_COST.  */

unsigned int
default_max_noce_ifcvt_seq_cost (edge e)
{
  bool predictable_p = predictable_edge_p (e);

  if (predictable_p)
    {
      if (OPTION_SET_P (param_max_rtl_if_conversion_predictable_cost))
	return param_max_rtl_if_conversion_predictable_cost;
    }
  else
    {
      if (OPTION_SET_P (param_max_rtl_if_conversion_unpredictable_cost))
	return param_max_rtl_if_conversion_unpredictable_cost;
    }

  return BRANCH_COST (true, predictable_p) * COSTS_N_INSNS (3);
}

/* Default implementation of TARGET_MIN_ARITHMETIC_PRECISION.  */

unsigned int
default_min_arithmetic_precision (void)
{
  return WORD_REGISTER_OPERATIONS ? BITS_PER_WORD : BITS_PER_UNIT;
}

/* Default implementation of TARGET_C_EXCESS_PRECISION.  */

enum flt_eval_method
default_excess_precision (enum excess_precision_type ATTRIBUTE_UNUSED)
{
  return FLT_EVAL_METHOD_PROMOTE_TO_FLOAT;
}

/* Return true if _BitInt(N) is supported and fill details about it into
   *INFO.  */
bool
default_bitint_type_info (int, struct bitint_info *)
{
  return false;
}

/* Default implementation for
  TARGET_STACK_CLASH_PROTECTION_ALLOCA_PROBE_RANGE.  */
HOST_WIDE_INT
default_stack_clash_protection_alloca_probe_range (void)
{
  return 0;
}

/* The default implementation of TARGET_EARLY_REMAT_MODES.  */

void
default_select_early_remat_modes (sbitmap)
{
}

/* The default implementation of TARGET_PREFERRED_ELSE_VALUE.  */

tree
default_preferred_else_value (unsigned, tree type, unsigned, tree *)
{
  return build_zero_cst (type);
}

/* Default implementation of TARGET_HAVE_SPECULATION_SAFE_VALUE.  */
bool
default_have_speculation_safe_value (bool active ATTRIBUTE_UNUSED)
{
#ifdef HAVE_speculation_barrier
  return active ? HAVE_speculation_barrier : true;
#else
  return false;
#endif
}
/* Alternative implementation of TARGET_HAVE_SPECULATION_SAFE_VALUE
   that can be used on targets that never have speculative execution.  */
bool
speculation_safe_value_not_needed (bool active)
{
  return !active;
}

/* Default implementation of the speculation-safe-load builtin.  This
   implementation simply copies val to result and generates a
   speculation_barrier insn, if such a pattern is defined.  */
rtx
default_speculation_safe_value (machine_mode mode ATTRIBUTE_UNUSED,
				rtx result, rtx val,
				rtx failval ATTRIBUTE_UNUSED)
{
  emit_move_insn (result, val);

#ifdef HAVE_speculation_barrier
  /* Assume the target knows what it is doing: if it defines a
     speculation barrier, but it is not enabled, then assume that one
     isn't needed.  */
  if (HAVE_speculation_barrier)
    emit_insn (gen_speculation_barrier ());
#endif

  return result;
}

/* How many bits to shift in order to access the tag bits.
   The default is to store the tag in the top 8 bits of a 64 bit pointer, hence
   shifting 56 bits will leave just the tag.  */
#define HWASAN_SHIFT (GET_MODE_PRECISION (Pmode) - 8)
#define HWASAN_SHIFT_RTX GEN_INT (HWASAN_SHIFT)

bool
default_memtag_can_tag_addresses ()
{
  return false;
}

uint8_t
default_memtag_tag_size ()
{
  return 8;
}

uint8_t
default_memtag_granule_size ()
{
  return 16;
}

/* The default implementation of TARGET_MEMTAG_INSERT_RANDOM_TAG.  */
rtx
default_memtag_insert_random_tag (rtx untagged, rtx target)
{
  gcc_assert (param_hwasan_instrument_stack);
  if (param_hwasan_random_frame_tag)
    {
      rtx fn = init_one_libfunc ("__hwasan_generate_tag");
      rtx new_tag = emit_library_call_value (fn, NULL_RTX, LCT_NORMAL, QImode);
      return targetm.memtag.set_tag (untagged, new_tag, target);
    }
  else
    {
      /* NOTE: The kernel API does not have __hwasan_generate_tag exposed.
	 In the future we may add the option emit random tags with inline
	 instrumentation instead of function calls.  This would be the same
	 between the kernel and userland.  */
      return untagged;
    }
}

/* The default implementation of TARGET_MEMTAG_ADD_TAG.  */
rtx
default_memtag_add_tag (rtx base, poly_int64 offset, uint8_t tag_offset)
{
  /* Need to look into what the most efficient code sequence is.
     This is a code sequence that would be emitted *many* times, so we
     want it as small as possible.

     There are two places where tag overflow is a question:
       - Tagging the shadow stack.
	  (both tagging and untagging).
       - Tagging addressable pointers.

     We need to ensure both behaviors are the same (i.e. that the tag that
     ends up in a pointer after "overflowing" the tag bits with a tag addition
     is the same that ends up in the shadow space).

     The aim is that the behavior of tag addition should follow modulo
     wrapping in both instances.

     The libhwasan code doesn't have any path that increments a pointer's tag,
     which means it has no opinion on what happens when a tag increment
     overflows (and hence we can choose our own behavior).  */

  offset += ((uint64_t)tag_offset << HWASAN_SHIFT);
  return plus_constant (Pmode, base, offset);
}

/* The default implementation of TARGET_MEMTAG_SET_TAG.  */
rtx
default_memtag_set_tag (rtx untagged, rtx tag, rtx target)
{
  gcc_assert (GET_MODE (untagged) == Pmode && GET_MODE (tag) == QImode);
  tag = expand_simple_binop (Pmode, ASHIFT, tag, HWASAN_SHIFT_RTX, NULL_RTX,
			     /* unsignedp = */1, OPTAB_WIDEN);
  rtx ret = expand_simple_binop (Pmode, IOR, untagged, tag, target,
				 /* unsignedp = */1, OPTAB_DIRECT);
  gcc_assert (ret);
  return ret;
}

/* The default implementation of TARGET_MEMTAG_EXTRACT_TAG.  */
rtx
default_memtag_extract_tag (rtx tagged_pointer, rtx target)
{
  rtx tag = expand_simple_binop (Pmode, LSHIFTRT, tagged_pointer,
				 HWASAN_SHIFT_RTX, target,
				 /* unsignedp = */0,
				 OPTAB_DIRECT);
  rtx ret = gen_lowpart (QImode, tag);
  gcc_assert (ret);
  return ret;
}

/* The default implementation of TARGET_MEMTAG_UNTAGGED_POINTER.  */
rtx
default_memtag_untagged_pointer (rtx tagged_pointer, rtx target)
{
  rtx tag_mask = gen_int_mode ((HOST_WIDE_INT_1U << HWASAN_SHIFT) - 1, Pmode);
  rtx untagged_base = expand_simple_binop (Pmode, AND, tagged_pointer,
					   tag_mask, target, true,
					   OPTAB_DIRECT);
  gcc_assert (untagged_base);
  return untagged_base;
}

#include "gt-targhooks.h"
