#include <config.h>
#include <common.h>
#include <command.h>

#define MVKW_SYSRST_DUR 0xf1018250
#define MVKW_RSTOUT     0xf1020108
#define MVKW_WDG        0xf1020324
#define MVKW_WDGEN      0xf1020300
#define RST_PWR		1	/* power on reset */
#define RST_DOG		2	/* watchdog timer, Linux reboot, uboot reset */
#define FSACTIVE	1	/* Root file system in use should be "active" */
#define FSFAILSAFE	2	/* Root file system in use should be "failsafe" */

void enable_watchdog(int flag);
void msleep(int);

char ethaddr[64];
char boot_args[256];


ulong sgd_magic = 0; 
extern int dramBoot;
int do_watchdog;
ulong rsttype;     
int do_firstboot;

#define BIG_ADDR "0x30000000"

void aei_checkpwron()
{
int i;

 if (*((ulong *) MVKW_SYSRST_DUR) < 0x00050000 )
 {
  rsttype = RST_PWR;
 }
 else
 {
  rsttype = RST_DOG;
 }
 *((ulong *) MVKW_SYSRST_DUR) = 0x80000000;
}

int aei_boardid(void)
{
int id;
 id = mvBoardIdGet();
 return id;
}

void my_bootm(char **myargv)
{
cmd_tbl_t *cmd;
char *tmpstr;
int delay_t;

    tmpstr = getenv("bootm_delay");
    if (tmpstr)
    {
     delay_t = simple_strtoul(tmpstr, NULL, 10);
     if (delay_t > 0) msleep(delay_t);
    }
    cmd = find_cmd("bootm");
    myargv[0] = "bootm";
    myargv[1] = getenv("loadaddr");
    myargv[2] = NULL;
    cmd->cmd(cmd, 0, 2, myargv);
}


void aei_setup ()
{
 ulong      val;
 /* ==1 for power on, ==2 for watchdog, ==3 for user held >10 seconds */
 ulong      fstarget;    /* target file system: active or failsafe */
 /* set !=0 if we got here due to a deliberate watchdog timeout */
 ulong      deliberate = 0; 
 ulong      dogcount = 0; /* the number of failed boot of a watchdog reset */
 char      *tmpstr;
 char      *fsstr;       /* environment string with "active" or "failsafe" */
 ulong      i;           /* generic counter */
 int do_saveenv;
 char *cp;
 int is_failsafe;

        is_failsafe = 0;
        do_saveenv = 0;
	tmpstr = getenv("ethaddr");
	fsstr = getenv("eth1addr");
	if (strcmp(tmpstr, fsstr))
        {
         /* make a copy so that saveenv works */
         //strcpy(ethaddr, tmpstr);
         //setenv("eth1addr", ethaddr);
         //do_saveenv = 1;
        }
	tmpstr = getenv("sgd_fw");
        cp = SGD_FW_VER; cp += 7;
        if ( (!tmpstr) || (strcmp(tmpstr, cp)))
        {
         //setenv("sgd_fw", cp);
         //do_saveenv = 1;
        }
        if (do_saveenv) saveenv();
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
         if (val & 0x80) is_failsafe = 1;
         if (val & 1) deliberate = 1; else deliberate = 0;
         val &= 0x7F;
         dogcount = val >> 1;
         if (dogcount) is_failsafe = 0; 
        }

        if (dogcount > 3) dogcount = 3;
	if (rsttype == RST_PWR) {
	 (void) setenv("bootmode", "pwron");
	 printf("Power-on to active\n");
	 enable_watchdog(1);
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
                * (unsigned long *) sgd_magic = 0xAA55AA00 | (dogcount << 1);
		(void) setenv("bootmode", "reboot");
		enable_watchdog(1);
		return;
	}

	/* At this point we know something is wrong */
	/* We don't want a failed system to cause a lot of NAND writes. */
	/* Only save dogcount if still in first three attempts */
	if (dogcount < 3) {
          dogcount++;
	}
        * (unsigned long *) sgd_magic = 0xAA55AA00 | (dogcount << 1);

	/* If this is our first or second unplanned watchdog (dogcount<2), then try */
	/* try to do a failsafe boot */
	if (dogcount <= 1) {
                if (is_failsafe)
                {
		 printf("Button Force reboot to failsafe\n");
		 setenv("bootmode", "reboot");
                }
                else
                {
		 printf("Watchdog reboot to failsafe\n");
		 setenv("bootmode", "error");
                }
		setenv("ubi_root", "failsafe");
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
  if (do_firstboot) do_watchdog = 0;
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
  if ((!cp) || (strcmp(cp, "1"))) do_watchdog = 1;
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

void local_boot(int argc, char **argv)
{
        cmd_tbl_t *cmd;
	char *myargv[8];
	char *cp;
	char *iname;

	if (argc > 2) 
         iname = argv[2]; 
        else
          iname = "/boot/uImage";
	setenv("ubi_root", "failsafe");
        cmd = find_cmd("ubi");
        myargv[0] = "ubi";
        myargv[1] = "part";
        myargv[2] = "ubi";
        myargv[3] = NULL;
        cmd->cmd(cmd, 0, 3, myargv);
        cmd = find_cmd("ubifsmount");
        myargv[0] = "ubifsmount";
        myargv[1] = getenv("ubi_root");
        myargv[2] = NULL;
        cmd->cmd(cmd, 0, 2, myargv);
	if (strcmp(iname, "/boot/uImage"))
        {
         sprintf(boot_args, "%s  %s %s rw "
          "mv_net_config=1,(%s,0:1:2:3),mtu=1500 zid=%s",
          getenv("console"), getenv("nandparts"), getenv("extra"),
          getenv("ethaddr"), getenv("zhomeid"));
        }
        else
        {
         * (unsigned long *) sgd_magic = 0xAA55AA00;
         sprintf(boot_args, "%s %s rootfstype=ubifs ubi.mtd=ubi ro noswap "
          "%s root=ubi0:%s mv_net_config=1,(%s,0:1:2:3),mtu=1500 "
          "bootmode=local",
          getenv("console"), getenv("nandparts"), getenv("extra"),
          getenv("ubi_root"), getenv("ethaddr"));
        }
        setenv("bootargs", boot_args);
        cmd = find_cmd("ubifsload");
        myargv[0] = "ubifsload";
        myargv[1] = getenv("loadaddr");
        myargv[2] = iname;
        myargv[3] = NULL;
        cmd->cmd(cmd, 0, 3, myargv);
        my_bootm(myargv);
}

int aei_boot ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	DECLARE_GLOBAL_DATA_PTR;
        cmd_tbl_t *cmd;
	char *myargv[8];
	int ret;
	char *cp, tcp;
	unsigned long val;
        
	cp = getenv("extra");
	if (!cp) setenv("extra", "");
        cmd = find_cmd("mtd");
        myargv[0] = "mtd";
        myargv[1] = NULL;
        cmd->cmd(cmd, 0, 1, myargv);
	enable_watchdog(0);
	cp = getenv("sgd_magic");
        if (cp) sgd_magic = simple_strtoul(cp, NULL, 16);
        if (sgd_magic == 0) 
        {
         cp = strchr(SGD_MAGIC, '=');
         sgd_magic = simple_strtoul(cp+1, NULL, 16);
        }
        aei_checkpwron();
	if (argc > 1)
	{
	 cp = argv[1]; tcp = *cp;
	 if (tcp == 'l')
           local_boot(argc, argv);
	 else if (tcp == 'v')
         {
          printf("FW Version=%s", SGD_FW_VER);
          printf("\n");
         }
         return 0;
	}
        cmd = find_cmd("ubi");
        myargv[0] = "ubi";
        myargv[1] = "part";
        myargv[2] = "ubi";
        myargv[3] = NULL;
        cmd->cmd(cmd, 0, 3, myargv);
        cp = getenv("first_boot");
        if (!cp) cp = "0";
        if (!strcmp(cp, "1"))
        {
         do_firstboot = 1;
         setenv("first_boot", "0");
         saveenv();
        }
	aei_setup();
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
         "%s root=ubi0:%s mv_net_config=1,(%s,0:1:2:3),mtu=1500 "
         "bootmode=%s",
         getenv("console"), getenv("nandparts"), getenv("ubi_root"),
         getenv("ethaddr"), getenv("bootmode"));
        setenv("bootargs", boot_args);
        my_bootm(argv);
        cmd = find_cmd("reset");
        myargv[0] = "reset";
        myargv[1] = NULL;
        cmd->cmd(cmd, 0, 1, myargv);
	return(0);
}

U_BOOT_CMD(
	aei_boot,	5,	1,	aei_boot,
	"aei_boot  - boot or setup",
	"This Service Gateway Device command checks the boot environment"
	" to determine boot or setup."
);
