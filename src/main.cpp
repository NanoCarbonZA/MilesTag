#include <Arduino.h>
#include <MilesTag.h>

MilesTagRX tagger = MilesTagRX(13, 1);


void setup()
{
  // setCpuFrequencyMhz(240); // Set CPU frequency to 160 MHz
  pinMode(27, INPUT_PULLUP);
  delay(2000);
  pinMode(27, INPUT_PULLDOWN);
  Serial.begin(115200);
  tagger.SetRx(13,1);                           //  Set the IR LED pin to 13 (default is 4)
  tagger.rxConfig();    
}

void loop()
{
  // Serial.println("loop -" + String(tagger.readIR()));

  Serial.println("");
  if (tagger.bufferPull()){
    for (int numberOfHits = 0; numberOfHits < tagger.hitCount; numberOfHits++){
      Serial.print(" Error: ");
      Serial.print(tagger.Hits[numberOfHits].error);
      Serial.print(" Player: ");
      Serial.print(tagger.Hits[numberOfHits].playerId);
      Serial.print(" quantity: ");
      Serial.print(tagger.Hits[numberOfHits].quantity);
      Serial.print(" No Of Bits: ");
      Serial.println(tagger.Hits[numberOfHits].noOfBits);
    }
    tagger.clearHits();
    for (int numberOfCommands = 0; numberOfCommands < tagger.commandCount; numberOfCommands++){
      Serial.print(" Error: ");
      Serial.print(tagger.Commands[numberOfCommands].error);
      Serial.print(" Command: ");
      Serial.print(tagger.Commands[numberOfCommands].command);
      Serial.print(" Data: ");
      Serial.print(tagger.Commands[numberOfCommands].data);
      Serial.print(" No Of Bits: ");
      Serial.println(tagger.Commands[numberOfCommands].noOfBits);

    }
    tagger.clearCommands();    
  } else {
    Serial.println("No Hits");
  }
    
  delay(15000);
}