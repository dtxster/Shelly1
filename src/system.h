/*
 * system.h for Smart Stairs Switch
 * 
 * Created on 10.01.2020
 * Author: Ivan Grigorov
 */

#ifndef SRC_SYSTEM_H_
#define SRC_SYSTEM_H_

/*
*  Includes
*/
#include "mgos.h"
#include "mgos_gpio.h"
#include "mgos_timers.h"


/*
*  Definitions
*/
#define SSID_SIZE 		32
#define PASSWORD_SIZE 	64
#define RELAY_DRV_GPIO	4
#define RELAY_KEY_GPIO	5
#define CALLBACK_PERIOD	1000

/*
*  Structures
*/
struct wifi_sta_settings {
  char ssid[SSID_SIZE];
  char pswd[PASSWORD_SIZE];
};

typedef enum{
	OFF,
	ON
}relay_t;

enum mode{
    Timer,
	Manual,
    Schedule
};

typedef struct{
    int seconds;
    mgos_timer_id id;
}mode_timer_t;

struct schedule_t{
	int min_start;
	int hour_start;
	int min_end;
	int hour_end;
};

typedef struct 
{
    relay_t relay;
	struct wifi_sta_settings wifi;
    enum mode mode;
    mode_timer_t delay;
    struct schedule_t schedule;
}system_state_t;

/*
*  Formatters
*/
#define STATUS_FMT "{cpu: %d, ssid: %Q, ip: %Q}"

void deviceInit(void);
void ButtonHandler(int pin, void *arg);
void cmdRelay_cb(void *arg);
void cmdRelay(int cmd);
void user_init(void);
void checkPumpSchedule(void);
void setDefaultSchedule(void);

// void becomeStation(struct sys_config_wifi_sta *device_cfg_sta);
void report_state(void);
void update_state(void);
bool saveSchedule(void);
bool getSchedule(void);

#endif /* SRC_SYSTEM_H_ */