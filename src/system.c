/*
 * system.c for Smart Stairs Switch
 * 
 * Created on 10.01.2020
 * Author: Ivan Grigorov
 */

#include "system.h"
#include <string.h>

system_state_t system_state;

/*
 * deviceInit: Configures peripherals used
 *
 */
void deviceInit(void)
{
	// Init GPIO
	LOG(LL_DEBUG, ("Initialize GPIO ports \n"));

	mgos_gpio_set_mode(RELAY_KEY_GPIO, MGOS_GPIO_MODE_INPUT);
	mgos_gpio_set_pull(RELAY_KEY_GPIO, MGOS_GPIO_PULL_NONE);

	mgos_gpio_set_mode(RELAY_DRV_GPIO, MGOS_GPIO_MODE_OUTPUT);
	mgos_gpio_set_pull(RELAY_DRV_GPIO, MGOS_GPIO_PULL_NONE);
	mgos_gpio_write(RELAY_DRV_GPIO, OFF);
    mgos_gpio_set_button_handler(RELAY_KEY_GPIO, MGOS_GPIO_PULL_NONE, MGOS_GPIO_INT_EDGE_POS, 100, ButtonHandler, NULL);

	// TODO - Get vlaue from config sss.timerval
	system_state.delay.seconds = 5;
	system_state.mode = Timer;

	LOG(LL_INFO, ("Switch delay %i seconds\n", system_state.delay.seconds));

	// Init Schedule
	if (!getSchedule()) {
		LOG(LL_INFO, ("Creating default schedule..."));
		setDefaultSchedule();
		saveSchedule();
		LOG(LL_INFO, ("Saved Schedule file schedule.txt\n"));
	}
	
	// TODO - MQTT Init
	mgos_mqtt_sub("shellysss/cmd", mqtt_cb, NULL);

}

/*
 * Button interrupt handler, button mode dependent commands
 *
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


/*
* MQTT Callback
*
*/
void mqtt_cb(struct mg_connection *nc, const char *topic,
                              int topic_len, const char *msg, int msg_len,
                              void *ud){

	if(mg_strncmp(mg_mk_str(msg), mg_mk_str("lamp_on"), 7) == 0){
		cmdRelay(ON);
	}
	else if(mg_strncmp(mg_mk_str(msg), mg_mk_str("lamp_off"), 8) == 0){
		cmdRelay(OFF);
	}

	(void)nc;
	(void)topic;
	(void)topic_len;
	(void)msg_len;
	(void)ud;
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