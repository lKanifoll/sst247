#ifndef TCP_MQTT_H_
#define TCP_MQTT_H_

#include "device_init.h"


typedef enum {OKEY = 0, UNKNOW = 1, ERR = 3, ECRC = 4, TIMEOUT = 5, NOACCESS = 6} EX_Response_TypeDef;

int s;
char recv_buf[600];
uint8_t MQTT_Encod_size[2];
uint8_t Buffer[1000],BufTmp[300];
uint16_t lenght, lenght_p;


void buffer_add_char(uint8_t, uint16_t);
void buffer_add_string(uint8_t *new_part, uint16_t position, uint16_t count);
void buffer_add_int(uint16_t new_int, uint16_t position);


void tcp_server(void *ptr);
void udp_server(void *ptr);

uint16_t EX_Send_Config(uint16_t pos);
uint8_t EX_Save_Config(uint8_t *buf, uint16_t count, uint16_t size);


void EX_Instructions_Read(struct netconn *conn);
void EX_Instructions_Write(struct netconn *conn, uint8_t *buf, uint16_t count, uint16_t size);
void EX_response(EX_Response_TypeDef response,struct netconn *conn);
void tcp_server_service();
void udp_server_service();
uint16_t CRC16(uint8_t *DATA, uint16_t length);
uint16_t CRC16_2(uint8_t *DATA, uint16_t start, uint16_t length);

void MQTT_Encoding_size(uint8_t* encoding_result, uint16_t size);
void mqtt_subscribe(void *ptr);
bool Connect_mqttserver();
#endif


