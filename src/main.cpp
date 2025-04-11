#include <M5Atom.h>
#include <Arduino.h>

void setup()
{
  M5.begin(true, false, true);
  delay(50);
  M5.dis.drawpix(0, 0x00ff00);
  Serial.begin(115200);
}

uint8_t FSM = 0; // Store the number of key presses.

void loop()
{
  if (M5.Btn.wasPressed())
  {
    Serial.println("Click!");

    switch (FSM)
    {
    case 0:
      M5.dis.drawpix(0, 0xfff000); // YELLOW
      break;
    case 1:
      M5.dis.drawpix(0, 0xff0000); // RED
      break;
    case 2:
      M5.dis.drawpix(0, 0x0000f0); // BLUE
      break;
    case 3:
      M5.dis.drawpix(0, 0x00ff00); // GREEN
      break;
    default:
      break;
    }

    FSM = ++FSM % 4;
  }
  delay(50);
  M5.update();
}