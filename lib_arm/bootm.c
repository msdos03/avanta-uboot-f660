/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307	 USA
 *
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <u-boot/zlib.h>
#include <asm/byteorder.h>

extern void enable_watchdog(int);
extern int aei_boardid(void);

DECLARE_GLOBAL_DATA_PTR;

#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_VFD) || \
    defined (CONFIG_LCD) || \
    defined (CONFIG_MARVELL_TAG)
static void setup_start_tag (bd_t *bd);

# ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd);
# endif
static void setup_commandline_tag (bd_t *bd, char *commandline);

# ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start,
			      ulong initrd_end);
# endif
static void setup_end_tag (bd_t *bd);

# if defined (CONFIG_VFD) || defined (CONFIG_LCD)
static void setup_videolfb_tag (gd_t *gd);
# endif

#if defined (CONFIG_MARVELL_TAG)
extern void mvEgigaStrToMac( char *source , char *dest );
static void setup_marvell_tag(void);
#endif

static struct tag *params;
#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */

int do_bootm_linux(int flag, int argc, char *argv[], bootm_headers_t *images)
{
	bd_t	*bd = gd->bd;
	char	*s;
	int	machid = bd->bi_arch_number;
	void	(*theKernel)(int zero, int arch, uint params);

#ifdef CONFIG_CMDLINE_TAG
	char *commandline = getenv ("bootargs");
#endif

	if ((flag != 0) && (flag != BOOTM_STATE_OS_GO))
		return 1;

	theKernel = (void (*)(int, int, uint))images->ep;

	s = getenv ("machid");
	if (s) {
		machid = simple_strtoul (s, NULL, 16);
		printf ("Using machid 0x%x from environment\n", machid);
	}

	show_boot_progress (15);

	debug ("## Transferring control to Linux (at address %08lx) ...\n",
	       (ulong) theKernel);

#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_LCD) || \
    defined (CONFIG_VFD) || \
    defined (CONFIG_MARVELL_TAG)
	setup_start_tag (bd);
#ifdef CONFIG_SERIAL_TAG
	setup_serial_tag (&params);
#endif
#ifdef CONFIG_REVISION_TAG
	setup_revision_tag (&params);
#endif
#ifdef CONFIG_SETUP_MEMORY_TAGS
	setup_memory_tags (bd);
#endif
#ifdef CONFIG_CMDLINE_TAG
	setup_commandline_tag (bd, commandline);
#endif
#ifdef CONFIG_INITRD_TAG
	if (images->rd_start && images->rd_end)
		setup_initrd_tag (bd, images->rd_start, images->rd_end);
#endif
#if defined (CONFIG_VFD) || defined (CONFIG_LCD)
	setup_videolfb_tag ((gd_t *) gd);
#endif
#if defined (CONFIG_MARVELL_TAG)
        /* Linux open port doesn't support the Marvell TAG */
	char *env = getenv("mainlineLinux");
	if(!env || ((strcmp(env,"no") == 0) ||  (strcmp(env,"No") == 0)))
	    setup_marvell_tag ();
#endif
	setup_end_tag (bd);
#endif

	/* we assume that the kernel is in place */
	printf ("\nStarting kernel ...\n\n");

#ifdef CONFIG_USB_DEVICE
	{
		extern void udc_disconnect (void);
		udc_disconnect ();
	}
#endif

	cleanup_before_linux ();

	enable_watchdog(2);
	theKernel (0, machid, bd->bi_boot_params);
	/* does not return */
	enable_watchdog(-1);

	return 1;
}


#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_LCD) || \
    defined (CONFIG_VFD) || \
    defined (CONFIG_MARVELL_TAG)
static void setup_start_tag (bd_t *bd)
{
	params = (struct tag *) bd->bi_boot_params;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}


#ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd)
{
	int i;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		params->hdr.tag = ATAG_MEM;
		params->hdr.size = tag_size (tag_mem32);

		params->u.mem.start = bd->bi_dram[i].start;
		params->u.mem.size = bd->bi_dram[i].size;

		params = tag_next (params);
	}
}
#endif /* CONFIG_SETUP_MEMORY_TAGS */


static void setup_commandline_tag (bd_t *bd, char *commandline)
{
	char *p;

	if (!commandline)
		return;

	/* eat leading white space */
	for (p = commandline; *p == ' '; p++);

	/* skip non-existent command lines so the kernel will still
	 * use its default command line.
	 */
	if (*p == '\0')
		return;

	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
		(sizeof (struct tag_header) + strlen (p) + 1 + 4) >> 2;

	strcpy (params->u.cmdline.cmdline, p);

	params = tag_next (params);
}


#ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start, ulong initrd_end)
{
	/* an ATAG_INITRD node tells the kernel where the compressed
	 * ramdisk can be found. ATAG_RDIMG is a better name, actually.
	 */
	params->hdr.tag = ATAG_INITRD2;
	params->hdr.size = tag_size (tag_initrd);

	params->u.initrd.start = initrd_start;
	params->u.initrd.size = initrd_end - initrd_start;

	params = tag_next (params);
}
#endif /* CONFIG_INITRD_TAG */


#if defined (CONFIG_VFD) || defined (CONFIG_LCD)
extern ulong calc_fbsize (void);
static void setup_videolfb_tag (gd_t *gd)
{
	/* An ATAG_VIDEOLFB node tells the kernel where and how large
	 * the framebuffer for video was allocated (among other things).
	 * Note that a _physical_ address is passed !
	 *
	 * We only use it to pass the address and size, the other entries
	 * in the tag_videolfb are not of interest.
	 */
	params->hdr.tag = ATAG_VIDEOLFB;
	params->hdr.size = tag_size (tag_videolfb);

	params->u.videolfb.lfb_base = (u32) gd->fb_base;
	/* Fb size is calculated according to parameters for our panel
	 */
	params->u.videolfb.lfb_size = calc_fbsize();

	params = tag_next (params);
}
#endif /* CONFIG_VFD || CONFIG_LCD */
#if defined(CONFIG_MARVELL_TAG)

extern unsigned int mvBoardIdGet(void);	
extern void mvBoardModuleConfigGet(u32 *modConfig);

static void setup_marvell_tag (void)
{
	char *env, *tcp;
	char temp[20];
	int i;
	unsigned int boardId;
	u32 modCfg;

	params->hdr.tag = ATAG_MARVELL;
	params->hdr.size = tag_size (tag_mv_uboot);

	params->u.mv_uboot.uboot_version = VER_NUM;
	if(strcmp(getenv("nandEcc"), "4bit") == 0)
		params->u.mv_uboot.nand_ecc = 4;
	else if(strcmp(getenv("nandEcc"), "1bit") == 0)
		params->u.mv_uboot.nand_ecc = 1;

	boardId = aei_boardid();
        //boardId = mvBoardIdGet();
	params->u.mv_uboot.uboot_version |= boardId;
	params->u.mv_uboot.tclk = CONFIG_SYS_TCLK;
	params->u.mv_uboot.sysclk = CONFIG_SYS_BUS_CLK;
	
#if defined(MV78XX0)
	/* Dual CPU Firmware load address */
	env = getenv("fw_image_base");
	if(env)
		params->u.mv_uboot.fw_image_base = simple_strtoul(env, NULL, 16);
	else
		params->u.mv_uboot.fw_image_base = 0;

	/* Dual CPU Firmware size */
	env = getenv("fw_image_size");
	if(env)
		params->u.mv_uboot.fw_image_size = simple_strtoul(env, NULL, 16);
	else
		params->u.mv_uboot.fw_image_size = 0;
#endif

#if defined(MV_INCLUDE_USB)
	extern unsigned int mvCtrlUsbMaxGet(void);

	for (i = 0 ; i < mvCtrlUsbMaxGet(); i++)
	{
		sprintf( temp, "usb%dMode", i);
		env = getenv(temp);
		if((!env) || (strcmp(env,"Host") == 0 ) || (strcmp(env,"host") == 0) )
			params->u.mv_uboot.isUsbHost |= (1 << i);
		else
			params->u.mv_uboot.isUsbHost &= ~(1 << i);	
	}
#endif /*#if defined(MV_INCLUDE_USB)*/
#if defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH)
	extern unsigned int mvCtrlEthMaxPortGet(void);
	extern int mvMacStrToHex(const char* macStr, unsigned char* macHex);

	for (i = 0 ;i < 4;i++)
	{
		memset(params->u.mv_uboot.macAddr[i], 0, sizeof(params->u.mv_uboot.macAddr[i]));
		params->u.mv_uboot.mtu[i] = 0; 
	}

	for (i = 0 ;i < mvCtrlEthMaxPortGet();i++)
	{
/* only on RD-6281-A egiga0 defined as eth1 */
#if defined (RD_88F6281A)
		sprintf( temp,(i==0 ? "eth1addr" : "ethaddr"));
#else
		sprintf( temp,(i ? "eth%daddr" : "ethaddr"), i);
# endif
#if defined(MV_KW2)
		if(i == 2)
			sprintf(temp, "mv_pon_addr");
#endif

		env = getenv(temp);
		if (env)
			mvMacStrToHex(env, (unsigned char*)params->u.mv_uboot.macAddr[i]);

/* only on RD-6281-A egiga0 defined as eth1 */
#if defined (RD_88F6281A)
		sprintf( temp,(i==0 ? "eth1mtu" : "ethmtu"));
#else
		sprintf( temp,(i ? "eth%dmtu" : "ethmtu"), i);
# endif
		env = getenv(temp);
		if (env)
			params->u.mv_uboot.mtu[i] = simple_strtoul(env, NULL, 10); 
	}
#endif /* (MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH) */

	/* Set Board modules configuration */

#ifdef DB_88F6500
	mvBoardModuleConfigGet(&modCfg);
#else
	modCfg = (u32)-1;
#endif
	params->u.mv_uboot.board_module_config = modCfg;

	params = tag_next (params);
}
#endif

#ifdef CONFIG_SERIAL_TAG
void setup_serial_tag (struct tag **tmp)
{
	struct tag *params = *tmp;
	struct tag_serialnr serialnr;
	void get_board_serial(struct tag_serialnr *serialnr);

	get_board_serial(&serialnr);
	params->hdr.tag = ATAG_SERIAL;
	params->hdr.size = tag_size (tag_serialnr);
	params->u.serialnr.low = serialnr.low;
	params->u.serialnr.high= serialnr.high;
	params = tag_next (params);
	*tmp = params;
}
#endif

#ifdef CONFIG_REVISION_TAG
void setup_revision_tag(struct tag **in_params)
{
	u32 rev = 0;
	u32 get_board_rev(void);

	rev = get_board_rev();
	params->hdr.tag = ATAG_REVISION;
	params->hdr.size = tag_size (tag_revision);
	params->u.revision.rev = rev;
	params = tag_next (params);
}
#endif  /* CONFIG_REVISION_TAG */


static void setup_end_tag (bd_t *bd)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */
