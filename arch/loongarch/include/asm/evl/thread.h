/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _EVL_LOONGARCH64_ASM_THREAD_H
#define _EVL_LOONGARCH64_ASM_THREAD_H

#include "asm/dovetail.h"
static inline bool evl_is_breakpoint(int trapnr)
{
	return trapnr == LOONGARCH64_TRAP_BP;
}

#endif /* !_EVL_LOONGARCH64_ASM_THREAD_H */
