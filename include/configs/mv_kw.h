/*
 * (C) Copyright 2003
 * Texas Instruments.
 * Kshitij Gupta <kshitij@ti.com>
 * Configuation settings for the TI OMAP Innovator board.
 *
 * (C) Copyright 2004
 * ARM Ltd.
 * Philippe Robin, <philippe.robin@arm.com>
 * Configuration for Integrator AP board.
 *.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
ror "Both CONFIG_ENV_SECT_SIZE and CONFIG_ENV_SIZE undefined
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "../../board/mv_feroceon/mv_kw/config/mvSysHwConfig.h"

/************/
/* VERSIONS */
/************/
#define CONFIG_IDENT_STRING	" Marvell version: 0.0.8_NQ"

/* version number passing when loading Kernel */
#define VER_NUM 0x09080000           /* 2009.08 */

/**********************************/
/* Marvell Monitor Extension      */
/**********************************/
#define enaMonExt()( /*(!getenv("enaMonExt")) ||\*/\
		     ( getenv("enaMonExt") && \
                       ((!strcmp(getenv("enaMonExt"),"yes")) ||\
		       (!strcmp(getenv("enaMonExt"),"Yes"))) \
		     )\
		    )

/********/
/* CLKs */
/********/
#ifndef __ASSEMBLY__
extern unsigned int mvSysClkGet(void);
extern unsigned int mvTclkGet(void);
#define UBOOT_CNTR		0		/* counter to use for uboot timer */
#define MV_TCLK_CNTR		1		/* counter to use for uboot timer */
#define MV_REF_CLK_DEV_BIT	1000		/* Number of cycle to eanble timer */
#define MV_REF_CLK_BIT_RATE	100000		/* Ref clock frequency */
#define MV_REF_CLK_INPUT_GPP	6		/* Ref clock frequency input */

#define CONFIG_SYS_HZ			1000
#define CONFIG_SYS_TCLK                mvTclkGet()
#define CONFIG_SYS_BUS_HZ              mvSysClkGet()
#define CONFIG_SYS_BUS_CLK             CONFIG_SYS_BUS_HZ
#endif
#define CONFIG_DISPLAY_CPUINFO
#define MV_KW1
/********************/
/* Dink PT settings */
/********************/
#define MV_PT

#ifdef MV_PT
#define MV_PT_BASE(cpu)  (CONFIG_SYS_MALLOC_BASE - 0x20000 - (cpu*0x20000))
#endif /* #ifdef MV_PT */


/*************************************/
/* High Level Configuration Options  */
/* (easy to change)		     */
/*************************************/
#define CONFIG_MARVELL
#define CONFIG_ARM926EJS

/* commands */

#define CONFIG_BOOTP_MASK	(CONFIG_BOOTP_DEFAULT | \
				 CONFIG_BOOTP_BOOTFILESIZE)

#define CONFIG_CMD_DHCP
#define CONFIG_CMD_I2C
#define CONFIG_CMD_EEPROM
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_DATE
#define CONFIG_CMD_LOADS
#define CONFIG_CMD_BSP
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_BOOTD
#define CONFIG_CMD_CONSOLE
#define CONFIG_CMD_RUN
#define CONFIG_CMD_MISC
#define CONFIG_CMD_RCVR
#define CONFIG_CMD_SAR


#if (defined(MV_INCLUDE_PEX) | defined(MV_INCLUDE_PCI))
#define CONFIG_PCI           /* pci support               */
#define CONFIG_CMD_PCI
#endif
#if defined (MV_INC_BOARD_PCI_SATA) || defined (MV_INCLUDE_INTEG_SATA)
#define CONFIG_CMD_IDE
#define CONFIG_CMD_EXT2
#endif

//#if defined(MV_MMC)
#define CONFIG_CMD_MMC 
//#endif

#define CONFIG_CMD_FAT


/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <config_cmd_default.h>

#define	CONFIG_SYS_MAXARGS	16		/* max number of command args	*/

/*-----------------------------------------------------------------------
 * IDE/ATA stuff (Supports IDE harddisk on PCMCIA Adapter)
 *-----------------------------------------------------------------------
 */

#undef	CONFIG_IDE_8xx_PCCARD		/* Use IDE with PC Card	Adapter	*/

#undef	CONFIG_IDE_8xx_DIRECT		/* Direct IDE    not supported	*/
#undef	CONFIG_IDE_LED			/* LED   for ide not supported	*/
#undef	CONFIG_IDE_RESET		/* reset for ide not supported	*/

#define CONFIG_SYS_IDE_MAXBUS		4	/* max. 1 IDE bus		*/
#define CONFIG_SYS_IDE_MAXDEVICE	CONFIG_SYS_IDE_MAXBUS * 8	/* max. 1 drive per IDE bus	*/

#define CONFIG_SYS_ATA_IDE0_OFFSET	0x0000

#undef CONFIG_MAC_PARTITION
#define CONFIG_DOS_PARTITION

/* For 6121/6145 support */
#define CONFIG_SCSI_AHCI
#ifdef CONFIG_SCSI_AHCI
#define CONFIG_SATA_6121
#define CONFIG_SYS_SCSI_MAX_SCSI_ID	4
#define CONFIG_SYS_SCSI_MAX_LUN	1
#define CONFIG_SYS_SCSI_MAX_DEVICE 	(CONFIG_SYS_SCSI_MAX_SCSI_ID * CONFIG_SYS_SCSI_MAX_LUN)
//#define SCSI_MAXDEVICE	CONFIG_SYS_SCSI_MAX_DEVICE
#endif

/* which initialization functions to call for this board */
#define CONFIG_MISC_INIT_R	1      /* after relloc initialization*/
#undef CONFIG_DISPLAY_MEMMAP    /* at the end of the bootprocess show the memory map*/

#define CONFIG_ENV_OVERWRITE    /* allow to change env parameters */

#undef	CONFIG_WATCHDOG		/* watchdog disabled		*/

/* Cache */
#define CONFIG_SYS_CACHELINE_SIZE	32	


/* global definetions. */
#define	CONFIG_SYS_SDRAM_BASE		0x00000000


#define CONFIG_SYS_RESET_ADDRESS	0xffff0000

#define CONFIG_SYS_MALLOC_BASE		(TEXT_BASE + (1 << 20)) /* TEXT_BASE + 1M */

/*
 * When locking data in cache you should point the INIT_RAM_ADDRESS
 * To an unused memory region. The stack will remain in cache until RAM
 * is initialized 
*/
#define CONFIG_SYS_MALLOC_LEN		(1 << 20)	/* (default) Reserve 1MB for malloc*/

#define CONFIG_SYS_GBL_DATA_SIZE	128  /* size in bytes reserved for init data */

#undef CONFIG_INIT_CRITICAL		/* critical code in start.S */


/********/
/* DRAM */
/********/

#define CONFIG_SYS_DRAM_BANKS		4

/* this defines whether we want to use the lowest CAL or the highest CAL available,*/
/* we also check for the env parameter CASset.					  */
#define MV_MIN_CAL

#define CONFIG_SYS_MEMTEST_START     0x00400000
#define CONFIG_SYS_MEMTEST_END       0x007fffff

/********/
/* RTC  */
/********/
#if defined(CONFIG_CMD_DATE)
#define CONFIG_SYS_NVRAM_SIZE  0x00 /* dummy */
#define CONFIG_SYS_NVRAM_BASE_ADDR DEVICE_CS1_BASE /* dummy */
#endif /* #if defined(CONFIG_CMD_DATE) */

/********************/
/* Serial + parser  */
/********************/
/*
 * The following defines let you select what serial you want to use
 * for your console driver.
 */
#define CONFIG_BAUDRATE         115200   /* console baudrate = 115200    */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600 }
 
#ifdef DB_FPGA
#define CONFIG_SYS_DUART_CHAN		1		/* channel to use for console */
#else
#define CONFIG_SYS_DUART_CHAN		0		/* channel to use for console */
#endif

#define CONFIG_SYS_INIT_CHAN1
#define CONFIG_SYS_INIT_CHAN2


#define CONFIG_LOADS_ECHO       0       /* echo off for serial download */
#define CONFIG_SYS_LOADS_BAUD_CHANGE           /* allow baudrate changes       */

#define CONFIG_SYS_CONSOLE_INFO_QUIET  /* don't print In/Out/Err console assignment. */
#undef CONFIG_SILENT_CONSOLE  /* define for Pex complince */

/* parser */
/* don't chang the parser if you want to load Linux(if you cahnge it to HUSH the cmdline will
	not pass to the kernel correctlly???) */
/*#define CONFIG_SYS_HUSH_PARSER */
#undef CONFIG_SYS_HUSH_PARSER 
#define CONFIG_AUTO_COMPLETE

#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "

#define	CONFIG_SYS_LONGHELP			/* undef to save memory		*/
#define	CONFIG_SYS_PROMPT	"Marvell>> "	/* Monitor Command Prompt	*/
#define	CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size	*/
#define	CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buffer Size */

/************/
/* ETHERNET */
/************/
/* to change the default ethernet port, use this define (options: 0, 1, 2) */
#define CONFIG_NET_MULTI
#define CONFIG_IPADDR		10.4.50.165
#define CONFIG_SERVERIP		10.4.50.5
#define CONFIG_NETMASK		255.255.255.0
#define ETHADDR          	"00:00:00:00:51:81"
#define ETH1ADDR          	"00:00:00:00:51:82"
#define ENV_ETH_PRIME			"egiga0"

/************/
/* PCI	    */
/************/
#if defined(MV_INCLUDE_PCI)

#if defined(DB_PEX_PCI)
#define ENV_PCI_MODE	"device"	/* PCI */
#else
#define ENV_PCI_MODE	"host"		/* PCI */
#endif

#endif

/************/
/* USB	    */
/************/
#ifdef MV_USB
#define MV_INCLUDE_USB
#define CONFIG_CMD_USB
#define CONFIG_USB_STORAGE
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_MARVELL
#define CONFIG_EHCI_IS_TDI
#define CONFIG_USB_STORAGE
#define CONFIG_DOS_PARTITION
#define CONFIG_ISO_PARTITION
#define CONFIG_SUPPORT_VFAT
#define ENV_USB0_MODE	"host"
#else
#undef MV_INCLUDE_USB
#undef CONFIG_CMD_USB
#undef CONFIG_USB_STORAGE
#endif


/************/
/* SDIO/MMC */
/************/

//#if defined(MV_88F6183) || defined(MV_88F6183L)

#if defined(CONFIG_CMD_MMC)
	#define CONFIG_MMC
	//#define CONFIG_GENERIC_MMC
	#define CONFIG_SYS_MMC_BASE 0xF0000000
#else
	#define CONFIG_LBA48
#endif

//#endif

/***************************************/
/* LINUX BOOT and other ENV PARAMETERS */
/***************************************/
#define MV_BOOTARGS_END ":::DB88FXX81:eth0:none"
#define MV_BOOTARGS_END_SWITCH ":::RD88FXX81:eth0:none"
#define RCVR_IP_ADDR "169.254.100.100"
#define	RCVR_LOAD_ADDR	"0x02000000"

#define CONFIG_ZERO_BOOTDELAY_CHECK

#define	CONFIG_SYS_LOAD_ADDR		0x02000000	/* default load address	*/

#undef	CONFIG_BOOTARGS

/* auto boot*/
#define CONFIG_BOOTDELAY	3 		/* by default no autoboot */

#if (CONFIG_BOOTDELAY >= 0)
#define CONFIG_ROOTPATH	/mnt/ARM_FS/
#endif /* #if (CONFIG_BOOTDELAY >= 0) */

#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size	*/

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define	CONFIG_SYS_BOOTMAPSZ		(8<<20)	/* Initial Memory map for Linux */

#define BRIDGE_REG_BASE_BOOTM 0xfbe00000 /* this paramaters are used when booting the linux kernel */ 

#define CONFIG_CMDLINE_TAG				1       /* enable passing of ATAGs  */
#define CONFIG_INITRD_TAG				1	/* enable INITRD tag for ramdisk data */
#define CONFIG_SETUP_MEMORY_TAGS		1
#define CONFIG_MARVELL_TAG				1
#define ATAG_MARVELL					0x41000403

/********/
/* I2C  */
/********/
#if defined(CONFIG_CMD_I2C)
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_SYS_MAX_I2C_BUS			2
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	1
#define CONFIG_SYS_I2C_MULTI_EEPROMS
#define CONFIG_SYS_I2C_SPEED			100000		/* I2C speed default */

/* I2C addresses for the two DIMM SPD chips */
#define DIMM0_I2C_ADDR			0x56
#define DIMM1_I2C_ADDR			0x54

/* CPU I2C settings */
//#define CPU_I2C  
#define I2C_CPU0_EEPROM_ADDR    	0x51
#endif

/********/
/* PCI  */
/********/
#ifdef CONFIG_PCI
 #define CONFIG_PCI_HOST PCI_HOST_FORCE /* select pci host function     */
 #define CONFIG_PCI_PNP          	/* do pci plug-and-play         */

/* Pnp PCI Network cards */
 #define CONFIG_EEPRO100		/* Support for Intel 82557/82559/82559ER chips */
#define CONFIG_E1000		/* Support for Intel 82557/82559/82559ER chips */
 #define CONFIG_SK98			/* yukon */
 #define YUK_ETHADDR         		"00:00:00:EE:51:81"
 #define CONFIG_DRIVER_RTL8029 

/* DB_PRPMC support only Yukon */
#if defined (DB_PRPMC) || defined (DB_MNG)
 #undef CONFIG_EEPRO100	
 #undef CONFIG_DRIVER_RTL8029 
#endif

#endif /* #ifdef CONFIG_PCI */

#define PCI_HOST_ADAPTER 0              /* configure ar pci adapter     */
#define PCI_HOST_FORCE   1              /* configure as pci host        */
#define PCI_HOST_AUTO    2              /* detected via arbiter enable  */

/* for Yukon */
#define __mem_pci(x) x
#define __io_pci(x) x
#define __arch_getw(a)			(*(volatile unsigned short *)(a))
#define __arch_putw(v,a)		(*(volatile unsigned short *)(a) = (v))



/***********************/
/* FLASH organization  */
/***********************/
/*
 * When CONFIG_SYS_MAX_FLASH_BANKS_DETECT is defined, the actual number of Flash
 * banks has to be determined at runtime and stored in a gloabl variable
 * mv_board_num_flash_banks. The value of CONFIG_SYS_MAX_FLASH_BANKS_DETECT is only
 * used instead of CONFIG_SYS_MAX_FLASH_BANKS to allocate the array flash_info, and
 * should be made sufficiently large to accomodate the number of banks that
 * might actually be detected.  Since most (all?) Flash related functions use
 * CONFIG_SYS_MAX_FLASH_BANKS as the number of actual banks on the board, it is
 * defined as mv_board_num_flash_banks.
 */
#define CONFIG_SYS_MAX_FLASH_BANKS_DETECT	5
#ifndef __ASSEMBLY__
extern int mv_board_num_flash_banks;
#endif
#define CONFIG_SYS_MAX_FLASH_BANKS 1
//(mv_board_num_flash_banks)
#define CONFIG_SYS_MAX_FLASH_SECT	300	/* max number of sectors on one chip */
//#define CONFIG_SYS_FLASH_PROTECTION

//#define CONFIG_SYS_EXTRA_FLASH_DEVICE	DEVICE3	/* extra flash at device 3 */
//#define CONFIG_SYS_EXTRA_FLASH_WIDTH	4	/* 32 bit */
//#define CONFIG_SYS_BOOT_FLASH_WIDTH	1	/* 8 bit */

//#define CONFIG_SYS_FLASH_ERASE_TOUT	120000/1000	/* 120000 - Timeout for Flash Erase (in ms) */
//#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* 500 - Timeout for Flash Write (in ms) */
//#define CONFIG_SYS_FLASH_LOCK_TOUT	500	/* 500- Timeout for Flash Lock (in ms) */

//#if !defined(MV_SPI) && !defined(MV_NAND) CONFIG_SPI

#define CONFIG_SYS_NO_FLASH
#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_IMLS

//#define CONFIG_CMD_JFFS2

/* SPI Flash configuration   */
/*****************************/
#if defined(MV_SPI)
	#define CONFIG_CMD_SPI
	#define CONFIG_CMD_SF
	#define CONFIG_SPI_FLASH
	#define CONFIG_SPI_FLASH_STMICRO

	#define CONFIG_ENV_SPI_MAX_HZ		20000000	/*Max 50Mhz- will sattle on SPI bus max 41.5Mhz */
	#define CONFIG_ENV_SPI_CS		0
	#define CONFIG_ENV_SPI_BUS		0
	//#define MV_SPI_CONN_TO_EXT_FLASH

	/* Boot from SPI settings */
	#if defined(MV_SPI_BOOT)
		#define CONFIG_ENV_IS_IN_SPI_FLASH

		#if defined(MV_SEC_128K)
			#define CONFIG_ENV_SECT_SIZE			0x20000
		#elif defined(MV_SEC_256K)
			#define CONFIG_ENV_SECT_SIZE			0x40000
		#endif

		#define CONFIG_ENV_SIZE		CONFIG_ENV_SECT_SIZE
		#define CONFIG_ENV_OFFSET	(768 << 10)	/* environment starts here  */
		#define MONITOR_HEADER_LEN	0x200
		#define MV_MONITOR_IMAGE_DST	TEXT_BASE 	/* Load NUB to this addr	*/
		#define CONFIG_SYS_MONITOR_LEN			(512 << 10)	/* Reserve 512 kB for Monitor */

		#ifdef MV_BOOTROM
			#define CONFIG_SYS_FLASH_BASE		DEVICE_SPI_BASE
			#define CONFIG_SYS_FLASH_SIZE      	_16M	
			#define MV_MONITOR_IMAGE_OFFSET	0	/* offset of the monitor from the u-boot image */
			#define CONFIG_SYS_MONITOR_BASE		0
		//(CONFIG_SYS_FLASH_BASE)
			#define CONFIG_ENV_ADDR    		(CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN)
		#else
			#define CONFIG_SYS_FLASH_BASE		BOOTDEV_CS_BASE
			#define CONFIG_SYS_FLASH_SIZE      	BOOTDEV_CS_SIZE
			#define MV_MONITOR_IMAGE_OFFSET	CONFIG_ENV_SECT_SIZE	/* offset of the monitor from the u-boot image */
			#define CONFIG_SYS_MONITOR_BASE		0
		//(CONFIG_SYS_FLASH_BASE + CONFIG_SYS_FLASH_SIZE - CONFIG_SYS_MONITOR_LEN)
			#define CONFIG_ENV_ADDR    		(CONFIG_SYS_MONITOR_BASE - CONFIG_ENV_SECT_SIZE)
		#endif /* ifdef MV_BOOTROM */
	#endif //#if defined(MV_SPI_BOOT)
#endif //#if defined(MV_SPI)

/* CFI FLASH organization  */
/***************************/
#undef CONFIG_FLASH_CFI_DRIVER
//#define CONFIG_SYS_FLASH_CFI
//#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE
//#define CONFIG_SYS_FLASH_QUIET_TEST
//#define CONFIG_SYS_FLASH_BANKS_LIST	{BOOTDEV_CS_BASE}

/* NAND-FLASH stuff     */
/************************/
#if defined(MV_NAND)
	#define CONFIG_SYS_MAX_NAND_DEVICE 1       /* Max number of NAND devices */
	#define CONFIG_SYS_NAND_MAX_CHIPS CONFIG_SYS_MAX_NAND_DEVICE

	#define CONFIG_CMD_NAND

	/* JFFS2 over NAND define */ 
	//#define CONFIG_JFFS2_NAND

	/* Boot from NAND settings */
	#if defined(MV_NAND_BOOT)

		#define CONFIG_ENV_IS_IN_NAND
		#define CONFIG_SKIP_BAD_BLOCK
		#define CONFIG_SYS_64BIT_VSPRINTF

		#if defined(MV_BOOTROM)
			#if defined(MV_LARGE_PAGE)
				#define CONFIG_ENV_OFFSET				(640 << 10)	/* environment starts here  */
				#define CONFIG_ENV_SECT_SIZE			(128 << 10)	/* environment take 1 block */
				#define CONFIG_ENV_SIZE				CONFIG_ENV_SECT_SIZE /* environment take 1 block */	
				#define MV_NBOOT_BASE				0	
				#define MV_NBOOT_LEN				(4 << 10)	/* Reserved 4KB for boot strap */
				#define	CONFIG_SYS_MONITOR_LEN				(640 << 10)	/* Reserve 4 * 128KB + ENV = 640KB for Monitor */
				#define CONFIG_SYS_MONITOR_BASE			0
				#define MV_MONITOR_IMAGE_OFFSET		0 /* offset of the monitor from the u-boot image */
				#define MV_MONITOR_IMAGE_DST			TEXT_BASE 	/* Load NUB to this addr	*/
				#define CONFIG_ENV_ADDR    			CONFIG_ENV_OFFSET
			#else /* ! LARGE PAGE NAND */
				#define CONFIG_ENV_OFFSET				0x84000	/* environment starts here  */
				#define MV_NBOOT_BASE				0	
				#define MV_NBOOT_LEN				(16 << 10)	/* Reserved 16kB for boot strap */
				#define	CONFIG_SYS_MONITOR_LEN				(512 << 10)	/* Reserve 512 kB for Monitor */
				#define CONFIG_SYS_MONITOR_BASE			(MV_NBOOT_BASE + MV_NBOOT_LEN)
				#define CONFIG_ENV_SECT_SIZE			0x4000
				#define MV_MONITOR_IMAGE_OFFSET		0 /* offset of the monitor from the u-boot image */
				#define CONFIG_ENV_ADDR    			0x84000 /* UBOOT_IMAGE_DEST - UBOOT_IMAGE_OFFS */
			#endif /* defined(MV_LARGE_PAGE) */
		#else /* ! MV_BOOTROM */
			#if defined(MV_LARGE_PAGE)
				#define CONFIG_ENV_OFFSET				(640 << 10)	/* environment starts here  */
				#define CONFIG_ENV_SECT_SIZE			(128 << 10)	/* environment take 1 block */
				#define CONFIG_ENV_SIZE				CONFIG_ENV_SECT_SIZE /* environment take 1 block */	
				#define MV_NBOOT_BASE				0	
				#define MV_NBOOT_LEN				(4 << 10)	/* Reserved 4KB for boot strap */
				#define	CONFIG_SYS_MONITOR_LEN				(640 << 10)	/* Reserve 4 * 128KB + ENV = 640KB for Monitor */
				#define CONFIG_SYS_MONITOR_BASE			0
				#define MV_MONITOR_IMAGE_OFFSET		0 /* offset of the monitor from the u-boot image */
				#define MV_MONITOR_IMAGE_DST			TEXT_BASE	/* Load NUB to this addr	*/
				#define CONFIG_ENV_ADDR    			CONFIG_ENV_OFFSET
			#else /* ! LARGE PAGE NAND */
				#define CONFIG_ENV_OFFSET				(16 << 10)	/* environment starts here  */
				#define CONFIG_ENV_SECT_SIZE			(128 << 10)
				#define CONFIG_ENV_SIZE				CONFIG_ENV_SECT_SIZE /* environment take 1 block */	
				#define MV_NBOOT_BASE				0	
				#define MV_NBOOT_LEN				(4 << 10)	/* Reserved 16kB for boot strap */
				#define	CONFIG_SYS_MONITOR_LEN				(640 << 10)	/* Reserve 4 * 128KB + ENV = 640KB for Monitor */
				#define CONFIG_SYS_MONITOR_BASE			(CONFIG_ENV_OFFSET)
				#define MV_MONITOR_IMAGE_OFFSET		CONFIG_ENV_SECT_SIZE /* offset of the monitor from the u-boot image */
				#define MV_MONITOR_IMAGE_DST			TEXT_BASE - CONFIG_ENV_SECT_SIZE	/* Load NUB to this addr	*/
				#define CONFIG_ENV_ADDR    			MV_MONITOR_IMAGE_DST
			#endif /* defined(MV_LARGE_PAGE) */
		#endif /* defined(MV_BOOTROM) */
		#define CONFIG_ENV_RANGE CONFIG_ENV_SIZE *16
	#endif /* MV_NAND_BOOT */
/*
#if defined(RD_88F5181L_FE) || defined(RD_88F5181L_GE) || defined(RD_88F5181L_FXO_GE) || \
		defined(RD_88F5181_GTWGE) || defined (RD_88F5181_GTWFE)
 #define BOARD_LATE_INIT
#endif*/
#endif //#if defined(MV_NAND)
/*****************/
/* others        */
/*****************/
#define MV_DFL_REGS		0xd0000000 	/* boot time MV_REGS */
#define MV_REGS		INTER_REGS_BASE /* MV Registers will be mapped here */

#undef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE	(1 << 20)	/* regular stack - up to 4M (in case of exception)*/
#define CONFIG_NR_DRAM_BANKS 	4

#endif	/* __CONFIG_H */
