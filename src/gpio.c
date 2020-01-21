/*
* gpio.c for Smart Stairs Switch
* 
* Created on 20.01.2020
* Author: Ivan Grigorov
*/
#include "gpio.h"
/*
* gpioInit: Configures peripherals used
*/ 
void gpioInit(void){
 	LOG(LL_INFO, ("Initialize GPIO ports \n"));
	// Input
	mgos_gpio_set_mode(RELAY_KEY_GPIO, MGOS_GPIO_MODE_INPUT);
	mgos_gpio_set_pull(RELAY_KEY_GPIO, MGOS_GPIO_PULL_NONE);
	mgos_gpio_set_button_handler(RELAY_KEY_GPIO, MGOS_GPIO_PULL_NONE, MGOS_GPIO_INT_EDGE_POS, 100, ButtonHandler, NULL);
	// Output
	mgos_gpio_set_mode(RELAY_DRV_GPIO, MGOS_GPIO_MODE_OUTPUT);
	mgos_gpio_set_pull(RELAY_DRV_GPIO, MGOS_GPIO_PULL_NONE);
	mgos_gpio_write(RELAY_DRV_GPIO, OFF);
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
	
	if(!system_state.mode){
		system_state.delay.id = mgos_set_timer((system_state.delay.seconds*1000), false, cmdRelay_cb, NULL);
	}

	mgos_set_timer(1000, false, btnActive_cb, NULL);
	updateState("state");
	(void)arg;
	(void)pin;
}
/*
* Check switch level, if up- change mode and clear relay timer
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
*/
void cmdRelay_cb(void *arg){
	if(arg){
		cmdRelay(ON);
	}
	else{
		cmdRelay(OFF);
	}
	updateState("state");
	(void)arg;
}