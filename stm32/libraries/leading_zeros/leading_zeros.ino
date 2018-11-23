#include <Streaming.h>
#include "StreamingEx.h"

void setup() {
  Serial.begin(115200);
}

int counter;
int max_counter = 1000;
int8_t hexdec;

void loop() {
  if ( hexdec ) {
    Serial << "counter=0x" << _HEXZ(counter, 4) << endl;
  }
  else {
    Serial << "counter=" << _DECZ(counter, 4) << endl;
  }

  if ( counter + 1 <= max_counter ) {
    counter = counter + 1;
  }
  else {
    counter = 0;
    hexdec ^= 1;
    max_counter = (hexdec) ? 0x1000 : 1000;
    delay(5000);
  }
}

