#!/bin/bash

export PATH=/opt/marvell/cross/bin:$PATH

G_PARAMS=""

# do_compile <le / be> <board> <NAND/NOR/SPI> <NANDBOOT=1 ...>
do_compile()
{
	ENDIANESS=$1
	BOARD=$2
	TYPE=$3
	PARAMS=$G_PARAMS

	TAR_DIR="../images/LE"
	ENDIAN=""
	COMPILER="arm-mv5sft-linux-gnueabi-"

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
        	* )
			TEMP="ERROR"
	esac

	TAR_DIR=$TAR_DIR/$TEMP/$TYPE

	export CROSS_COMPILE=$COMPILER
        make env
}

export ARCH=arm

do_compile le rd88f6510sfu NAND

