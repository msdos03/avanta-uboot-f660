/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
 * Boot support
 */
#include <config.h>
#include <common.h>
#include <command.h>


/* This routine checks the boot environment to determine which
 * kernel to use and which root file system to use.
 *
 *    SYSRSTn = 0xf1010050  ; write of 0xffffffff to clear
 *            == 0x0008000x on watchdog or internal reboot
 *            == 0x00000005 on power on reset (or about 5)
 *    Watchdog counter address == 0xf1020324  (set to ffffffff for 21 seconds)
 *    Watchdog reset enable addr == 0xf1020108   (set to 2 to enable reset)
 *    Watchdog counter enable == 0xf1020300  (set to 0x10 to start counter)
 *
 */

// gpio data in
// 0x18110, 0x18150, 0x18190
// reset button
#define RST_BTN_ADDR    0xf1018150
#define RST_BTN_MASK        (1 << 31)
// WPS button
#define WPS_BTN_ADDR    0xf1018110
#define WPS_BTN_MASK        (1 << 27)
#define MVKW_SYSRST_DUR 0xf1018250
#define MVKW_RSTOUT     0xf1020108
#define MVKW_WDG        0xf1020324
#define MVKW_WDGEN      0xf1020300
#define	RST_NOSET	0	/* Unset type of reset */
#define RST_PWR		1	/* power on reset */
#define RST_DOG		2	/* watchdog timer, Linux reboot, uboot reset */
#define FSUNSET		0	/* Target root fs is not yet set */
#define FSACTIVE	1	/* Root file system in use should be "active" */
#define FSFAILSAFE	2	/* Root file system in use should be "failsafe" */
#define BLEN            100     /* String buffer length */

void enable_watchdog(int flag);
void msleep(int);

char ethaddr[64];
char boot_args[256];


ulong sgd_magic = 0; 
extern int dramBoot;
int do_watchdog;
int fake_boardid;

int aei_boardid(void)
{
int id;
 id = mvBoardIdGet();
 if (fake_boardid) 
 {
  printf("Fake ID is set.\n");
  id -= 1;
 }
 return id;
}

void aei_setup ()
{
 ulong      val;
 /* ==1 for power on, ==2 for watchdog, ==3 for user held >10 seconds */
 ulong      rsttype;     
 ulong      fstarget;    /* target file system: active or failsafe */
 /* set !=0 if we got here due to a deliberate watchdog timeout */
 ulong      deliberate = 0; 
 ulong      dogcount = 0; /* the number of failed boot of a watchdog reset */
 char      *tmpstr;
 char      *fsstr;       /* environment string with "active" or "failsafe" */
 char       buf[BLEN];   /* generic string buffer */
 ulong      ret;         /* generic subroutine return value */
 ulong      i;           /* generic counter */
 int do_saveenv;
 char *myargv[8], *cp;
 int wps_button, rst_button;

        do_saveenv = 0;
	tmpstr = getenv("ethaddr");
	fsstr = getenv("eth1addr");
	if (strcmp(tmpstr, fsstr))
        {
         /* make a copy so that saveenv works */
         strcpy(ethaddr, tmpstr);
         setenv("eth1addr", ethaddr);
         do_saveenv = 1;
        }
	tmpstr = getenv("sgd_app");
        if ( (!tmpstr) || (strcmp(tmpstr, "1")))
        {
         setenv("sgd_app", "1");
         do_saveenv = 1;
        }
	tmpstr = getenv("sgd_fw");
        cp = SGD_FW_VER; cp += 7;
        if ( (!tmpstr) || (strcmp(tmpstr, cp)))
        {
         setenv("sgd_fw", cp);
         do_saveenv = 1;
        }
        if (do_saveenv) saveenv();
        // in case mtd is not called
        //{
         //cmd_tbl_t *cmd;
         //cmd = find_cmd("mtd");
         //if (cmd)
         //{
          //myargv[0] = "mtd";
          //myargv[1] = NULL;
          //cmd->cmd(cmd, 0, 1, myargv);
         //}
        //}
        if (sgd_magic == 0)
        {
	 tmpstr = getenv("sgd_magic");
         if (tmpstr) sgd_magic = simple_strtoul(tmpstr, NULL, 16);
         if (sgd_magic == 0) 
         {
          cp = strchr(SGD_MAGIC, '=');
          sgd_magic = simple_strtoul(cp+1, NULL, 16);
          //sgd_magic = 0xc00010;
         }
        }
	/* Get the type of reset */
	//if (*((ulong *) MVKW_SYSRST_DUR) > 0x00100000 )
	if (*((ulong *) MVKW_SYSRST_DUR) < 0x00050000 )
        //if (dramBoot != 1)
		rsttype = RST_PWR;
	else
		rsttype = RST_DOG;
	/* Clear the reset-duration counter for the next time */
	//*((ulong *) MVKW_SYSRST_DUR) = 0xffffffff;
	*((ulong *) MVKW_SYSRST_DUR) = 0x80000000;

	/* Get the current target file system and kernel location */
	fsstr = getenv("ubi_root");
	fstarget = (!strcmp("active", fsstr)) ? FSACTIVE : FSFAILSAFE ;

	/* Before this command runs we should have mounted ubi0:config */
	//ret = ubifs_ls("deliberate_reboot");
	val = * (unsigned long *) sgd_magic;
        val &= 0xFFFFFF00;
	deliberate = 1;  dogcount = 0;
        if (val == 0xAA55AA00)
        {
	 val = * (unsigned long *) sgd_magic;
         val &= 0x000000FF;
         if (val & 1) deliberate = 1; else deliberate = 0;
         dogcount = val >> 1;
        }

        if (dogcount > 3) dogcount = 3;
	//tmpstr = getenv("dogcount");
	//if (tmpstr) { dogcount = simple_strtoul(tmpstr, NULL, 10); }

	/* At this point we have all of the information that we need. */
	/* Use our failsafe logic to determine what to do */
	/* (Note: doing a setenv() invalidates any getenv() string pointers) */

	/* If a cold start, then return to do a normal boot */
	if (rsttype == RST_PWR) {
		/* User might be holding buttons down. */
                * (unsigned long *) sgd_magic = 0xAA55AA00;
		i = 3000;
		while (i != 0) {
		        rst_button = *((ulong *) RST_BTN_ADDR);
                        rst_button &= RST_BTN_MASK;
		        wps_button = *((ulong *) WPS_BTN_ADDR);
		        wps_button &= WPS_BTN_MASK;
		        wps_button = 1;
			/* normal boot if buttons not both held */
			if ((rst_button) && (wps_button)) { //non-zero:released
				(void) setenv("bootmode", "pwron");
		                printf("Power-on to active\n");
				enable_watchdog(1);
				return;
			}
			msleep(1);  //sleep for 1 millisecond
			i--;
		}
		/* Both buttons held for 5 seconds.  Go to failsafe. */
		if (wps_button == 0)
		{
		 (void) setenv("ubi_root", "failsafe");
		 (void) setenv("bootmode", "pwron-button");
		 printf("Power-on with buttons boot to failsafe\n");
		}
		else
		{
		 (void) setenv("ubi_root", "failsafe");
		 (void) setenv("bootmode", "rma");
		 printf("Power-on with buttons boot to RMA\n");
		}
		return;
	}
        if (do_saveenv)
        {
         dogcount = 0;
         deliberate = 1;
        }

	/* If deliberate_reboot is set, then return and do a normal reboot */
	if (deliberate && (dogcount == 0)) {
		printf("Deliberate reboot to %s\n", (fstarget == FSACTIVE) ? "active" : "failsafe" );
		dogcount = 1;
		//(void) sprintf(buf, "%d", dogcount + 1);
                * (unsigned long *) sgd_magic = 0xAA55AA00 | (dogcount << 1);
		//(void) setenv("dogcount", buf);
		//(void) saveenv();
		(void) setenv("bootmode", "reboot");
		enable_watchdog(1);
		return;
	}


	/* At this point we know something is wrong */


	/* We don't want a failed system to cause a lot of NAND writes. */
	/* Only save dogcount if still in first three attempts */
	if (dogcount < 3) {
          dogcount++;
		//(void) sprintf(buf, "%d", dogcount + 1);
		//(void) setenv("dogcount", buf);
		//(void) saveenv();
	}
        * (unsigned long *) sgd_magic = 0xAA55AA00 | (dogcount << 1);

	/* If this is our first or second unplanned watchdog (dogcount<2), then try */
	/* try to do a failsafe boot */
	if (dogcount <= 1) {
		printf("Watchdog reboot to failsafe\n");
		(void) setenv("bootmode", "error");
		(void) setenv("ubi_root", "failsafe");
		printf("Reset boot to failsafe dogcount 1\n");
		enable_watchdog(1);
	}

	/* If our third boot attempt, then failsafe booting is failing. Try active */
	else if (dogcount == 2) {
		printf("Watchdog reboot to active -- dogcount 2!\n");
		(void) setenv("bootmode", "error");
		(void) setenv("ubi_root", "active");
		enable_watchdog(1);
	}

	/* If neither partition is booting (dogcount=3) try failsafe without a watchdog */
	else {
		printf("Watchdog reboot to failsafe -- watchdog disabled -- good luck!\n");
		(void) setenv("bootmode", "error");
		(void) setenv("ubi_root", "failsafe");
	}

	/* The environment is set.  Go try to boot */
	return;
}


/*
 * Enable the watchdog with a 21 second timeout
 */
void enable_watchdog(int flag)
{
 if (flag == 2)
 {
  if (do_watchdog)
  {
   printf("Watchdog is enabled\n");
   *((ulong *) MVKW_WDG) = 0xffffffff;     // 21 seconds
   *((ulong *) MVKW_RSTOUT) |= 2;           // Causes a reset
   *((ulong *) MVKW_WDGEN) |= 0x00000010;   // Start the counter
  }
 }
 else if (flag == 1)
 {
  char *cp;
  cp = getenv("no_watchdog");
  if ((!cp) || (strcmp(cp, "1")))
  {
   cp = getenv("sgd_state");
   if ((!cp) || (strcmp(cp, "1"))) do_watchdog = 1;
  }
 }
 else if (flag == 0)
 {
  do_watchdog = 0;
 }
 else if (flag == -1)
 {
   printf("Watchdog is disabled\n");
   do_watchdog = 0;
   *((ulong *) MVKW_RSTOUT) &= ~2;
   *((ulong *) MVKW_WDGEN) &= ~0x00000010;
 }
}

/*
 * msleep for N millisecondsseconds
 */
void msleep(int n)
{
        ulong start = get_timer(0);
        ulong delay;

        delay = n * CONFIG_SYS_HZ / 1000;

        while (get_timer(start) < delay) {
                udelay (100);
        }

        return;
}


/* not sure a single bootm will hang */
/* So we just run bootm twice by first booting it to an non-existing address */
void ram_boot(int argc, char **argv)
{
        cmd_tbl_t *cmd;
	char *myargv[8];
	char *cp;
	char *iname;

	if (argc > 2) iname = argv[2]; else iname = "uImage";
        //cp = "console=ttyS0,115200 "
         //"mv_net_config=1,(00:50:43:11:11:11,0:1:2:3),mtu=1500";
        cp = "console=ttyS0,115200 $(nandparts) rw "
         "mv_net_config=1,(00:50:43:11:11:11,0:1:2:3),mtu=1500 "
         "myip=$(ipaddr) sip=$(serverip) zid=$(zhomeid)";
        setenv("bootargs", cp);
        cmd = find_cmd("ubi");
        myargv[0] = "ubi";
        myargv[1] = "part";
        myargv[2] = "ubi";
        myargv[3] = NULL;
        cmd->cmd(cmd, 0, 3, myargv);
        cmd = find_cmd("ubifsmount");
        myargv[0] = "ubifsmount";
        myargv[1] = "failsafe";
        myargv[2] = NULL;
        cmd->cmd(cmd, 0, 2, myargv);
        cmd = find_cmd("ubifsload");
        myargv[0] = "ubifsload";
        myargv[1] = "0x30000000";
        myargv[2] = "/boot/minImage";
        myargv[3] = NULL;
        cmd->cmd(cmd, 0, 3, myargv);
        cmd = find_cmd("bootm");
        myargv[0] = "bootm";
        myargv[1] = "0x30000000";
        myargv[2] = NULL;
        cmd->cmd(cmd, 0, 2, myargv);
        cmd = find_cmd("ubifsload");
        myargv[0] = "ubifsload";
        myargv[1] = "0x800000";
        myargv[2] = "/boot/minImage";
        myargv[3] = NULL;
        cmd->cmd(cmd, 0, 3, myargv);
        cmd = find_cmd("bootm");
        myargv[0] = "bootm";
        myargv[1] = "0x800000";
        myargv[2] = NULL;
        cmd->cmd(cmd, 0, 2, myargv);
}

/* not sure a single bootm will hang */
/* So we just run bootm twice by first booting it to an non-existing address */
void net_boot(int argc, char **argv)
{
        cmd_tbl_t *cmd;
	char *myargv[8];
	char *cp;
	char *iname;

	if (argc > 2) iname = argv[2]; else iname = "uImage";
        cp = "console=ttyS0,115200 $(nandparts) rw "
         "mv_net_config=1,(00:50:43:11:11:11,0:1:2:3),mtu=1500 "
         "myip=$(ipaddr) sip=$(serverip) zid=$(zhomeid)";
        setenv("bootargs", cp);
        cmd = find_cmd("tftp");
        myargv[0] = "tftp";
        myargv[1] = "0x30000000";
        myargv[2] = iname;
        myargv[3] = NULL;
        cmd->cmd(cmd, 0, 3, myargv);
        cmd = find_cmd("mtd");
        myargv[0] = "mtd";
        myargv[1] = NULL;
        cmd->cmd(cmd, 0, 1, myargv);
        cmd = find_cmd("bootm");
        myargv[0] = "bootm";
        myargv[1] = "0x30000000";
        myargv[2] = NULL;
        cmd->cmd(cmd, 0, 2, myargv);
        cmd = find_cmd("tftp");
        myargv[0] = "tftp";
        myargv[1] = "0x800000";
        myargv[2] = iname;
        myargv[3] = NULL;
        cmd->cmd(cmd, 0, 3, myargv);
        cmd = find_cmd("bootm");
        myargv[0] = "bootm";
        myargv[1] = "0x800000";
        myargv[2] = NULL;
        cmd->cmd(cmd, 0, 2, myargv);
}

int aei_boot ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	DECLARE_GLOBAL_DATA_PTR;
        cmd_tbl_t *cmd;
	char *myargv[8];
	int ret;
	unsigned long val;
	char *cp, tcp;
	int app_flag;

	app_flag = 1;
	setenv("no_watchdog", "1");
	enable_watchdog(0);
        fake_boardid = 0;
	cp = getenv("sgd_magic");
        if (cp) sgd_magic = simple_strtoul(cp, NULL, 16);
        if (sgd_magic == 0) 
        {
         cp = strchr(SGD_MAGIC, '=');
         sgd_magic = simple_strtoul(cp+1, NULL, 16);
        }
	val = * (unsigned long *) sgd_magic;
	if (argc > 1)
	{
	 cp = argv[1]; tcp = *cp;
	 if (tcp == 'l')
           local_boot(argc, argv);
	 else if (tcp == 'c')
         {
           fake_boardid = 1;
           local_boot(argc, argv);
           fake_boardid = 0;
         }
	 else if (tcp == 'n')
           net_boot(argc, argv);
	 else if (tcp == 'o')
         {
           fake_boardid = 1;
           net_boot(argc, argv);
           fake_boardid = 0;
         }
	 else if (tcp == 'u')
           usb_boot(argc, argv);
         else
           return 0;
	}
        cmd = find_cmd("ubi");
        myargv[0] = "ubi";
        myargv[1] = "part";
        myargv[2] = "ubi";
        myargv[3] = NULL;
        cmd->cmd(cmd, 0, 3, myargv);
	aei_seutp();
        cmd = find_cmd("ubifsmount");
        myargv[0] = "ubifsmount";
        myargv[1] = getenv("ubi_root");
        myargv[2] = NULL;
        cmd->cmd(cmd, 0, 2, myargv);
        cmd = find_cmd("ubifsload");
        myargv[0] = "ubifsload";
        myargv[1] = getenv("loadaddr");
        myargv[2] = "/boot/uImage";
        myargv[3] = NULL;
        cmd->cmd(cmd, 0, 3, myargv);
        sprintf(boot_args, "%s rootfstype=ubifs ubi.mtd=ubi ro noswap "
         "%s root=ubi0:%s mv_net_config=1,(%s,0:1:2:3).mtu=1500 "
         "apps=%d bootmode=%s",
         getenv("console"), getenv("nandparts"), getenv("ubi_root"),
         getenv("ethaddr"), app_flag, getenv("bootmode"));
        setenv("bootargs", boot_args);
        cmd = find_cmd("bootm");
        myargv[0] = "bootm";
        myargv[1] = getenv("loadaddr");
	myargv[2] = NULL;
        cmd->cmd(cmd, 0, 2, myargv);
	enable_watchdog(-1);
	return(0);
}

U_BOOT_CMD(
	aei_boot,	3,	1,	aei_boot,
	"aei_boot  - boot or setup",
	"This Service Gateway Device command checks the boot environment"
	" to determine boot or setup."
);
