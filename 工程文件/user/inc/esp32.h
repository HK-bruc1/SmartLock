#ifndef ESP32_H
#define ESP32_H

u8 Esp32_SendandReceive(u8 *cmd, u8 *respond, u32 wait_ms);
void Exit_Cipmode(void);
u8 ESP32_Restore(void);
void Esp32_Init(void);
u8 Esp32_Wificonnect(u8 *user, u8* password);
void open_Remote(void);

void mqtt_init(void);
void publish_close(void);
void ProcessUartData(void);
void ProcessESP32Data(uint8_t* data, uint16_t len);
void clean_buff(void);

#endif
