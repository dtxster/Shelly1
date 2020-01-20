/*
* mqtt.c for Smart Stairs Switch
* 
* Created on 20.01.2020
* Author: Ivan Grigorov
*/
#include "mqtt.h"
mqtt_cmd cmd;
/*
* mqttInit: Inits MQTT
*/ 
void mqttInit(void)
{
    mgos_config_set_mqtt_server(&mgos_sys_config, MQTT_BROOKER);
    // Subscribe to commands topic
    mgos_mqtt_sub("shellysss/cmd", mqtt_cb, NULL);
}
/*
* MQTT Callback
*/
void mqtt_cb(struct mg_connection *nc, const char *topic,
                              int topic_len, const char *msg, int msg_len,
                              void *ud){
    
    json_scanf(msg, strlen(msg), MACHINE_STATE_FMT, &cmd.stat, &cmd.mode, &cmd.delay);

	system_state.relay = !cmd.stat;
 	system_state.mode = cmd.mode;
	// mgos_sys_config_set_sss_mode(system_state.mode);
	system_state.delay.seconds = cmd.delay;
	// mgos_sys_config_set_sss_timerval(system_state.delay.seconds);
	ButtonHandler(1, NULL);

	(void)nc;
	(void)topic;
	(void)topic_len;
	(void)msg_len;
	(void)ud;
}
/*
* Send JSON string to device/stat topic
*/
void updateState(char *stat){
	if(strcmp(stat, "state") == 0){
		mgos_mqtt_pubf("shellysss/stat", 0, true, MACHINE_STATE_FMT, system_state.relay, system_state.mode, system_state.delay);	
	}
	else if (strcmp(stat, "device") == 0)
	{
		mgos_mqtt_pubf("shellysss/stat", 0, true, MACHINE_STATE_FMT, system_state.relay);
	}
}