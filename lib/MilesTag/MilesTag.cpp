/*
Copyright <2018> <Ethan Johnston>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Arduino.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"



#ifdef __cplusplus
}
#endif

#include "MilesTag.h"
////////////////////////////////////
#define         DEBUG       true
////////////////////////////////////

// Clock divisor (base clock is 80MHz)
#define CLK_DIV                 80
#define TICK_10_US              (80000000 / CLK_DIV / 100000) // = 10
#define TIMEOUT_US              5   //50          //RMT receiver timeout value(us)

#define ROUND_TO                1   //50          //rounding value for microseconds timings
#define MARK_EXCESS             0   //100         //tweeked to get the right timing
#define SPACE_EXCESS            0   //50          //tweeked to get the right timing
#define TIMEOUT_US              5   //50          //RMT receiver timeout value(us)
#define MIN_CODE_LENGTH         5                 //Minimum data pulses received for a valid packet



#define IR_PIN  GPIO_NUM_13
#define REC_PIN  GPIO_NUM_12

#define DEBUG_SCALE 1
#define CDEBUG 1

#define HEADER_US 3200
#define SPACE_US 800
#define ONE_US 1600
#define ZERO_US 800
#define OFFSET 150

#define SIGNAL_ID_HIT 0x01
#define SIGNAL_ID_GRENADE 0x02
#define SIGNAL_ID_AMMO 0x03
#define SIGNAL_ID_MEDKIT 0x04

#define SIGNAL_ID_BASE 0x10
#define SIGNAL_ID_SPAWNPOINT 0x11
#define SIGNAL_ID_CAPTUREPOINT 0x12

enum IRSignalType
{
  HIT,
  GRENADE,
  AMMUNITION,
  MEDKIT,
  BASE,
  SPAWNPOINT,
  CAPTUREPOINT
};

void printBinary(uint16_t number, byte numberOfDigits)
{
    uint16_t _number = number;
    byte _numberOfDigits = numberOfDigits;
    //#ifdef DEBUG
        int _mask = 0;
        for (byte _n = 1; _n <= _numberOfDigits; _n++)
        {
            _mask = (_mask << 1) | 0x0001;
        }
        _number = _number & _mask;  // truncate v to specified number of places

        while(_numberOfDigits)
        {
            if (_number & (0x0001 << (_numberOfDigits - 1) ) )
            {
                Serial.print(F("1"));
            }
            else
            {
                Serial.print(F("0"));
            }

            --_numberOfDigits;

            if( ( (_numberOfDigits % 4) == 0) && (_numberOfDigits != 0) )
            {
                Serial.print(F("_"));
            }
        }
    //#endif
    Serial.println("");
}



MilesTagTX::MilesTagTX()
{
  if(DEBUG)
    Serial.print("ESP32_IR::Constructing");
}

MilesTagTX::MilesTagTX(int _txPin, int _channel)
{
  if(DEBUG)
    Serial.print("ESP32_IR::Constructing");
  SetTx(_txPin, _channel);
}

bool MilesTagTX::SetTx(int _txPin, int _channel)
{
    bool _status = true;

    if (_txPin >= GPIO_NUM_0 && _txPin < GPIO_NUM_MAX)
        txGpioNum = _txPin;
    else
        _status = false;

    if (_channel >= RMT_CHANNEL_0 && _channel < RMT_CHANNEL_MAX)
        txRmtPort = _channel;
    else
        _status = false;

    if (_status == false && DEBUG)
        Serial.println("ESP32_IR::Tx Pin init failed");
    return _status;
}

//transmit code
void MilesTagTX::txConfig()
{
  rmt_config_t config;
  config.channel = (rmt_channel_t)txRmtPort;
  config.gpio_num = (gpio_num_t)txGpioNum;
  config.mem_block_num = 1;//how many memory blocks 64 x N (0-7)
  config.clk_div = CLK_DIV;
  config.tx_config.loop_en = false;
  config.tx_config.carrier_duty_percent = 50;
  config.tx_config.carrier_freq_hz = 38000;
  config.tx_config.carrier_level = (rmt_carrier_level_t)1;
  config.tx_config.carrier_en = 1;
  config.tx_config.idle_level = (rmt_idle_level_t)0;
  config.tx_config.idle_output_en = true;
  config.rmt_mode = (rmt_mode_t)0;//RMT_MODE_TX;
  rmt_config(&config);
  rmt_driver_install(config.channel, 0, 0);//19     /*!< RMT interrupt number, select from soc.h */
}

void MilesTagTX::fireShot(unsigned long playerId, unsigned long dmg) {
   sendCommand(true, QuantitytoBin(dmg), playerId);
}

void MilesTagTX::sendCommand(bool shortCommand, uint8_t command, uint16_t data) {
  unsigned long encodedData = 0;
  Serial.println("sendCommand");
  if (shortCommand) {
    Serial.println("short command");
    // Use 12 bits for short commands
    encodedData |= (1 << 11); // Set the 12th bit for short command (1)
    encodedData |= (command & 0xF) << 6; // Encode command (bits 11-8)
    encodedData |= (data & 0x3F) << 1; // Encode player ID (bits 7-2)
  } else {
    Serial.println("long command");
    // Use 24 bits for long commands
    encodedData |= (0 << 23); // Set the 24th bit for shot command (0)
    encodedData |= (command & 0xF) << 18; // Encode command (bits 23-20)
    encodedData |= (data & 0x3FFFF) << 1; // Encode data (bits 19-2)
  }

  // Add parity bit (bit 0)
  encodedData = add_parity(encodedData);
  if (DEBUG) {
    printBinary( encodedData, shortCommand ? 12 : 24);
  }
  
  // Send the data
  irTransmit(encodedData, shortCommand ? 12 : 24);
}

void MilesTagTX::irTransmit(unsigned long data, int nbits) {
  Serial.println("irTransmit" + String(data) + " " + String(nbits));
  // Header
  rmt_item32_t irDataArray[nbits];

  irDataArray[0].duration0 = HEADER_US * DEBUG_SCALE;
  irDataArray[0].level0 = 1;
  irDataArray[0].duration1 = SPACE_US * DEBUG_SCALE;
  irDataArray[0].level1 = 0;

  // Data
  for (int i = 1; i <= nbits; i++) {
    unsigned long mask = 1ULL << (nbits - i);
    if (data & mask) {
      irDataArray[i].duration0 = ONE_US * DEBUG_SCALE;
      irDataArray[i].level0 = 1;
      irDataArray[i].duration1 = SPACE_US * DEBUG_SCALE;
      irDataArray[i].level1 = 0;
    } else {
      irDataArray[i].duration0 = ZERO_US * DEBUG_SCALE;
      irDataArray[i].level0 = 1;
      irDataArray[i].duration1 = SPACE_US * DEBUG_SCALE;
      irDataArray[i].level1 = 0;
    }
  }
  sendIR(irDataArray, nbits + 1, true);
}

/**************************************************************************************************************************************************/

void MilesTagTX::sendIR(rmt_item32_t data[], int IRlength, bool waitTilDone)
{
    if(DEBUG)   Serial.println("ESP32_IR::sendIR()");
    rmt_config_t config;
    config.channel = (rmt_channel_t)txRmtPort;
    rmt_write_items(config.channel, data, IRlength, waitTilDone);  //false means non-blocking
    //Wait until sending is done.
    if(waitTilDone)
    {
        rmt_wait_tx_done(config.channel,1);
        //before we free the data, make sure sending is already done.
        //free(data);
    }
}

/**************************************************************************************************************************************************/


unsigned long MilesTagTX::QuantitytoBin(unsigned long dmg) {
  if (dmg >= 100) {
    dmg = 15;
  } else if (dmg >= 75) {
    dmg = 14;
  } else if (dmg >= 50) {
    dmg = 13;
  } else if (dmg >= 40) {
    dmg = 12;
  } else if (dmg >= 35) {
    dmg = 11;
  } else if (dmg >= 30) {
    dmg = 10;
  } else if (dmg >= 25) {
    dmg = 9;
  } else if (dmg >= 20) {
    dmg = 8;
  } else if (dmg >= 17) {
    dmg = 7;
  } else if (dmg >= 15) {
    dmg = 6;
  } else if (dmg >= 10) {
    dmg = 5;
  } else if (dmg >= 7) {
    dmg = 4;
  } else if (dmg >= 5) {
    dmg = 3;
  } else if (dmg >= 4) {
    dmg = 2;
  } else if (dmg >= 2) {
    dmg = 1;
  } else if (dmg >= 1) {
    dmg = 0;
  }
  return dmg;
}

unsigned long MilesTagTX::has_even_parity(unsigned long x){
    unsigned long count = 0, i, b = 1;
    for(i = 0; i < 32; i++){
        if( x & (b << i) ){count++;}
    }
    if( (count % 2) ){return 0;}
    return 1;
}

unsigned long MilesTagTX::add_parity(unsigned long x){
  unsigned long parity = 0;
  if (has_even_parity(x)) {
    parity = 1;
  }
  x = x << 1;
  x = x | parity;
  return x;
}

//Recieve code
MilesTagRX::MilesTagRX()
{
  if(DEBUG)
    Serial.print("ESP32_IR::Constructing");
}

MilesTagRX::MilesTagRX(int _txPin, int _channel)
{
  if(DEBUG)
    Serial.print("ESP32_IR::Constructing");
  SetRx(_txPin, _channel);
}

bool MilesTagRX::SetRx(int _rxPin, int _channel)
{
    bool _status = true;

    if (_rxPin >= GPIO_NUM_0 && _rxPin < GPIO_NUM_MAX)
        rxGpioNum = _rxPin;
    else
        _status = false;

    if (_channel >= RMT_CHANNEL_0 && _channel < RMT_CHANNEL_MAX)
        rxRmtPort = _channel;
    else
        _status = false;

    if (_status == false && DEBUG)
        Serial.println("ESP32_IR::Tx Pin init failed");
    return _status;
}


void MilesTagRX::rxConfig(){
  rmt_config_t config;
  config.rmt_mode = RMT_MODE_RX;
  config.channel = (rmt_channel_t)rxRmtPort;
  config.gpio_num = (gpio_num_t)rxGpioNum;
  gpio_pullup_en((gpio_num_t)rxGpioNum);
  config.mem_block_num = 1; //how many memory blocks 64 x N (0-7)
  config.rx_config.filter_en = 1;
  config.rx_config.filter_ticks_thresh = 100; // 80000000/100 -> 800000 / 100 = 8000  = 125us
  config.rx_config.idle_threshold = HEADER_US + OFFSET;
  config.clk_div = CLK_DIV;
  ESP_ERROR_CHECK(rmt_config(&config));
  ESP_ERROR_CHECK(rmt_driver_install(config.channel, 1000, 0));
  // rmt_get_ringbuf_handle(config.channel, &ringBuf);
  rmt_rx_start(config.channel, 1);
}

void MilesTagRX::ClearHits() {
  for (int i = 0; i < 20; i++) {
    Hits[i].PlayerID = 0;
    Hits[i].Quantity = 0;
    Hits[i].Error = true;
  }
  HitCount = 0;
}
void MilesTagRX::ClearCommands() {
  for (int i = 0; i < 20; i++) {
    Commands[i].Command = 0;
    Commands[i].Data = 0;
    Commands[i].Error = true;
  }
  CommandCount = 0;
}

bool MilesTagRX::BufferPull()
{
  unsigned long data = 0;
  RingbufHandle_t ringBuf = NULL;
  rmt_config_t config;
  // Serial.println("RX Pin: " + String(rxGpioNum) + " Channel: " + String(rxRmtPort));
  config.channel = (rmt_channel_t)rxRmtPort;
  rmt_get_ringbuf_handle(config.channel, &ringBuf);

  if (ringBuf == NULL)
  {
    Serial.println("Failed to get Ring Buffer Handle.");
    return false;
  }

  // while(ringBuf) {
  size_t rx_size = 0;
  rmt_item32_t *item = (rmt_item32_t *)xRingbufferReceive(ringBuf, &rx_size, (TickType_t)TIMEOUT_US);
  int numItems = rx_size / sizeof(rmt_item32_t);
  if (numItems == 0)
  {
    return false; // If no items, continue to next iteration
  }
  // memset(item, 0, sizeof(unsigned int) * 64);
  rmt_item32_t *itemproc = item;

  bool shortCommand = false;
  int bitNumber = 0;
  int commandsize = 23;

  for (size_t i = 0; i < (rx_size / 4); i++)
  {
    // unsigned int markValue = (itemproc->duration0) / 0.0125;
    unsigned int markValue = (itemproc->duration0);
    markValue = ROUND_TO * round((float)markValue / ROUND_TO);

    // unsigned int spaceValue = (itemproc->duration1) / 0.0125;
    unsigned int spaceValue = (itemproc->duration1);
    spaceValue = ROUND_TO * round((float)spaceValue / ROUND_TO);

    // Serial.println("Item " + String(i) + " - " + String(itemproc->duration0) + "("+String(markValue)+") - " + String(itemproc->duration1));

    if (markValue > (HEADER_US - OFFSET) &&  markValue < (HEADER_US + OFFSET))
    {
      // Serial.println("Header Received");
      bitNumber = 0;
    }
    else
    {
      // Check if the first bit after the header is 1 (shortCommand)
      if (bitNumber == 1)
      {
        if (markValue > (ZERO_US - OFFSET) && markValue < (ZERO_US + OFFSET))
        {
          // Serial.println("**************************************************");
          // Serial.println("Short Command");
          shortCommand = true;
          commandsize = 12;
        }
      }
      else
      {
        // Loop through the bits
        if (markValue > (ONE_US - OFFSET) && markValue < (ONE_US + OFFSET))
        {
          data = data | 1 << (commandsize - bitNumber);
        }
        else if (markValue > (ZERO_US - OFFSET) && markValue < (ZERO_US + OFFSET))
        {
          data = data | 0 << (commandsize - bitNumber);
        }
      }
    }
    bitNumber++;
    if (shortCommand && bitNumber%13 == 0)
    {
      Serial.println("Shot - " + String(data,BIN));
      Hits[HitCount] = DecodeShotData(data);
      // Serial.println("Hitcount before: " + String(HitCount));
      HitCount++;
      // Serial.println("Hit Count - " + String(HitCount));
      data = 0;
    }
    else if (bitNumber%23 == 0)
    {
      Serial.println("Command - " + String(data,BIN));
      MTCommandData command = DecodeCommandData(data);
      if (!command.Error)
      {
        Commands[CommandCount] = command;
        CommandCount++;
        Serial.println("Command Count - " + String(CommandCount));
      }
      data = 0;
    }

    ++itemproc;
  }
  // Serial.println("BufferPull - " + String(data,HEX));
  
  // printBinary(data, 12);
  vRingbufferReturnItem(ringBuf, (void *)item);


  Serial.println("**************************************************");
  return true;
  
  // }
}



MTShotRecieved MilesTagRX::DecodeShotData(unsigned long data) {
  MTShotRecieved decodedData;

  unsigned long dataWp = (data & 0xFFFFFFFE) >> 1;

  unsigned long count = 0, i, b = 1;
  for(i = 0; i < 32; i++){
    if( dataWp & (b << i) ){count++;}
  }

  decodedData.PlayerID = (dataWp & 0x3F) >> 1;
  decodedData.Quantity = BintoQuantity((dataWp & 0x3C0) >> 6);

  decodedData.Error = false;
  if (has_even_parity(data)) {
    decodedData.Error = true;
  }
  if (count > 12) {
    decodedData.Error = true;
  }
  if (decodedData.Quantity > 100) {
    decodedData.Error = true;
  }
  if (decodedData.PlayerID > 63) {
    decodedData.Error = true;
  }

  return decodedData;
}

MTCommandData MilesTagRX::DecodeCommandData(unsigned long data) {
  MTCommandData decodedData;

  unsigned long dataWp = (data & 0xFFFFFFFE) >> 1;

  unsigned long count = 0, i, b = 1;
  for (i = 0; i < 32; i++) {
    if (dataWp & (b << i)) {
      count++;
    }
  }

  decodedData.Command = (dataWp & 0x3C0000) >> 18;
  decodedData.Data = (dataWp & 0x3FFFF) >> 1;

  decodedData.Error = false;
  if (has_even_parity(data)) {
    decodedData.Error = true;
  }
  if (count > 24) {
    decodedData.Error = true;
  }
  if (decodedData.Command > 0xf) {
    decodedData.Error = true;
  }
  if (decodedData.Data > 0xffff) {
    decodedData.Error = true;
  }

  return decodedData;
}

void MilesTagRX::processCommand(uint16_t command) {
  // Process the command according to your game logic
  // For example, you can use a switch statement to handle different commands
  switch (command) {
    case 0x01: // Example command
      // Handle command 0x01
      break;
    // ... add more cases for other commands
    default:
      // Handle unrecognized command
      break;
  }
}


unsigned long MilesTagRX::BintoQuantity(unsigned long dmg) {
  Serial.println("Dmg - " + String(dmg));
  unsigned long dmgarray[16] = {1, 2, 4, 5, 7, 10, 15, 17, 20, 25, 30, 35, 40, 50, 75, 100};
  return dmgarray[dmg];
}

unsigned long MilesTagRX::has_even_parity(unsigned long x){
    unsigned long count = 0, i, b = 1;
    for(i = 0; i < 32; i++){
        if( x & (b << i) ){count++;}
    }
    if( (count % 2) ){return 0;}
    return 1;
}
