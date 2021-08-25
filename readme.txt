
1. To add functions to u-boot, check the source file

   common/cmd_actiontec.c

2. To build the u-boot,

   sh build_sg200.sh

   Output files:

    u-boot-sg200_533_512m_ddr3_nand.bin:  image for NAND flash

     From u-boot command prompt, nand image can be updated with the command:

      bubt u-boot-sg200_533_512m_ddr3_nand.bin

    u-boot:  image for JTAG
      Note: The entry PC is modified at 6A0000 
            (see output u-boot.lds for reset_vector_sect)
   

