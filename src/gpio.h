/*
* gpio.h for Smart Stairs Switch
* 
* Created on 20.01.2020
* Author: Ivan Grigorov
*/
#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_
/*
*  Includes
*/
#include "mgos_gpio.h"
#include "mgos_timers.h"
#include "system.h"
/*
*  Definitions
*/
#define RELAY_DRV_GPIO	4
#define RELAY_KEY_GPIO	5

/*
*  JSON Formatters
*/

/*
*  Enumeration/Structures
*/

/*
*  Function prototypes
*/
void gpioInit(void);
void ButtonHandler(int pin, void *arg);
void btnActive_cb(void *arg);
void cmdRelay_cb(void *arg);
void cmdRelay(int cmd);
#endif /* SRC_GPIO_H_ */