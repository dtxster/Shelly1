/*
* mqtt.c for Smart Stairs Switch
* 
* Created on 20.01.2020
* Author: Ivan Grigorov
*/
#include "mqtt.h"
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
	if(mg_strncmp(mg_mk_str(msg), mg_mk_str("lamp_on"), 7) == 0){
		system_state.relay = OFF;
		ButtonHandler(1, NULL);
		LOG(LL_DEBUG, ("MQTT lamp On"));
	}
	else if(mg_strncmp(mg_mk_str(msg), mg_mk_str("lamp_off"), 8) == 0){
		system_state.relay = ON;
		ButtonHandler(1, NULL);
		LOG(LL_DEBUG, ("MQTT lamp Off"));
	}
	else if(mg_strncmp(mg_mk_str(msg), mg_mk_str("mode_timer"), 10) == 0){
		system_state.mode = Timer;
		mgos_sys_config_set_sss_mode(system_state.mode);
		LOG(LL_DEBUG, ("MQTT mode timer"));
	}
	else if(mg_strncmp(mg_mk_str(msg), mg_mk_str("mode_manual"), 11) == 0){
		system_state.mode = Manual;
		mgos_sys_config_set_sss_mode(system_state.mode);
		LOG(LL_DEBUG, ("MQTT mode manual"));
	}
	else if(mg_strncmp(mg_mk_str(msg), mg_mk_str("mode_schedule"), 11) == 0){
		system_state.mode = Schedule;
		mgos_sys_config_set_sss_mode(system_state.mode);
		LOG(LL_DEBUG, ("MQTT schedule on"));
	}
	else if(mg_strncmp(mg_mk_str(msg), mg_mk_str("schedule_mode_hour"), 12) == 0){
		system_state.schedule_mode = Hour;
		scheduleSet();
		LOG(LL_DEBUG, ("MQTT schedule mode hour"));
	}
	else if(mg_strncmp(mg_mk_str(msg), mg_mk_str("schedule_mode_sun"), 12) == 0){
		system_state.schedule_mode = Sun;
		scheduleSet();
		LOG(LL_DEBUG, ("MQTT schedule mode sun"));
	}
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