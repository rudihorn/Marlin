
#include "../../inc/MarlinConfig.h"
#include "msc_sd.h"

#if SD_CONNECTION_IS(ONBOARD)
  #include "../../sd/cardreader.h"
  #include "usbd_msc.h"
  #include "usbd_msc_storage_if.h"
  #include "USB.h"

  // Sd2Card uses 512 bytes
  #define BLOCK_SIZE 512U

  Sd2Card * pCard = nullptr;

  static int8_t MSC_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size) {
      *block_num = pCard->cardSize();
      *block_size = BLOCK_SIZE;
      return USBD_OK;
  }

  static int8_t MSC_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len) {
    if (!pCard->writeStart(blk_addr, blk_len))
      return false;

    for (int i= 0; i < blk_len; i++) {
      if (!pCard->writeData(buf))
        return false;
      buf += BLOCK_SIZE;
    }

    pCard->writeStop();
    return USBD_OK;
  }

  static int8_t MSC_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len) {
    if (!pCard->readStart(blk_addr))
      return false;

    for (uint16_t i = 0; i < blk_len; i++) {
      if (!pCard->readData(buf))
        return false;
      buf += BLOCK_SIZE;
    }

    pCard->readStop();
    return USBD_OK;
  }

  static int8_t MSC_GetMaxLun(void) {
    return 0; // only one storage logical unit
  }

  static int8_t MSC_IsReady(uint8_t lun) {
    if (pCard->cardSize() > 0) {
      // tell the host the sd card has been initialised
      return 0;
    } else {
      // signal to the host that the SD card is still initialising
      return 1;
    }
  }

  static int8_t MSC_IsWriteProtected(uint8_t lun) {
    return 0;
  }

  static int8_t MSC_Init(uint8_t lun) {
    return 0;
  }

  USBD_StorageTypeDef USBD_MSC_SD_fops =
  {
    MSC_Init,
    MSC_GetCapacity,
    MSC_IsReady,
    MSC_IsWriteProtected,
    MSC_Read,
    MSC_Write,
    MSC_GetMaxLun,
    USBD_MSC_fops.pInquiry,
  };
#endif

void MSC_SD_Init() {
  #if SD_CONNECTION_IS(ONBOARD)
    pCard = &card.getSd2Card();
    USBDevice.register_msc(&USBD_MSC_SD_fops);
  #endif
}