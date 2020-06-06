
#include "../../inc/MarlinConfig.h"
#include "../../MarlinCore.h"
#include "msc_sd.h"

#if SD_CONNECTION_IS(ONBOARD)
  #include "../../sd/cardreader.h"
  #include "usbd_msc.h"
  #include "USB.h"
  #include "USBMscHandler.h"

  // Sd2Card uses 512 bytes
  #define BLOCK_SIZE 512U

  class Sd2CardUSBMscHandler : public USBMscHandler {
    public:
      void initialize() {
        pCard = &card.getSd2Card();
      }

      bool GetCapacity(uint32_t *pBlockNum, uint16_t *pBlockSize) {
        *pBlockNum = pCard->cardSize();
        *pBlockSize = BLOCK_SIZE;
        return true;
      }

      bool Write(uint8_t *pBuf, uint32_t blkAddr, uint16_t blkLen) {
        for (uint32_t i = 0; i < blkLen; i++) {
          if (!pCard->writeBlock(blkAddr + i, pBuf))
            return false;
          pBuf += BLOCK_SIZE;
        }

        return true;
      }

      bool Read(uint8_t *pBuf, uint32_t blkAddr, uint16_t blkLen) {
        for (uint32_t i = 0; i < blkLen; i++) {
          if (!pCard->readBlock(blkAddr + i, pBuf))
            return false;
          pBuf += BLOCK_SIZE;
        }

        return true;
      }

      bool IsReady() {
        return card.isMounted();
      }

    private:
      Sd2Card * pCard = nullptr;
  };

  Sd2CardUSBMscHandler usbMscHandler;
#endif

void MSC_SD_Init() {
  #if SD_CONNECTION_IS(ONBOARD)
    usbMscHandler.initialize();
    USBDevice.registerMscHandler(usbMscHandler);
  #endif
}