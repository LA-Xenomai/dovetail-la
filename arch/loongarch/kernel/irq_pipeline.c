/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2018 Philippe Gerum  <rpm@xenomai.org>.
 * Copyright (C) 2023 Hao Miao <haomiao19@mails.ucas.ac.cn>.
 */
#include <linux/irq.h>
#include <linux/irq_pipeline.h>

void arch_do_IRQ_pipelined(struct irq_desc *desc)
{
	struct pt_regs *regs = raw_cpu_ptr(&irq_pipeline.tick_regs);
	unsigned int irq = irq_desc_get_irq(desc);

	__handle_domain_irq(NULL, irq, false, regs);
}

void __init arch_irq_pipeline_init(void)
{
	/* no per-arch init. */
}
