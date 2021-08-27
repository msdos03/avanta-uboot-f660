
#define SGD_MODEL_VER "sgd_revision=3A\0"
#define SGD_FW_VER "sgd_fw=1.0.7\0"
#define SGD_MAGIC "sgd_magic=0xc00010\0"

/*   start SG200 extra */

/*
 * Default environment variables
 */
#define CONFIG_BOOTCOMMAND             "nand read 2000000 100000 200000;bootm"

/* #define CONFIG_MTDPARTS                "mtdparts=nand:2m(uboot),-(ubi)" */
#define CONFIG_MTDPARTS "mtdparts=nand:1m(uboot),-(ubi)"
#define MTDPARTS "1m(uboot),-(ubi)"

#define CONFIG_EXTRA_ENV_SETTINGS      "mtdids=nand0=nand\0" \
       "ubi_root=active\0" \
       SGD_MAGIC \
       "mtdparts=mtdparts=nand:" MTDPARTS "\0" \
       "nandparts=mtdparts=nand_mtd:" MTDPARTS "\0" \
       "sgd_type=AEIServiceGateway\0" \
       "sgd_model=SG200\0" \
       "sgd_serial=not set\0" \
       SGD_MODEL_VER \
       SGD_FW_VER \
       "zhomeid=00,00,00,00\0"

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

/**** End of SG 200 extra */
