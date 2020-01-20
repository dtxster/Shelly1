/*
* system.c for Smart Stairs Switch
* 
* Created on 10.01.2020
* Author: Ivan Grigorov
*/

/*
*  Includes
*/
#include "system.h"
/*
* deviceInit: Init system, GPIO, Config, MQTT, cron
*/ 
void deviceInit(void)
{
	// Init GPIO
	gpioInit();
	// Get config
	system_state.mode = mgos_sys_config_get_sss_mode();
	system_state.delay.seconds = mgos_sys_config_get_sss_timerval();

	LOG(LL_INFO, ("System mode %s\n", system_state.mode ? "Manual": "Timer"));
	LOG(LL_INFO, ("Switch delay %i seconds\n", system_state.delay.seconds));

	// MQTT init
	mqttInit();
	// Schedule init
	scheduleSet();
}
/*
* Sets system scedule into cron
*/
void scheduleSet(void){
	// Init Schedule
	if (!getSchedule()) {
		LOG(LL_INFO, ("Creating default schedule..."));
		setDefaultSchedule();
		saveSchedule();
		LOG(LL_INFO, ("Saved Schedule file schedule.txt\n"));
	}
	if(system_state.mode == Schedule){

		if(system_state.schedule_mode){
			mgos_cron_add("@sunrise * * *", cmdRelay_cron_cb, NULL);
			mgos_cron_add("@sunset * * *", cmdRelay_cron_cb, NULL);
		}
		else if(!system_state.schedule_mode){
			// TODO -Add values from file for time
			mgos_cron_add("@sunrise * * *", cmdRelay_cron_cb, NULL);
			mgos_cron_add("@sunset * * *", cmdRelay_cron_cb, NULL);
		}
	}
}
/*
* Relay Cron Callback
*/
void cmdRelay_cron_cb(void *user_data, mgos_cron_id_t id){

	if(user_data){
		cmdRelay(ON);
	}
	else{
		cmdRelay(OFF);
	}
	(void)id;
}
/*
* Initialize structure with default schedule
*/
void setDefaultSchedule(void){
	system_state.schedule.hour_start = 18;
	system_state.schedule.min_start = 0;
	system_state.schedule.hour_end = 7;
	system_state.schedule.min_end = 0;
}
/*
* Save Schedule
*/
bool saveSchedule(void)
{
	FILE *fp;
	size_t size_of_elements = sizeof(system_state.schedule);
	size_t number_of_elements = sizeof(system_state.schedule)/size_of_elements;

	fp = fopen("schedule.txt", "wb");

	if (fp == NULL)
		return false;

	fwrite(&system_state.schedule, size_of_elements, number_of_elements, fp);
	LOG(LL_DEBUG, ("Wrote %d bytes\n", number_of_elements*size_of_elements));

    return true;
}
/*
* Get Schedule
*/
bool getSchedule(void)
{
	FILE *fp;
	size_t size_of_elements = sizeof(system_state.schedule);
	size_t number_of_elements = sizeof(system_state.schedule)/size_of_elements;

	fp = fopen("schedule.txt", "rb");

	if (fp == NULL)
		return false;

	fread(&system_state.schedule, size_of_elements, number_of_elements, fp);
	LOG(LL_DEBUG, ("Read %d bytes\n", number_of_elements*size_of_elements));

    return true;
}