
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

	if [ $ENDIANESS == "be" ]; then
		ENDIAN="BE=1"
		TAR_DIR="../images/BE"
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
        	* )
			TEMP="ERROR"
	esac

	TAR_DIR=$TAR_DIR/$TEMP/$TYPE

	export CROSS_COMPILE=$COMPILER
	make mrproper
	make $BOARD"_config" $PARAMS $ENDIAN
	make -j3 -s &>> log.txt
	mkdir -p $TAR_DIR
	cp u-boot-$BOARD* $TAR_DIR/
	rm $TAR_DIR/*.srec
}

export ARCH=arm

rm -Rf ../images
mkdir ../images
mkdir ../images/LE
mkdir ../images/BE

rm -f log.txt
touch log.txt

############
# DB Board #
############

# Little-Endian
G_PARAMS="NANDBOOT=1 NAND=1 DDR3=1"
do_compile le db88f6500bp NAND

G_PARAMS="NORBOOT=1 NOR=1 DDR3=1"
do_compile le db88f6500bp NOR

G_PARAMS="SPIBOOT=1 SPI=1 DDR3=1"
do_compile le db88f6500bp SPI

# Big-Endian
G_PARAMS="NANDBOOT=1 NAND=1 DDR3=1"
do_compile be db88f6500bp NAND

G_PARAMS="SPIBOOT=1 SPI=1 DDR3=1"
do_compile be db88f6500bp SPI

################
# RD-SFU Board #
################
G_PARAMS="NANDBOOT=1 NAND=1"
do_compile le rd88f6510sfu NAND


###############
# RD-GW Board #
###############
G_PARAMS="NANDBOOT=1 NAND=1 DDR3=1"
do_compile le rd88f6560gw NAND

