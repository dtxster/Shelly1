/*
 * system.c for Smart Stairs Switch
 * 
 * Created on 10.01.2020
 * Author: Ivan Grigorov
 */

#include "system.h"

system_state_t system_state;
struct mgos_config *device_cfg;

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
	LOG(LL_INFO, ("Switch delay %i seconds\n", system_state.delay.seconds));

	// Init Schedule
	if (!getSchedule()) {
		LOG(LL_INFO, ("Creating default schedule..."));
		setDefaultSchedule();
		saveSchedule();
		LOG(LL_INFO, ("Saved Schedule file schedule.txt\n"));
	}
	
	// TODO - MQTT Init
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
	
	// TODO - ne vzema timer ID i ne raboti timera / bez timerd trugva no samo 1 put ne go registrira pak
	system_state.delay.id = mgos_set_timer((system_state.delay.seconds*1000), false, cmdRelay_cb, false);
	
	mgos_msleep(1000);

	if(RELAY_KEY_GPIO){
		system_state.mode = Manual;
		mgos_clear_timer(system_state.delay.id);
	}

	(void)arg;
	(void)pin;
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