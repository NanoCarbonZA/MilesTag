/*
Copyright <2018> <Ethan Johnston>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef MilesTag_h
#define MilesTag_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp32-hal.h"
#include "esp_intr.h"
#include "driver/gpio.h"
#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "freertos/semphr.h"
#include "soc/rmt_struct.h"

#ifdef __cplusplus
}
#endif

class MilesTagTX
{
  public:
    MilesTagTX();
    MilesTagTX(int _txPin, int _channel);
    bool SetTx(int _txPin, int _channel);
    void txConfig();
    void fireShot(unsigned long playerId, unsigned long dmg);
    void sendCommand(bool shortCommand, uint8_t command, uint16_t data);
    void sendIR(rmt_item32_t data[], int IRlength, bool waitTilDone);

  private:
    void irTransmit(unsigned long Buffer, int nbits);
    unsigned long QuantitytoBin(unsigned long dmg);
    unsigned long has_even_parity(unsigned long x);
    unsigned long add_parity(unsigned long x);

    int             txGpioNum;
    int             txRmtPort;
};

typedef struct MTShotRecieved {
  unsigned long Quantity;
  unsigned long PlayerID;
  bool Error = true;
} MTShotRecieved;

typedef struct {
  uint8_t Command;
  uint16_t Data;
  bool Error;
} MTCommandData;

class MilesTagRX
{
  public:
    MilesTagRX();
    MilesTagRX(int _rxPin, int _channel);
    bool SetRx(int _rxPin, int _channel);
    void rxConfig();
    int readIR();
    void decodeRAW(rmt_item32_t *rawDataIn, int numItems, unsigned int *irDataOut);
    void getDataIR(rmt_item32_t item, unsigned int* irDataOut, int index);
    MTShotRecieved DecodeShotData(unsigned long data);
    MTCommandData DecodeCommandData(unsigned long data);
    void processCommand(uint16_t command);
    void ClearHits();
    void ClearCommands();
    bool BufferPull();
    MTShotRecieved Hits[20];
    MTCommandData Commands[20];
    int HitCount = 0;
    int CommandCount;
  private:
    unsigned long BintoQuantity(unsigned long dmg);
    unsigned long has_even_parity(unsigned long x);
    int             rxGpioNum;
    int             rxRmtPort;    
};
#endif
