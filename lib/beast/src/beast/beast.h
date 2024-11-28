#ifndef __BEAST_H__
#define __BEAST_H__

#define MCP23X17 1;

#include <Arduino.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <esp_partition.h>
#include <esp_ota_ops.h>
#ifndef MCP23X17
#include <Adafruit_MCP23X08.h>
#else
#include <Adafruit_MCP23X17.h>
#endif

#include "beast_assets.h"
#include "beast_worker.h"

HardwareSerial SerialXiao1(0);
#define Port SerialXiao1


#ifndef MCP23X17
Adafruit_MCP23X08 mcp;
#else
Adafruit_MCP23X17 mcp;
#endif


#define MCP23XXX_ADDR 0x20

#define CUSTOM_3     0
#define CUSTOM_2     1
#define CUSTOM_1    2

#define BTN_MESHTASTIC     3
#define SWITCH_BUTTON    4

#define LED_B       5
#define LED_G    6
#define LED_R    7


size_t currentFirmware = 1;

bool boot_nvs()
{
  if (nvs_flash_init() != ESP_OK)
  {
    Port.println("Error al inicializar NVS. Reiniciando...");
    delay(1000);
    return false;
  }
  Port.println("[NVS] OK");
  return true;
}
bool boot_i2c()
{
  if (!mcp.begin_I2C())
  {
    Port.println("Error MCP.");
    return false;
  }
  Port.println("[I2C] OK");
  return true;
}
void boot_mcp()
{
  mcp.pinMode(BTN_MESHTASTIC, INPUT_PULLUP);
  mcp.pinMode(SWITCH_BUTTON, INPUT_PULLUP);
  //  Leds
  mcp.pinMode(LED_R, OUTPUT);
  mcp.pinMode(LED_G, OUTPUT);
  mcp.pinMode(LED_B, OUTPUT);

  delay(2000);
  mcp.digitalWrite(LED_R, LOW);
  delay(100);
  mcp.digitalWrite(LED_R, HIGH);
  mcp.digitalWrite(LED_G, HIGH);
  mcp.digitalWrite(LED_B, HIGH);
  delay(100);
  mcp.digitalWrite(LED_R, LOW);
  delay(100);
  mcp.digitalWrite(LED_R, HIGH);
  delay(100);
  mcp.digitalWrite(LED_R, LOW);
  mcp.digitalWrite(LED_G, LOW);
  mcp.digitalWrite(LED_B, LOW);
}


void beast() {
    Port.begin(115200);
    // Boot
    if (!boot_nvs())
    {
        esp_restart();
    }
    boot_i2c();
    boot_mcp();

    Port.println(ascii_art);

    currentFirmware = beast_worker_load_firmware_state();

    // Start Input Task 
    xTaskCreate(
        beast_worker_input_task,
        "Read Inputs",
        2048,
        NULL,
        1,
        NULL
    );
    Port.println("[Beast] OK");
}
#endif