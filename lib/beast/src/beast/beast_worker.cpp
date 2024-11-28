#include "beast_worker.h"

void beast_worker_save_firmware_state(size_t app) {
  nvs_handle_t nvs_handle;
  if (nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
    nvs_set_u8(nvs_handle, "firmware", app ? 1 : 0);
    nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
  } else {
    Port.println("[NVS] Error on save");
  }
}
void beast_worker_switch_firmware(size_t app) {
  beast_worker_save_firmware_state(app);

  const esp_partition_t* particion = nullptr;

  if (app) {
    particion = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
  } else {
    particion = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, NULL);
  }

  if (particion != NULL) {
    esp_err_t err = esp_ota_set_boot_partition(particion);
    if (err == ESP_OK) {
      esp_restart();
    } else {
      Port.printf("[NVS] Error configuration %s\n", esp_err_to_name(err));
    }
  } else {
    Port.println("[NVS] Error Partition");
  }
}
size_t beast_worker_load_firmware_state() {
  nvs_handle_t nvs_handle;
  uint8_t firnware = 1;
  if (nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
    nvs_get_u8(nvs_handle, "firmware", &firnware);
    nvs_close(nvs_handle);
  } else {
    Port.println("[NVS] Error load.");
  }
  return firnware == 1;
}

void beast_worker_input_task(void *pvParameters)
{
  while (true)
  {
    uint16_t pin_states = mcp.readGPIO(0);
    uint8_t switch_state = (pin_states >> SWITCH_BUTTON) & 0x01;
    uint8_t btn_meshtastic_state = (pin_states >> BTN_MESHTASTIC) & 0x01;

    if (switch_state != currentFirmware) {
        currentFirmware = switch_state;
        beast_worker_switch_firmware(currentFirmware); 
    }
    vTaskDelay(pdMS_TO_TICKS(100)); // 100 ms
  }
}