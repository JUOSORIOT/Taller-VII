#include "HX711.h"
const int LOADCELL_DOUT_PIN = 19;
const int LOADCELL_SCK_PIN = 18;
HX711 balanza;
void setup()
{ Serial.begin(115200);
  balanza.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  //delay(250);

  balanza.set_scale(423);
  balanza.tare(10);  // Hacer 10 lecturas, el promedio es la tara
}

void loop()
{ if (balanza.is_ready())
  { float reading = balanza.get_units(10);
    if (reading < 0)
    {
      reading = 0;
    }
    Serial.println(reading);
  }
  else
    Serial.println("HX711 not found.");
  delay(500);
}
