/*
 * SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
 *
 * Derived from Xenomai Cobalt, https://xenomai.org/
 * Copyright (C) 2006 Gilles Chanteperdrix <gilles.chanteperdrix@xenomai.org>.
 * Copyright (C) 2023 Hao Miao <haomiao19@mails.ucas.ac.cn>.
 */
#ifndef _EVL_LOONGARCH64_ASM_UAPI_FPTEST_H
#define _EVL_LOONGARCH64_ASM_UAPI_FPTEST_H

#include <linux/types.h>

#define evl_loongarch64_fpu  0x1

/*
 * CAUTION: keep this code strictly inlined in macros: we don't want
 * GCC to apply any callee-saved logic to fpsimd registers in
 * evl_set_fpregs() before evl_check_fpregs() can verify their
 * contents, but we still want GCC to know about the registers we have
 * clobbered.
 */

#define evl_set_fpregs(__features, __val)				\
	do {								\
		unsigned int __i;					\
		__u64 __e[32];						\
									\
		if (__features & evl_loongarch64_fpu) {			\
			for (__i = 0; __i < 32; __i++)			\
				__e[__i] = (__val);			\
			__asm__ __volatile__("fld.d $f0, %0, 8 * 0\n"	\
					     "fld.d $f1, %0, 8 * 1\n"	\
					     "fld.d $f2, %0, 8 * 2\n"	\
					     "fld.d $f3, %0, 8 * 3\n"	\
					     "fld.d $f4, %0, 8 * 4\n"	\
					     "fld.d $f5, %0, 8 * 5\n"	\
					     "fld.d $f6, %0, 8 * 6\n"	\
					     "fld.d $f7, %0, 8 * 7\n"	\
					     "fld.d $f8, %0, 8 * 8\n"	\
					     "fld.d $f9, %0, 8 * 9\n"	\
					     "fld.d $f10, %0, 8 * 10\n"	\
					     "fld.d $f11, %0, 8 * 11\n"	\
					     "fld.d $f12, %0, 8 * 12\n"	\
					     "fld.d $f13, %0, 8 * 13\n"	\
					     "fld.d $f14, %0, 8 * 14\n"	\
					     "fld.d $f15, %0, 8 * 15\n"	\
					     "fld.d $f16, %0, 8 * 16\n"	\
					     "fld.d $f17, %0, 8 * 17\n"	\
					     "fld.d $f18, %0, 8 * 18\n"	\
					     "fld.d $f19, %0, 8 * 19\n"	\
					     "fld.d $f10, %0, 8 * 20\n"	\
					     "fld.d $f21, %0, 8 * 21\n"	\
					     "fld.d $f22, %0, 8 * 22\n"	\
					     "fld.d $f23, %0, 8 * 23\n"	\
					     "fld.d $f24, %0, 8 * 24\n"	\
					     "fld.d $f25, %0, 8 * 25\n"	\
					     "fld.d $f26, %0, 8 * 26\n"	\
					     "fld.d $f27, %0, 8 * 27\n"	\
					     "fld.d $f28, %0, 8 * 28\n"	\
					     "fld.d $f29, %0, 8 * 29\n"	\
					     "fld.d $f30, %0, 8 * 30\n"	\
					     "fld.d $f31, %0, 8 * 31\n"	\
					     : /* No outputs. */	\
					     : "r"(&__e[0])		\
					     : "f0", "f1", "f2", "f3",	\
					       "f4", "f5", "f6", "f7",	\
					       "f8", "f9", "f10",	\
					       "f11", "f12", "f13",	\
					       "f14", "f15", "f16",	\
					       "f17", "f18", "f19",	\
					       "f20", "f21", "f22",	\
					       "f23", "f24", "f25",	\
					       "f26", "f27", "f28",	\
					       "f29", "f30", "f31",	\
					       "memory");		\
		}							\
	} while (0)

#define evl_check_fpregs(__features, __val, __bad)			\
	({								\
		unsigned int __result = (__val), __i;			\
		__u64 __e[32];						\
									\
		if (__features & evl_loongarch64_fpu) {			\
			__asm__ __volatile__("fst.d $f0, %0, 8 * 0\n"	\
					     "fst.d $f1, %0, 8 * 1\n"	\
					     "fst.d $f2, %0, 8 * 2\n"	\
					     "fst.d $f3, %0, 8 * 3\n"	\
					     "fst.d $f4, %0, 8 * 4\n"	\
					     "fst.d $f5, %0, 8 * 5\n"	\
					     "fst.d $f6, %0, 8 * 6\n"	\
					     "fst.d $f7, %0, 8 * 7\n"	\
					     "fst.d $f8, %0, 8 * 8\n"	\
					     "fst.d $f9, %0, 8 * 9\n"	\
					     "fst.d $f10, %0, 8 * 10\n"	\
					     "fst.d $f11, %0, 8 * 11\n"	\
					     "fst.d $f12, %0, 8 * 12\n"	\
					     "fst.d $f13, %0, 8 * 13\n"	\
					     "fst.d $f14, %0, 8 * 14\n"	\
					     "fst.d $f15, %0, 8 * 15\n"	\
					     "fst.d $f16, %0, 8 * 16\n"	\
					     "fst.d $f17, %0, 8 * 17\n"	\
					     "fst.d $f18, %0, 8 * 18\n"	\
					     "fst.d $f19, %0, 8 * 19\n"	\
					     "fst.d $f10, %0, 8 * 20\n"	\
					     "fst.d $f21, %0, 8 * 21\n"	\
					     "fst.d $f22, %0, 8 * 22\n"	\
					     "fst.d $f23, %0, 8 * 23\n"	\
					     "fst.d $f24, %0, 8 * 24\n"	\
					     "fst.d $f25, %0, 8 * 25\n"	\
					     "fst.d $f26, %0, 8 * 26\n"	\
					     "fst.d $f27, %0, 8 * 27\n"	\
					     "fst.d $f28, %0, 8 * 28\n"	\
					     "fst.d $f29, %0, 8 * 29\n"	\
					     "fst.d $f30, %0, 8 * 30\n"	\
					     "fst.d $f31, %0, 8 * 31\n"	\
					     :  /* No outputs. */	\
					     : "r"(&__e[0])		\
					     : "memory");		\
									\
			for (__i = 0; __i < 32; __i++)			\
				if (__e[__i] != __val) {		\
					__result = __e[__i];		\
					(__bad) = __i;			\
					break;				\
				}					\
		}							\
		__result;						\
	})

#endif /* !_EVL_LOONGARCH64_ASM_UAPI_FPTEST_H */
