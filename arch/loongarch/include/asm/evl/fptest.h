/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _EVL_LOONGARCH64_ASM_FPTEST_H
#define _EVL_LOONGARCH64_ASM_FPTEST_H

#include <linux/cpufeature.h>
#include <uapi/asm/evl/fptest.h>

static inline bool evl_begin_fpu(void)
{
	return false;
}

static inline void evl_end_fpu(void) { }

static inline u32 evl_detect_fpu(void)
{
	u32 features = 0;

	if (cpu_has_fpu)
		return features |= evl_loongarch64_fpu;

	return features;
}

#endif /* _EVL_LOONGARCH64_ASM_FPTEST_H */
