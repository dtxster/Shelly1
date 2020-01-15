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
#include "mgos_wifi.h"
#include "mgos_http_server.h"

/*
*  Variables
*/

/*
 * handle_get_cpu_usage. This reports CPU usage, and also provides connection status
 *
 */
static void handle_get_cpu_usage(struct mg_connection *nc) {

	int cpu_usage = 0;
	struct mbuf fb;
	struct json_out fout = JSON_OUT_MBUF(&fb);
	mbuf_init(&fb, 256);

	cpu_usage = ((double)mgos_get_free_heap_size()/(double)mgos_get_heap_size()) * 100.0;
	json_printf(&fout, STATUS_FMT, cpu_usage, mgos_wifi_get_connected_ssid(), mgos_wifi_get_status_str());
    mbuf_trim(&fb);

	struct mg_str f = mg_mk_str_n(fb.buf, fb.len);	/* convert to string	*/

	// Use chunked encoding in order to avoid calculating Content-Length
	mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
	//	mg_printf_http_chunk(nc, f.p);
	mg_send_http_chunk(nc, f.p, f.len);
	mg_send_http_chunk(nc, "", 0);
	nc->flags |= MG_F_SEND_AND_CLOSE;

	LOG(LL_INFO, ("%s\n", f.p));

	mbuf_free(&fb);
}

/*
 * HTTP Event Handler
 *
 */
static void http_get_ev_handler(struct mg_connection *nc, int ev, void *ev_data, void *user_data) {
  struct http_message *hm = (struct http_message *) ev_data;

  printf("Event: %d, uri: %s\n", ev, hm->uri.p);

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      if (mg_vcmp(&hm->uri, "/get_cpu_usage") == 0) {
        handle_get_cpu_usage(nc);
      }
      else {
    	mg_http_send_redirect(nc, 302, mg_mk_str("/"), mg_mk_str(NULL));
      }
      break;
    default:
      break;
  }
  (void) user_data;
}

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
	printf ("mgos_app_init Device id is: Shelly_S\r\n");
	deviceInit();
	mgos_register_http_endpoint("/get_cpu_usage", http_get_ev_handler, NULL);

	LOG(LL_DEBUG, ("Setup syscheck call back : %d sec \n", CALLBACK_PERIOD/1000));
	mgos_set_timer(CALLBACK_PERIOD, true, periodicCallBackHandler, NULL);

	LOG(LL_INFO, ("System Started"));
  return MGOS_APP_INIT_SUCCESS;
}

