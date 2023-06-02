// SPDX-License-Identifier: GPL-2.0
/*
 * LoongArch idle loop support.
 *
 * Copyright (C) 2020 Loongson Technology Corporation Limited
 */
#include <linux/cpu.h>
#include <linux/irqflags.h>
#include <asm/cpu.h>
#include <asm/idle.h>

void __cpuidle arch_cpu_idle(void)
{
	raw_local_irq_enable();
	hard_cond_local_irq_enable();
	__arch_cpu_idle(); /* idle instruction needs irq enabled */
}
