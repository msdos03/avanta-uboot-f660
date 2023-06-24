#!/bin/bash

export PATH=~/marvell/cross/bin:$PATH

G_PARAMS=""

# do_compile <le / be> <board> <NAND/NOR/SPI> <NANDBOOT=1 ...>
do_compile()
{
	ENDIANESS=$1
	BOARD=$2
	PARAMS=$G_PARAMS

	ENDIAN=""
	COMPILER="arm-mv5sft-linux-gnueabi-"

	if [ $ENDIANESS == "be" ]; then
		ENDIAN="BE=1"
		COMPILER="armeb-mv5sft-linux-gnueabi-"
	fi

	case $BOARD in
		db88f6500bp )
			TEMP="DB-88F65XX-BP"
			;;
		rd88f6510sfu )
			TEMP="RD-88F6510-SFU"
			;;
		rd88f6560gw )
			TEMP="RD-88F6560-GW"
			;;
		f660 )
			TEMP="F660"
			;;
        	* )
			TEMP="ERROR"
	esac

	export CROSS_COMPILE=$COMPILER
	make mrproper
	make $BOARD"_config" $PARAMS $ENDIAN
	make -s >> log.txt
}

export ARCH=arm

############
# DB Board #
############

# Little-Endian
G_PARAMS="NANDBOOT=1 NAND=1"
#do_compile le db88f6500bp NAND

G_PARAMS="NANDBOOT=1 NAND=1 DDR3=1"
#do_compile le db88f6500bp NAND

#G_PARAMS="SPIBOOT=1 SPI=1"
#do_compile le db88f6500bp SPI

G_PARAMS="NORBOOT=1 NOR=1"
#do_compile le db88f6500bp NOR

G_PARAMS="SPIBOOT=1 SPI=1"
#do_compile le db88f6500bp SPI

# Big-Endian
G_PARAMS="NANDBOOT=1 NAND=1"
#do_compile be db88f6500bp NAND

G_PARAMS="SPIBOOT=1 SPI=1"
#do_compile be db88f6500bp SPI

################
# RD-SFU Board #
################
G_PARAMS="NANDBOOT=1 NAND=1"
#do_compile le rd88f6510sfu NAND

###############
# RD-GW Board #
###############
##do_compile le rd88f6560gw NAND
do_compile le f660

