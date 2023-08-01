/* The example of ESP-IDF
 *
 * This sample code is in the public domain.
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "simple_file_system.h"

#include "w25q64.h"

#define TAG "W25Q64"

void dump(uint8_t *dt, int n)
{
	uint16_t clm = 0;
	uint8_t data;
	uint8_t sum;
	uint8_t vsum[16];
	uint8_t total =0;
	uint32_t saddr =0;
	uint32_t eaddr =n-1;

	printf("----------------------------------------------------------\n");
	uint16_t i;
	for (i=0;i<16;i++) vsum[i]=0;  
	uint32_t addr;
	for (addr = saddr; addr <= eaddr; addr++) {
		data = dt[addr];
		if (clm == 0) {
			sum =0;
			printf("%05"PRIx32": ",addr);
		}

		sum+=data;
		vsum[addr % 16]+=data;

		printf("%02x ",data);
		clm++;
		if (clm == 16) {
			printf("|%02x \n",sum);
			clm = 0;
		}
	}
	printf("----------------------------------------------------------\n");
	printf("       ");
	for (i=0; i<16;i++) {
		total+=vsum[i];
		printf("%02x ",vsum[i]);
	}
	printf("|%02x \n\n",total);
}

W25Q64_t flash;

int flash_erase(uint32_t address, uint32_t size) {
	return 1;
}

int flash_read(uint32_t address, uint8_t *buffer, uint32_t size) {
	return W25Q64_fastread(&flash, address, buffer, size);
}

int flash_write(uint32_t address, uint8_t *buffer, uint32_t size) {
	// w25q64_WR
	return 1;
}

void app_main()
{
	W25Q64_init(&flash);

	sfs_t sfs;
	sfs_config_t sfs_cfg = {
		.read_fnc = flash_read,
		.write_fnc = flash_write,
		.erase_fnc = flash_erase,
		.flash_size_mb = 8,
		.flash_sector_kb = 4
	};

	W25Q64_eraseSector(&flash, 0, true);
	uint8_t buff[13] = {0xFF, 0xFF, 0x53, 0x46, 0x53, 0x54, 0x45, 0x53, 0x54, 0xFF, 0xFF, 0xFF, 0};
	W25Q64_pageWrite(&flash, 4, 0, buff, sizeof(buff));

	// uint8_t buf = '\0';
	// W25Q64_pageWrite(&flash, 0, 12, &buf, 1);

	// uint8_t buff2[2] = {0x01, 0xD1};
	// W25Q64_pageWrite(&flash, 0, 13, buff2, sizeof(buff2));
	uint8_t rbuf[256];
	W25Q64_fastread(&flash, 0, rbuf, 256);
	ESP_LOGI(TAG, "===SFS READ====");
	for (int i = 0; i < 13; ++i) {
		ESP_LOGI(TAG, "%d\t0x%2X", i, rbuf[i]);
	}
	
	sfs_init(&sfs, &sfs_cfg);
	char name[] = "TEST";
	sfs_file_t file;
	sfs_open(&sfs, &file, name);
	// W25Q64_t dev;
	// W25Q64_init(&dev);

	// // // Read 256 byte data from Address=0
	// uint8_t rbuf[256];
	// int len;
	// memset(rbuf, 0, 256);
	// len =  W25Q64_fastread(&dev, 0, rbuf, 256);
	// if (len != 256) {
	// 	ESP_LOGE(TAG, "fastread fail");
	// 	while(1) { vTaskDelay(1); }
	// }
	// ESP_LOGI(TAG, "Fast Read Data: len=%d", len);
	// dump(rbuf, 256);

	// // Erase data by Sector
	// bool flag = W25Q64_eraseSector(&dev, 0, true);
	// if (flag == false) {
	// 	ESP_LOGE(TAG, "eraseSector fail %d", 12);
	// 	while(1) { vTaskDelay(1); }
	// }
	// // memset(rbuf, 0, 256);
	// // len =  W25Q64_read(&dev, 0, rbuf, 256);
	// // if (len != 256) {
	// // 	ESP_LOGE(TAG, "read fail");
	// // 	while(1) { vTaskDelay(1); }
	// // }
	// // ESP_LOGI(TAG, "Read Data: len=%d", len);
	// // dump(rbuf, 256);

	// // Write data to Sector=0 Address=10
	// uint8_t wdata[26];    // 書込みデータ
	// for (int i=0; i<26; i++) {
	// 	wdata[i]='A'+i; // A-Z     
	// }  
	// len =  W25Q64_pageWrite(&dev, 0, 10, wdata, 26);
	// if (len != 26) {
	// 	ESP_LOGE(TAG, "pageWrite fail");
	// 	while(1) { vTaskDelay(1); }
	// }
	// ESP_LOGI(TAG, "Page Write(Sector=0 Address=10) len=%d", len);


	// for (int i=0; i<26; i++) {
	// 	wdata[i] = i; // A-Z     
	// }  
	// len =  W25Q64_pageWrite(&dev, 0, 50, wdata, 26);
	// if (len != 26) {
	// 	ESP_LOGE(TAG, "pageWrite fail");
	// 	while(1) { vTaskDelay(1); }
	// }
	// ESP_LOGI(TAG, "Page Write(Sector=0 Address=10) len=%d", len);

	// // First read 256 byte data from Address=0
	// memset(rbuf, 0, 256);
	// len =  W25Q64_fastread(&dev, 0, rbuf, 256);
	// if (len != 256) {
	// 	ESP_LOGE(TAG, "fastread fail");
	// 	while(1) { vTaskDelay(1); }
	// }
	// ESP_LOGI(TAG, "Fast Read Data: len=%d", len);
	// dump(rbuf, 256);

	// // Write data to Sector=0 Address=0
	// for (int i=0; i < 10;i++) {
	// 	wdata[i]='0'+i; // 0-9     
	// }  
	// len =  W25Q64_pageWrite(&dev, 0, 0, wdata, 10);
	// if (len != 10) {
	// 	ESP_LOGE(TAG, "pageWrite fail");
	// 	while(1) { vTaskDelay(1); }
	// }
	// ESP_LOGI(TAG, "Page Write(Sector=0 Address=0) len=%d", len);

	// // First read 256 byte data from Address=0
	// memset(rbuf, 0, 256);
	// len =  W25Q64_fastread(&dev, 0, rbuf, 256);
	// if (len != 256) {
	// 	ESP_LOGE(TAG, "fastread fail");
	// 	while(1) { vTaskDelay(1); }
	// }
	// ESP_LOGI(TAG, "Fast Read Data: len=%d", len);
	// dump(rbuf, 256);

	// for (int i=0; i < 10;i++) {
	// 	wdata[i]='l'; // 0-9     
	// }  
	// len =  W25Q64_pageWrite(&dev, 0, 0, wdata, 10);
	// if (len != 10) {
	// 	ESP_LOGE(TAG, "pageWrite fail");
	// 	while(1) { vTaskDelay(1); }
	// }
	// ESP_LOGI(TAG, "Page Rewrite(Sector=0 Address=0) len=%d", len);

	// // First read 256 byte data from Address=0
	// memset(rbuf, 0, 256);
	// len =  W25Q64_fastread(&dev, 0, rbuf, 256);
	// if (len != 256) {
	// 	ESP_LOGE(TAG, "fastread fail");
	// 	while(1) { vTaskDelay(1); }
	// }
	// ESP_LOGI(TAG, "Fast Read Data: len=%d", len);
	// dump(rbuf, 256);

	// ESP_LOGI(TAG, "Success All Test");
}

