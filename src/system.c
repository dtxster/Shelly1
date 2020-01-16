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
*  Variables
*/
system_state_t system_state;
struct mgos_config sys_config;

/*
 * deviceInit: Configures peripherals used
 */
void deviceInit(void)
{
	// Init GPIO
	LOG(LL_INFO, ("Initialize GPIO ports \n"));
	// Input
	mgos_gpio_set_mode(RELAY_KEY_GPIO, MGOS_GPIO_MODE_INPUT);
	mgos_gpio_set_pull(RELAY_KEY_GPIO, MGOS_GPIO_PULL_NONE);
	mgos_gpio_set_button_handler(RELAY_KEY_GPIO, MGOS_GPIO_PULL_NONE, MGOS_GPIO_INT_EDGE_POS, 100, ButtonHandler, NULL);
	// Output
	mgos_gpio_set_mode(RELAY_DRV_GPIO, MGOS_GPIO_MODE_OUTPUT);
	mgos_gpio_set_pull(RELAY_DRV_GPIO, MGOS_GPIO_PULL_NONE);
	mgos_gpio_write(RELAY_DRV_GPIO, OFF);

	// TODO - Get config values from config sss.
	system_state.mode = mgos_sys_config_get_sss_mode();
	system_state.delay.seconds = mgos_sys_config_get_sss_timerval();

	LOG(LL_INFO, ("System mode %s\n", system_state.mode ? "Schedule": "Timer"));
	LOG(LL_INFO, ("Switch delay %i seconds\n", system_state.delay.seconds));

	// Init Schedule
	if (!getSchedule()) {
		LOG(LL_INFO, ("Creating default schedule..."));
		setDefaultSchedule();
		saveSchedule();
		LOG(LL_INFO, ("Saved Schedule file schedule.txt\n"));
	}
	
	// MQTT init
	mgos_mqtt_sub("shellysss/cmd", mqtt_cb, NULL);
	// Schedule init
	scheduleMode();
}
/*
* MQTT Callback
*/
void mqtt_cb(struct mg_connection *nc, const char *topic,
                              int topic_len, const char *msg, int msg_len,
                              void *ud){
	if(mg_strncmp(mg_mk_str(msg), mg_mk_str("lamp_on"), 7) == 0){
		cmdRelay(ON);
		LOG(LL_DEBUG, ("MQTT lamp Of"));
	}
	else if(mg_strncmp(mg_mk_str(msg), mg_mk_str("lamp_off"), 8) == 0){
		cmdRelay(OFF);
		LOG(LL_DEBUG, ("MQTT lamp Off"));
	}
	else if(mg_strncmp(mg_mk_str(msg), mg_mk_str("mode_timer"), 10) == 0){
		system_state.mode = Timer;
		LOG(LL_DEBUG, ("MQTT mode timer"));
	}
	else if(mg_strncmp(mg_mk_str(msg), mg_mk_str("mode_manual"), 11) == 0){
		system_state.mode = Manual;
		LOG(LL_DEBUG, ("MQTT mode manual"));
	}
	else if(mg_strncmp(mg_mk_str(msg), mg_mk_str("mode_schedule"), 11) == 0){
		system_state.mode = Schedule;
		scheduleMode();
		LOG(LL_DEBUG, ("MQTT schedule on"));
	}
	else if(mg_strncmp(mg_mk_str(msg), mg_mk_str("schedule_mode_hour"), 12) == 0){
		system_state.schedule_mode = Hour;
		scheduleMode();
		LOG(LL_DEBUG, ("MQTT schedule mode hour"));
	}
	else if(mg_strncmp(mg_mk_str(msg), mg_mk_str("schedule_mode_sun"), 12) == 0){
		system_state.schedule_mode = Sun;
		scheduleMode();
		LOG(LL_DEBUG, ("MQTT schedule mode sun"));
	}
	(void)nc;
	(void)topic;
	(void)topic_len;
	(void)msg_len;
	(void)ud;
}
/*
 * Button interrupt handler, button mode dependent commands
 */
void scheduleMode(void){

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
 * Button interrupt handler, button mode dependent commands
 */
void ButtonHandler(int pin, void *arg){

	if(!system_state.relay){
		system_state.relay = ON;
	}
	else{
		system_state.relay = OFF;

		if(system_state.mode){
			system_state.mode = Timer;
		}
	}
	cmdRelay(system_state.relay);
	
	if(system_state.mode){
		system_state.delay.id = mgos_set_timer((system_state.delay.seconds*1000), false, cmdRelay_cb, NULL);
	}

	mgos_set_timer(1000, false, btnActive_cb, NULL);
	(void)arg;
	(void)pin;
}

/*
 * Check switch level, if up- change mode and clear relay timer
 *
 */
void btnActive_cb(void *arg){

	if(mgos_gpio_read(RELAY_KEY_GPIO)){
		system_state.mode = Manual;
		mgos_clear_timer(system_state.delay.id);
	}
	(void)arg;
}


/*
 * Relay Control
 *
 */
void cmdRelay(int cmd)
{
    if (cmd){
		mgos_gpio_write(RELAY_DRV_GPIO, 1);
	}
	else{
		mgos_gpio_write(RELAY_DRV_GPIO, 0);
	}
}

/*
 * Relay Callback
 *
 */
void cmdRelay_cb(void *arg){
	if(arg){
		cmdRelay(ON);
	}
	else{
		cmdRelay(OFF);
	}
	
	(void)arg;
}

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
 * setDefaultSchedule: Initialize structure with default
 * schedule
 *
 */
void setDefaultSchedule(void){
	system_state.schedule.hour_start = 18;
	system_state.schedule.min_start = 0;
	system_state.schedule.hour_end = 7;
	system_state.schedule.min_end = 0;
}


/*
 * Save Schedule
 *
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
 *
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