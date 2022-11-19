/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * CPU specific code
 */

#include <common.h>
#include <command.h>
#include <asm/system.h>

static void cache_flush(void);

int cleanup_before_linux (void)
{
	/*
	 * this function is called just before we call linux
	 * it prepares the processor for linux
	 *
	 * we turn off caches etc ...
	 */

	unsigned long i;

	disable_interrupts ();

	/* turn off I/D-cache */
	icache_disable();
	dcache_disable();
	/* flush I/D-cache */
	cache_flush();

#ifdef CONFIG_MARVELL
	/* turn off L2 Cache */
	asm ("mrc p15, 1, %0, c15, c1, 0":"=r" (i));
	i &= ~0x00400000;
	asm ("mcr p15, 1, %0, c15, c1, 0": :"r" (i));
	__asm__ __volatile__("nop;nop;nop;nop;nop;nop;nop");
	/* Clean L2 Cache */
	asm ("mcr p15, 1, %0, c15, c9, 0": :"r" (i));
	__asm__ __volatile__("nop;nop;nop;nop;nop;nop;nop");
	/* Drain write buffer */
	asm ("mcr p15, 0, %0, c7, c10, 4": :"r" (i));
	__asm__ __volatile__("nop;nop;nop;nop;nop;nop;nop");
#endif

	return 0;
}

/* flush I/D-cache */
static void cache_flush (void)
{
	unsigned long i = 0;

	asm ("mcr p15, 0, %0, c7, c7, 0": :"r" (i));
}
