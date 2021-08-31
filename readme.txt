To build the u-boot,

   Prepare any 32bit Linux system

   sh build_hgg420n.sh

   Output files:

    u-boot-hgg420n_533_128m_ddr3_nand.bin:  image for NAND flash

     From u-boot command prompt, nand image can be updated with the command:

      bubt u-boot-hgg420n_533_128m_ddr3_nand.bin

    u-boot:  image for JTAG
      Note: The entry PC is modified at 6A0000 
            (see output u-boot.lds for reset_vector_sect)
   

