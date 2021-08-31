/*
 * Default environment variables
 */
#define CONFIG_BOOTCOMMAND             "nand read 2000000 100000 200000;bootm"

/* #define CONFIG_MTDPARTS                "mtdparts=nand:2m(uboot),-(ubi)" */
#define CONFIG_MTDPARTS "mtdparts=nand:1m(uboot),-(firmware)"
#define MTDPARTS "1m(uboot),-(firmware)"

#define CONFIG_EXTRA_ENV_SETTINGS      "mtdids=nand0=nand\0" \
       "mtdparts=mtdparts=nand:" MTDPARTS "\0" \
       "nandparts=mtdparts=nand_mtd:" MTDPARTS "\0"
/* UBI file system */

#define CONFIG_CMD_FAT
#define CONFIG_CMD_EXT2
/* #define CONFIG_CMD_JFFS2 */
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_RBTREE
#define CONFIG_MTD_DEVICE               /* needed for mtdparts commands */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_LZO

