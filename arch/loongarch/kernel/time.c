// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Loongson Technology Corporation Limited
 *
 * Common time service routines for LoongArch machines.
 *
 * This program is free software; you can redistribute	it and/or modify it
 * under  the terms of	the GNU General	 Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */
#include <linux/clockchips.h>
#include <linux/export.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/sched_clock.h>
#include <linux/spinlock.h>
#include <linux/of_clk.h>
#include <linux/clk.h>

#include <asm/cpu-features.h>
#include <asm/loongarchregs.h>
#include <asm/time.h>

u64 cpu_clock_freq;
EXPORT_SYMBOL(cpu_clock_freq);
u64 const_clock_freq;
EXPORT_SYMBOL(const_clock_freq);

static DEFINE_RAW_SPINLOCK(state_lock);
static DEFINE_PER_CPU(struct clock_event_device, constant_clockevent_device);

static void constant_event_handler(struct clock_event_device *dev)
{
}

irqreturn_t constant_timer_interrupt(int irq, void *data)
{
	int cpu = smp_processor_id();
	struct clock_event_device *cd;

	/* Clear Timer Interrupt */
	write_csr_tintclear(CSR_TINTCLR_TI);
	cd = &per_cpu(constant_clockevent_device, cpu);
	clockevents_handle_event(cd);

	return IRQ_HANDLED;
}

static int constant_set_state_oneshot(struct clock_event_device *evt)
{
	unsigned long timer_config;

	raw_spin_lock(&state_lock);

	timer_config = csr_readq(LOONGARCH_CSR_TCFG);
	timer_config |= CSR_TCFG_EN;
	timer_config &= ~CSR_TCFG_PERIOD;
	csr_writeq(timer_config, LOONGARCH_CSR_TCFG);

	raw_spin_unlock(&state_lock);

	return 0;
}

static int constant_set_state_oneshot_stopped(struct clock_event_device *evt)
{
	unsigned long timer_config;

	raw_spin_lock(&state_lock);

	timer_config = csr_readq(LOONGARCH_CSR_TCFG);
	timer_config &= ~CSR_TCFG_EN;
	csr_writeq(timer_config, LOONGARCH_CSR_TCFG);

	raw_spin_unlock(&state_lock);

	return 0;
}

static int constant_set_state_periodic(struct clock_event_device *evt)
{
	unsigned long period;
	unsigned long timer_config;

	raw_spin_lock(&state_lock);

	period = const_clock_freq / HZ;
	timer_config = period & CSR_TCFG_VAL;
	timer_config |= (CSR_TCFG_PERIOD | CSR_TCFG_EN);
	csr_writeq(timer_config, LOONGARCH_CSR_TCFG);

	raw_spin_unlock(&state_lock);

	return 0;
}

static int constant_set_state_shutdown(struct clock_event_device *evt)
{
	return 0;
}

static int constant_timer_next_event(unsigned long delta, struct clock_event_device *evt)
{
	unsigned long timer_config;

	delta &= CSR_TCFG_VAL;
	timer_config = delta | CSR_TCFG_EN;
	csr_writeq(timer_config, LOONGARCH_CSR_TCFG);

	return 0;
}

static unsigned long __init get_loops_per_jiffy(void)
{
	unsigned long lpj = (unsigned long)const_clock_freq;

	do_div(lpj, HZ);

	return lpj;
}

static long init_offset __nosavedata;

void save_counter(void)
{
	init_offset = drdtime();
}

void sync_counter(void)
{
	/* Ensure counter begin at 0 */
	csr_writeq(init_offset, LOONGARCH_CSR_CNTC);
}

int constant_clockevent_init(void)
{
	unsigned int irq;
	unsigned int cpu = smp_processor_id();
	unsigned long min_delta = 0x600;
	unsigned long max_delta = (1UL << 48) - 1;
	struct clock_event_device *cd;
	static int timer_irq_installed;

	irq = get_timer_irq();

	cd = &per_cpu(constant_clockevent_device, cpu);

	cd->name = "Constant";
	cd->features = CLOCK_EVT_FEAT_ONESHOT | CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_PERCPU | CLOCK_EVT_FEAT_PIPELINE;

	cd->irq = irq;
	cd->rating = 320;
	cd->cpumask = cpumask_of(cpu);
	cd->set_state_oneshot = constant_set_state_oneshot;
	cd->set_state_oneshot_stopped = constant_set_state_oneshot_stopped;
	cd->set_state_periodic = constant_set_state_periodic;
	cd->set_state_shutdown = constant_set_state_shutdown;
	cd->set_next_event = constant_timer_next_event;
	cd->event_handler = constant_event_handler;

	clockevents_config_and_register(cd, const_clock_freq, min_delta, max_delta);

	if (timer_irq_installed)
		return 0;

	timer_irq_installed = 1;

	sync_counter();

	if (request_irq(irq, constant_timer_interrupt, IRQF_PERCPU | IRQF_TIMER, "timer", NULL))
		pr_err("Failed to request irq %d (timer)\n", irq);

	lpj_fine = get_loops_per_jiffy();
	pr_info("Constant clock event device register\n");

	return 0;
}

static u64 read_const_counter(struct clocksource *clk)
{
	return drdtime();
}

static u64 native_sched_clock(void)
{
	return read_const_counter(NULL);
}

static struct clocksource clocksource_const = {
	.name = "Constant",
	.rating = 400,
	.read = read_const_counter,
	.mask = CLOCKSOURCE_MASK(64),
	.flags = CLOCK_SOURCE_IS_CONTINUOUS,
	.mult = 0,
	.shift = 10,
};

int __init constant_clocksource_init(void)
{
	int res;
	unsigned long freq;

	freq = const_clock_freq;

	clocksource_const.mult =
		clocksource_hz2mult(freq, clocksource_const.shift);

	res = clocksource_register_hz(&clocksource_const, freq);

	sched_clock_register(native_sched_clock, 64, freq);

	pr_info("Constant clock source device register\n");

	return res;
}

void __init time_init(void)
{
#ifdef CONFIG_OF
	struct clk* cpu_clk;
	of_clk_init(NULL);

	if (!cpu_clock_freq) {
		cpu_clk = clk_get(NULL, "cpu_clk");
		if (cpu_clk)
			cpu_clock_freq = clk_get_rate(cpu_clk);
	}
#endif

	if (!cpu_has_cpucfg)
		const_clock_freq = cpu_clock_freq;
	else
		const_clock_freq = calc_const_freq();

	init_offset = -(drdtime() - csr_readq(LOONGARCH_CSR_CNTC));

	constant_clockevent_init();
	constant_clocksource_init();
}
