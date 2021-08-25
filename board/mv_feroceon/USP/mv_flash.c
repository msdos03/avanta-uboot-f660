#include <common.h>
#include <flash.h>

#ifdef CONFIG_SPI_FLASH
#include <spi.h>
#include <spi_flash.h>

struct spi_flash *flash;

unsigned long spi_flash_init(void)
{
	flash = spi_flash_probe(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS, CONFIG_ENV_SPI_MAX_HZ, SPI_MODE_3);
	if (!flash) {
		printf("Failed to initialize SPI flash at %u:%u\n", CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS);
		return 1;
	}
	return 0;
}

#endif

#ifndef CONFIG_SYS_NO_FLASH

//flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];

//static struct spi_flash *env_flash;
/*
unsigned long flash_init(void)
{
	int i;

	for (i = 0; i < CONFIG_SYS_MAX_FLASH_BANKS; i++) {
		flash_info[i].flash_id = FLASH_UNKNOWN;
		flash_info[i].sector_count = 0;
		flash_info[i].size = 0;
	}

	return 1;
}

int flash_erase(flash_info_t *info, int s_first, int s_last)
{
	return 1;
}

void flash_print_info(flash_info_t *info)
{
	printf("No flashrom installed\n");
}

int write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt)
{
	return 0;
}
*/
#endif