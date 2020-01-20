/*
* mqtt.h for Smart Stairs Switch
* 
* Created on 20.01.2020
* Author: Ivan Grigorov
*/
#ifndef SRC_MQTT_H_
#define SRC_MQTT_H_
/*
*  Includes
*/
#include "system.h"
#include "mgos_mqtt.h"
#include "frozen.h"
/*
*  Enumeration/Structures
*/
typedef struct{
    bool stat;
    int mode;
    int delay;
}mqtt_cmd;

/*
*  JSON Formatters
*/
#define MACHINE_STATE_FMT "{relay: %B, mode: %d, delay: %d}"

/*
*  Function prototypes
*/
void mqttInit(void);
void updateState(char *stat);
void mqtt_cb(struct mg_connection *nc, const char *topic,
                              int topic_len, const char *msg, int msg_len,
                              void *ud);
#endif /* SRC_MQTT_H_ */