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
#include "mgos_cron.h"
#include "gpio.h"
#include "mqtt.h"

/*
*  Definitions
*/
#define SSID_SIZE 		32
#define PASSWORD_SIZE 	64
#define RELAY_DRV_GPIO	4
#define RELAY_KEY_GPIO	5
#define CALLBACK_PERIOD	1000
#define MQTT_BROOKER "10.10.0.243:1883"
/*
*  JSON Formatters
*/

/*
*  Enumeration/Structures
*/
enum relay{
	OFF,
	ON
};

enum sysmode{
    Timer,
    Schedule,
    Manual
};

struct mode_timer{
    int seconds;
    mgos_timer_id id;
};

struct mode_schedule{
    int min_start;
	int hour_start;
	int min_end;
	int hour_end;
    bool enabled;
};

enum schedule_mode{
    Hour,
	Sun
};

struct wifi_sta_settings {
  char ssid[SSID_SIZE];
  char pswd[PASSWORD_SIZE];
};

typedef struct 
{
    enum relay relay;
    enum sysmode mode;
    struct mode_timer delay;
    struct mode_schedule schedule;
    enum schedule_mode schedule_mode;
    struct wifi_sta_settings wifi;
}system_state_t;

system_state_t system_state;
/*
*  Function prototypes
*/
void deviceInit(void);

void setDefaultSchedule(void);
bool saveSchedule(void);
bool getSchedule(void);

void cmdRelay_cron_cb(void *user_data, mgos_cron_id_t id);
void scheduleSet(void);

#endif /* SRC_SYSTEM_H_ */