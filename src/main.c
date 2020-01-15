/*
 * main.c for Smart Stairs Switch
 * Created on 10.01.2020
 * Author: Ivan Grigorov
 */

/*
*  Includes
*/
#include "mgos.h"
#include "system.h"

/*
*  Variables
*/

/*
 * periodicCallBackHandler: Function called periodically which updates system
 * 							variables.
 */
static void periodicCallBackHandler(void *arg) {
	return;
	(void)arg;
}

/*
 * mgos_app_init: Initializes application
 *
 */
enum mgos_app_init_result mgos_app_init(void) {

	cs_log_set_level(LL_DEBUG);
    LOG(LL_DEBUG, ("Starting system\n"));

	deviceInit();

	LOG(LL_DEBUG, ("Setup syscheck call back : %d sec \n", CALLBACK_PERIOD/1000));
	mgos_set_timer(CALLBACK_PERIOD, true, periodicCallBackHandler, NULL);

	LOG(LL_INFO, ("System Started"));
  return MGOS_APP_INIT_SUCCESS;
}

