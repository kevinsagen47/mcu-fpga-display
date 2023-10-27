/******************************************************************************
 * @file     webserver.c
 * @version  V1.00
 * $Revision: 8 $
 * $Date: 19/06/12 15:59p $
 * @brief    uIP http server deamon.
 *
 * @note
 * Copyright (C) All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NuMicro.h"
#include "uip.h"
#include "uip_arp.h"
#include "ultrasound_options.h"
#include "html_itri_favicon.h"
#ifdef __SUPPORT_LOGIN_PAGE_
#include "login.gz.h"
#endif

// header of web page html files
#include "driver_correction.gz.h"
#include "main_page_1.gz.h"
#include "main_page_2.gz.h"
#include "observe_current_parameter_1.gz.h"
#include "observe_current_parameter_2.gz.h"
#include "observe_current_parameter_3.gz.h"
#include "observe_current_parameter_4.gz.h"
#include "observe_current_parameter_5.gz.h"
#include "parameter_setting_page_1.gz.h"
#include "parameter_setting_page_2.gz.h"
#include "parameter_setting_page_sec_vib.gz.h"
#include "post_vib.gz.h"
#include "pre_vib.gz.h"
#include "store_and_load.gz.h"
#include "system_configuration.gz.h"
#include "system_information.gz.h"
#include "welding_head_diagnosis.gz.h"
#include "welding_head_drop.gz.h"
#include "welding_record.gz.h"

// header of css files
#include "common_css.h"
#include "store_and_load_css.h"
#include "system_information_css.h"

// header of data base api
#include "dependencies.h"

#pragma diag_suppress 870

#define MAX_HTML_SCRIPT_PER_PACKET 1460
#define URL_FILENAME_LENGTH 64
#define JSON_KEY_BUFFER_SIZE 0x40
#define JSON_VALUE_BUFFER_SIZE 0x40
#define FETCH_JSON_BUFFER_SIZE 256

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

// external function declaration
void mcu_to_fpga(void);

extern int history_record_array[255][18];
extern unsigned int history_point_set;
int read_freq_delta(void);
unsigned int read_freq_start(void);
unsigned int read_freq_end(void);
unsigned int read_freq_max(void);
unsigned int read_freq_min(void);
unsigned int read_P_max(void);
unsigned int read_avg_power(void);
unsigned int read_energy_display(void);
unsigned int read_time_on(void);
unsigned int read_F_set_history(void);
unsigned int read_F_start(void);
unsigned int read_F_max(void);
unsigned int read_encoder_speed_history(void);
unsigned int read_distance_reached(void);
unsigned int read_absolute_hold(void);
unsigned int read_distance_travelled(void);
unsigned int read_collapse_hold(void);
unsigned int read_total_time_display(void);

// local function declaration
char send_response(void);
char bVerifyPassword(const char *packet_data, int packet_len);
char parseURL(char *html_filename);
char parsing_json(const char *post_data, int data_len, char *json_key, char *json_value);
void initParameter(void);
void update_parameter_setting_page_1(char *json_key, char *json_value);
void update_parameter_setting_page_2(char *json_key, char *json_value);
void update_parameter_setting_page_seg_vib(char *json_key, char *json_value);
void update_parameter_setting_page_post_vib(char *json_key, char *json_value);
void update_parameter_setting_page_pre_vib(char *json_key, char *json_value);
void base64_decode(const char *data, unsigned char input_length, unsigned char *decoded_data, unsigned char output_length);
char bIsContinuationPacket(const char *packet_data, int packet_len);
void pack_css_data(u8_t *css_hex_ptr, int css_hex_len);								

// password of login page
#ifdef __ENCODED_PASSWORD_
const char *LOGIN_KEY = "authentication";
const char *LOGIN_VALUE = "TW1zbEUxMDA=";
#endif
const char *LOGIN_PASSWORD = "MmslE100";

// string format of fetch get for parameter pages
const char *JSON_PARAMETER_SETTING_PAGE_1 = "{\"mode_select\":\"%d\",\"mode_value\":\"%d\",\"pre_vib\":\"%d\",\"post_vib\":\"%d\",\"pressure_time\":\"%d\",\"trigger_pressure\":\"%d\",\"vib_mode\":\"%d\",\"vib_amp\":\"%d\"}";
const char *JSON_PARAMETER_SETTING_PAGE_2 = "{\"defect_boundary\":\"%d\",\"abort_time\":\"%d\",\"start_freq\":\"%d\"}";
const char *JSON_PARAMETER_SETTING_PAGE_SEC_VIB = "{\"sec_vib_mode\":\"%d\",\"sec_vib_value\":\"%d\",\"vib_a\":\"%d\",\"vib_b\":\"%d\"}";
const char *JSON_PARAMETER_SETTING_PAGE_POST_VIB = "{\"post_vib_mode\":\"%d\",\"post_vib_value\":\"%d\",\"vib_amp\":\"%d\",\"vib_time\":\"%d\"}";
const char *JSON_PARAMETER_SETTING_PAGE_PRE_VIB = "{\"pre_vib_mode\":\"%d\",\"pre_vib_value\":\"%d\",\"vib_amp\":\"%d\"}";

enum __RESPONSE_HEADER_TYPE_ {
    RESPONSE_HEADER_TYPE_OK = 0,
	RESPONSE_HEADER_TYPE_CSS,
    RESPONSE_HEADER_TYPE_FAVICON,
	RESPONSE_HEADER_TYPE_JSON,
	RESPONSE_HEADER_TYPE_GZIP,
	RESPONSE_HEADER_TYPE_GZIP_404,
    RESPONSE_HEADER_TYPE_404
};

const char *http_header_array[] = {
	"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n\r\n",
	"HTTP/1.1 200 OK\r\nContent-Type: text/css\r\nConnection: keep-alive\r\n\r\n",
	"HTTP/1.1 200 OK\r\nContent-Type: image/ico\nConnection: keep-alive\r\n\r\n",
	"HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nConnection: keep-alive\r\nTransfer-Encoding: chunked\r\n\r\n",
	"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: keep-alive\r\nContent-Encoding: gzip\r\n\r\n",
	"HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nConnection: keep-alive\r\nContent-Encoding: gzip\r\n\r\n",
	"HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n"
};

struct http_server_state
{
    u8_t time_out;
    u8_t connect_flag;
};

char *html_filename_array[] = {
	"main_page_1.html",
	"main_page_2.html",
	"parameter_setting_page_1.html",
	"parameter_setting_page_2.html",
	"parameter_setting_page_sec_vib.html",
	"post_vib.html",
	"pre_vib.html",
	"observe_current_parameter_1.html",
	"observe_current_parameter_2.html",
	"observe_current_parameter_3.html",
	"observe_current_parameter_4.html",
	"observe_current_parameter_5.html",
	"system_information.html",
	"system_configuration.html",
	"driver_correction.html",	
	"welding_record.html",
	"welding_head_diagnosis.html",
	"welding_head_drop.html",
	"store_and_load.html"
};

const unsigned char *html_header_file_array[] = {
	main_page_1_hex,
	main_page_2_hex,
	parameter_setting_page_1_hex,
	parameter_setting_page_2_hex,
	parameter_setting_page_sec_vib_hex,
	post_vib_hex,
	pre_vib_hex,
	observe_current_parameter_1_hex,
	observe_current_parameter_2_hex,
	observe_current_parameter_3_hex,
	observe_current_parameter_4_hex,
	observe_current_parameter_5_hex,
	system_information_hex,
	system_configuration_hex,
	driver_correction_hex,
	welding_record_hex,
	welding_head_diagnosis_hex,
	welding_head_drop_hex,
	store_and_load_hex,
};

const int html_header_file_size[] = {
	sizeof(main_page_1_hex),	
	sizeof(main_page_2_hex),
	sizeof(parameter_setting_page_1_hex),
	sizeof(parameter_setting_page_2_hex),
	sizeof(parameter_setting_page_sec_vib_hex),
	sizeof(post_vib_hex),
	sizeof(pre_vib_hex),	
	sizeof(observe_current_parameter_1_hex),
	sizeof(observe_current_parameter_2_hex),
	sizeof(observe_current_parameter_3_hex),
	sizeof(observe_current_parameter_4_hex),
	sizeof(observe_current_parameter_5_hex),
	sizeof(system_information_hex),
	sizeof(system_configuration_hex),
	sizeof(driver_correction_hex),
	sizeof(welding_record_hex),
	sizeof(welding_head_diagnosis_hex),
	sizeof(welding_head_drop_hex),
	sizeof(store_and_load_hex)
};

/*const char *pseudo_welding_record[][15] = {
  {"2022/1/1", "1", "60", "100", "0.5", "0.5", "0.05", "100", "100", "100", "100", "100", "10", "90", "50"},
  {"2022/2/1", "2", "120", "112", "0.5", "0.5", "0.06", "100", "100", "100", "100", "100", "10", "90", "50"},
  {"2022/3/1", "3", "180", "80", "0.6", "0.6", "0.04", "100", "100", "100", "100", "100", "10", "90", "50"},
  {"2022/4/1", "1", "60", "96", "0.6", "0.6", "0.05", "100", "100", "100", "100", "100", "10", "90", "50"},
  {"2022/5/1", "5", "300", "120", "0.8", "0.8", "0.02", "100", "100", "100", "100", "100", "10", "90", "50"},
  {"2022/6/1", "3", "180", "151", "0.9", "0.9", "0.01", "100", "100", "100", "100", "100", "10", "90", "50"},
  {"2022/7/1", "8", "480", "178", "1.1", "1.1", "0.05", "100", "100", "100", "100", "100", "10", "90", "50"},
  {"2022/8/1", "2", "120", "105", "0.4", "0.4", "0.03", "100", "100", "100", "100", "100", "10", "90", "50"},
  {"2022/9/1", "3", "180", "102", "0.7", "0.7", "0.02", "100", "100", "100", "100", "100", "10", "90", "50"},
  {"2022/10/1", "9", "540", "111", "1.0", "1.1", "0.01", "100", "100", "100", "100", "100", "10", "90", "50"},	
};*/

// global variable for parameter pages
char ultrasound_active_mode = 1;  // 1 is time mode by default
char ultrasound_sec_vib_mode = 1;  // 1 is time mode by default

// global variable for buffer
char http_output_buf[2048];
char http_table_buf[2048];

// global variable for multiple httpd_appcall entrance
u8_t *transfer_ptr = NULL;
int transfer_len = 0;
char flag_pass_authentication = 0;
char flag_post_continuation = 0;
char flag_post_to_login_continuation = 0;
char flag_post_appdata_found_json = 0;
char flag_check_password = 0;  
char flag_chunked_transfer = 0;
char post_to_modify_parameter = 0;  // 1 for page 1, 2 for page 2, 3 for page segment amp

// external global variable
extern uint8_t uartInStr[];
extern uint8_t uartInEvent;

// uip variable
static uint8_t rxbuf[1514];
static uint8_t txbuf[1514];
uint32_t volatile u32PktLen;

// Our MAC address
struct uip_eth_addr ethaddr = {{0x00, 0x00, 0x00, 0x59, 0x16, 0x88}};

// timer variable
uint32_t volatile curTime = 0;
uint32_t volatile prevTime = 0;  // increase every 0.5 sec

void httpd_init(void)
{
    uip_listen(HTONS(80));
}

void httpd_appcall(void)
{
    struct http_server_state *hs;
	char transfer_header_flag = 0;	// 1 means to transfer header before body (i.e. transfer_ptr doesn't contain header field)
	enum __RESPONSE_HEADER_TYPE_ transfer_header_type = RESPONSE_HEADER_TYPE_OK;
	char i = 0;
	char j = 0;
	char html_filename[URL_FILENAME_LENGTH] = {0};
	char json_key[JSON_KEY_BUFFER_SIZE] = {0};
	char json_value[JSON_VALUE_BUFFER_SIZE] = {0};
	char fetch_json_buf[FETCH_JSON_BUFFER_SIZE] = {0};
	
    hs = (struct http_server_state *)(uip_conn->appstate);
	
    if (uip_connected())
    {
		#ifdef __ENABLE_DEBUG_PRINTF_
        printf("http server connected !\n");
		#endif
        hs->connect_flag = 1;
        hs->time_out = 0;
    }
    else if (uip_newdata())	
    {
        if ((uip_len >= 5) && (strncmp((const char *)uip_appdata, "GET /", 5) == 0))
        {
			if (strncmp((const char *)uip_appdata, "GET /favicon.ico", strlen("GET /favicon.ico")) == 0)
			{			
				#ifdef __ENABLE_DEBUG_PRINTF_
				printf("GET /favicon.ico \n");
				#endif
				transfer_ptr = (u8_t *)favicon_hex;
				transfer_len = sizeof(favicon_hex);
				transfer_header_flag = 1;
				transfer_header_type = RESPONSE_HEADER_TYPE_FAVICON;
			}			
			else
            {
				if ((strncmp((char const *)uip_appdata, "GET /index.html", 15) == 0) || (strncmp((char const *)uip_appdata, "GET / ", 6) == 0))
				{	
					#ifdef __ENABLE_DEBUG_PRINTF_
					printf("GET /index.html or GET / \n");
					#endif
				
					#ifdef __SUPPORT_LOGIN_PAGE_
					transfer_ptr = (u8_t *)login_hex + 2;
					transfer_len = sizeof(login_hex) - 2;
					transfer_header_flag = 1;
					transfer_header_type = RESPONSE_HEADER_TYPE_GZIP;
					#else  // if not to support login page, show up main page 1 instead
					transfer_ptr = (u8_t *)html_main_page_1_hex;
					transfer_len = sizeof(html_main_page_1_hex);
					transfer_header_flag = 1;
					#endif										                		                
				}					
				else if (strncmp((const char *)uip_appdata, "GET /css/", strlen("GET /css/")) == 0)
				{							
					if (strncmp((const char *)uip_appdata, "GET /css/common_css.css", strlen("GET /css/common_css.css")) == 0)
					{			
						#ifdef __ENABLE_DEBUG_PRINTF_
						printf("GET /css/common_css.css \n");
						#endif
				
						transfer_len = strlen(http_header_array[RESPONSE_HEADER_TYPE_CSS]) + sizeof(common_css_hex);
						if (transfer_len < MAX_HTML_SCRIPT_PER_PACKET)
						{
							pack_css_data((u8_t *)common_css_hex, sizeof(common_css_hex));		
							transfer_ptr = (u8_t *)http_output_buf;									
							transfer_header_flag = 0;
						}
						else
						{
							transfer_ptr = (u8_t *)common_css_hex;
							transfer_len = sizeof(common_css_hex);
							transfer_header_flag = 1;
							transfer_header_type = RESPONSE_HEADER_TYPE_CSS;
						}
					}		
					else if (strncmp((const char *)uip_appdata, "GET /css/system_information_css.css", strlen("GET /css/system_information_css.css")) == 0)
					{			
						#ifdef __ENABLE_DEBUG_PRINTF_
						printf("GET /css/system_information_css.css \n");
						#endif
						
						transfer_len = strlen(http_header_array[RESPONSE_HEADER_TYPE_CSS]) + sizeof(system_information_css_hex);
						if (transfer_len < MAX_HTML_SCRIPT_PER_PACKET)  // improve performance by reduce TCP transaction
						{
							pack_css_data((u8_t *)system_information_css_hex, sizeof(system_information_css_hex));		
							transfer_ptr = (u8_t *)http_output_buf;									
							transfer_header_flag = 0;
						}
						else
						{
							transfer_ptr = (u8_t *)system_information_css_hex;
							transfer_len = sizeof(system_information_css_hex);
							transfer_header_flag = 1;
							transfer_header_type = RESPONSE_HEADER_TYPE_CSS;
						}
					}
					else if (strncmp((const char *)uip_appdata, "GET /css/store_and_load.css", strlen("GET /css/store_and_load.css")) == 0)
					{			
						#ifdef __ENABLE_DEBUG_PRINTF_
						printf("GET /css/store_and_load.css \n");
						#endif
						
						transfer_len = strlen(http_header_array[RESPONSE_HEADER_TYPE_CSS]) + sizeof(store_and_load_css_hex);
						if (transfer_len < MAX_HTML_SCRIPT_PER_PACKET)  // improve performance by reduce TCP transaction
						{
							pack_css_data((u8_t *)store_and_load_css_hex, sizeof(store_and_load_css_hex));		
							transfer_ptr = (u8_t *)http_output_buf;									
							transfer_header_flag = 0;
						}
						else
						{
							transfer_ptr = (u8_t *)store_and_load_css_hex;
							transfer_len = sizeof(store_and_load_css_hex);
							transfer_header_flag = 1;
							transfer_header_type = RESPONSE_HEADER_TYPE_CSS;
						}
					}		
				}				
				else
				{
					if (flag_pass_authentication == 1)
					{
						if (strncmp((const char *)uip_appdata, "GET /html/", strlen("GET /html/")) == 0)
						{				
							if (parseURL(html_filename) == 1)
							{
								transfer_ptr = NULL;
								transfer_len = 0;
								transfer_header_flag = 1;
								for (i = 0; i < (sizeof(html_filename_array)/sizeof(char *)); i++)
								{										
									if (strncmp((const char *)html_filename, html_filename_array[i], strlen(html_filename)) == 0)
									{
										#ifdef __ENABLE_DEBUG_PRINTF_
										printf("HTTP GET URL %s with length %d bytes \n", html_filename, html_header_file_size[i] - 2);
										#endif
										transfer_ptr = (u8_t *)(html_header_file_array[i]) + 2;											
										transfer_len = html_header_file_size[i] - 2;
										transfer_header_type = RESPONSE_HEADER_TYPE_GZIP;
										break;
									}
								}
							}
							else
							{
								#ifdef __ENABLE_DEBUG_PRINTF_
								printf("Unknown GET /html/ request \n");
								#endif
							}													
						}
						else if (strncmp((const char *)uip_appdata, "GET /parameter/page_1.html", strlen("GET /parameter/page_1.html")) == 0)
						{
							#ifdef __ENABLE_DEBUG_PRINTF_
							printf("GET /parameter/page_1.html \n");
							#endif
							// const char *JSON_PARAMETER_SETTING_PAGE_1 = "{\"mode_select\":\"%d\",\"mode_value\":\"%d\",\"pre_vib\":\"%d\",\"post_vib\":\"%d\",\"pressure_time\":\"%d\",\"trigger_pressure\":\"%d\",\"vib_mode\":\"%d\",\"vib_amp\":\"%d\"}";
							#ifdef __FPGA_MODE_
							unsigned int mode_value_tmp = 0;
							switch(read_mode_set()) {
								case 2:
									mode_value_tmp = read_distance_relative_set_display();
									break;
								case 3:
									mode_value_tmp = read_distance_absolute_set_display();
									break;
								case 5:
									mode_value_tmp = read_energy_set_display();
									break;
								default:
									mode_value_tmp = read_time_set_stage_one_display();
									break;
							}
							sprintf(fetch_json_buf, JSON_PARAMETER_SETTING_PAGE_1, 
									read_mode_set(), mode_value_tmp,
									read_mode_early_stage_display(), read_mode_after_stage_display(),
									read_hold_time_display(), read_force_set_display(),
									read_stage2_mode_address_display(), read_amplitude_set_display());
							#else
							sprintf(fetch_json_buf, JSON_PARAMETER_SETTING_PAGE_1, 
									parameter_page_1_value_array[0], parameter_page_1_value_array[1],
									parameter_page_1_value_array[2], parameter_page_1_value_array[3],
									parameter_page_1_value_array[4], parameter_page_1_value_array[5],
									parameter_page_1_value_array[6], parameter_page_1_value_array[7]);							
							#endif
							transfer_len = sprintf(http_output_buf, "%s%x\r\n%s\r\n0\r\n\r\n", http_header_array[RESPONSE_HEADER_TYPE_JSON], strlen(fetch_json_buf), fetch_json_buf);
							transfer_ptr = (u8_t *)http_output_buf;	
							transfer_header_flag = 0;
						}			
						else if (strncmp((const char *)uip_appdata, "GET /parameter/page_2.html", strlen("GET /parameter/page_2.html")) == 0)
						{
							#ifdef __ENABLE_DEBUG_PRINTF_
							printf("GET /parameter/page_2.html \n");
							#endif
							// const char *JSON_PARAMETER_SETTING_PAGE_2 = "{\"defect_boundary\":\"%d\",\"abort_time\":\"%d\",\"start_freq\":\"%d\"}";
							sprintf(fetch_json_buf, JSON_PARAMETER_SETTING_PAGE_2, 
									0, read_timeout_set_display(),
									0);
							transfer_len = sprintf(http_output_buf, "%s%x\r\n%s\r\n0\r\n\r\n", http_header_array[RESPONSE_HEADER_TYPE_JSON], strlen(fetch_json_buf), fetch_json_buf);
							transfer_ptr = (u8_t *)http_output_buf;	
							transfer_header_flag = 0;					
						}	
						else if (strncmp((const char *)uip_appdata, "GET /parameter/sec_vib.html", strlen("GET /parameter/sec_vib.html")) == 0)
						{
							#ifdef __ENABLE_DEBUG_PRINTF_
							printf("GET /parameter/sec_vib.html \n");
							#endif
							unsigned int stage2_mode_value_tmp = 0;
							switch(read_stage2_mode_address_display()) {
								case 3:
									stage2_mode_value_tmp = read_time_set_stage2_display();
									break;
								case 4:
									stage2_mode_value_tmp = read_distance_set_stage2_display();
									break;
								case 6:
									stage2_mode_value_tmp = read_energy_set_stage2_display();
									break;
								default:									
									break;
							}
														
							// const char *JSON_PARAMETER_SETTING_PAGE_SEC_VIB = "{\"sec_vib_mode\":\"%d\",\"sec_vib_value\":\"%d\",\"vib_a\":\"%d\",\"vib_b\":\"%d\"}";
							sprintf(fetch_json_buf, JSON_PARAMETER_SETTING_PAGE_SEC_VIB, 
									read_stage2_mode_address_display(), stage2_mode_value_tmp,
									read_amplitudeA_set_stage2_display(), read_amplitudeB_set_stage2_display());
							transfer_len = sprintf(http_output_buf, "%s%x\r\n%s\r\n0\r\n\r\n", http_header_array[RESPONSE_HEADER_TYPE_JSON], strlen(fetch_json_buf), fetch_json_buf);
							transfer_ptr = (u8_t *)http_output_buf;	
							transfer_header_flag = 0;					
						}	
						else if (strncmp((const char *)uip_appdata, "GET /parameter/post_vib.html", strlen("GET /parameter/post_vib.html")) == 0)
						{
							unsigned int post_vib_mode_value_tmp = 0;
							//MODE 0 OFF
							//MODE 4 TIME
							//MODE 5 DISTANCE
							if (read_mode_after_stage_display() == 0)
							{
								//write_mode_after_stage_set(4);
								//post_vib_mode_value_tmp = read_time_after_trigger_display();
							}
							else if (read_mode_after_stage_display() == 4)
							{
								post_vib_mode_value_tmp = read_time_after_trigger_display();
							}
							else if (read_mode_after_stage_display() == 5)
							{
								post_vib_mode_value_tmp = read_distance_after_trigger_display();
							}
							
							// const char *JSON_PARAMETER_SETTING_PAGE_POST_VIB = "{\"post_vib_mode\":\"%d\",\"post_vib_value\":\"%d\",\"vib_amp\":\"%d\",\"vib_time\":\"%d\"}";
							sprintf(fetch_json_buf, JSON_PARAMETER_SETTING_PAGE_POST_VIB, 
									read_mode_after_stage_display(), post_vib_mode_value_tmp,
									read_power_after_stage_display(), read_time_on_after_stage_display());
							transfer_len = sprintf(http_output_buf, "%s%x\r\n%s\r\n0\r\n\r\n", http_header_array[RESPONSE_HEADER_TYPE_JSON], strlen(fetch_json_buf), fetch_json_buf);
							transfer_ptr = (u8_t *)http_output_buf;	
							transfer_header_flag = 0;	
						}
						else if (strncmp((const char *)uip_appdata, "GET /parameter/pre_vib.html", strlen("GET /parameter/pre_vib.html")) == 0)
						{
							unsigned int pre_vib_mode_value_tmp = 0;
							//MODE 0 OFF
							//MODE 1 TIME
							//MODE 2 DISTANCE
							if (read_mode_early_stage_display() == 0)
								pre_vib_mode_value_tmp = 0;
							else if (read_mode_early_stage_display() == 1)
								pre_vib_mode_value_tmp = read_time_early_trigger_display();
							else if (read_mode_early_stage_display() == 2)
								pre_vib_mode_value_tmp = read_distance_early_trigger_display();
							
							// const char *JSON_PARAMETER_SETTING_PAGE_PRE_VIB = "{\"mode_select\":\"%d\",\"mode_value\":\"%d\",\"vib_amp\":\"%d\"}";
							sprintf(fetch_json_buf, JSON_PARAMETER_SETTING_PAGE_PRE_VIB, 
									read_mode_early_stage_display(), pre_vib_mode_value_tmp, read_power_early_stage_display());
							transfer_len = sprintf(http_output_buf, "%s%x\r\n%s\r\n0\r\n\r\n", http_header_array[RESPONSE_HEADER_TYPE_JSON], strlen(fetch_json_buf), fetch_json_buf);
							transfer_ptr = (u8_t *)http_output_buf;	
							transfer_header_flag = 0;	
						}
						else if (strncmp((const char *)uip_appdata, "GET /data/welding_record.html", strlen("GET /data/welding_record.html")) == 0)
						{							
							char *tmp_ptr = http_table_buf;
							//int array_row = sizeof(pseudo_welding_record)/(15*sizeof(char *));
							//int array_row = sizeof(history_record_array)/sizeof(history_record_array[0]);
							int array_row = 10;
							int sprintf_len = 0;
							int total_sprintf_len = 0;
							
							sprintf_len = sprintf(tmp_ptr, "[");
							tmp_ptr += sprintf_len;
							total_sprintf_len += sprintf_len;
							for (i = 0; i < (array_row); i++)
							{								
								sprintf_len = sprintf(tmp_ptr, "[");
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								/*for (j = 0; j < 15; j++)
								{							
									if (j < 14)
										sprintf_len = sprintf(tmp_ptr, "\"%s\",", pseudo_welding_record[i][j]);
									else
										sprintf_len = sprintf(tmp_ptr, "\"%s\"", pseudo_welding_record[i][j]);
									tmp_ptr += sprintf_len;
									total_sprintf_len += sprintf_len;
								}*/
								if (i <= history_point_set)
								{
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_freq_delta_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_freq_start_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_freq_end_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_freq_max_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_freq_min_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_P_max_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_avg_power_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_energy_display_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_time_on_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_F_set_history_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_F_start_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_F_max_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_encoder_speed_history_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_distance_reached_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_absolute_hold_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_distance_travelled_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\",", read_collapse_hold_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								sprintf_len = sprintf(tmp_ptr, "\"%d\"", read_total_time_display_html(i));
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
								}
		
								if (i < (array_row - 1))
									sprintf_len = sprintf(tmp_ptr, "],");
								else
									sprintf_len = sprintf(tmp_ptr, "]");
								tmp_ptr += sprintf_len;
								total_sprintf_len += sprintf_len;
							}
							sprintf_len = sprintf(tmp_ptr, "]");
							tmp_ptr += sprintf_len;
							total_sprintf_len += sprintf_len;
							
							transfer_len = sprintf((char *)http_output_buf, "%s%x\r\n%s\r\n0\r\n\r\n", http_header_array[RESPONSE_HEADER_TYPE_JSON], strlen(http_table_buf), http_table_buf);
							transfer_ptr = (u8_t *)http_output_buf;
							
							transfer_header_flag = 0;							
						}
						else  // by default, return 200 OK 
						{
							#ifdef __ENABLE_DEBUG_PRINTF_
							printf("Unknown GET method %s \n", uip_appdata);
							#endif
							transfer_header_flag = 1;
						}
					}
					else  // if (flag_pass_authentication == 1)
					{
						transfer_header_flag = 1;
						transfer_header_type = RESPONSE_HEADER_TYPE_404;	
						#ifdef __ENABLE_DEBUG_PRINTF_
						printf("RESPONSE_HEADER_TYPE_404: case 1 \n");
						#endif						
					}							
				}  // end of general GET method				
			}  
        }
		else	
		if ((uip_len >= 4) && (strncmp((const char *)uip_appdata, "POST", 4) == 0))
        {			
			#ifdef __SUPPORT_LOGIN_PAGE_	
			if (strncmp((const char *)uip_appdata, "POST /authentication.html", strlen("POST /authentication.html")) == 0) 
			{
				#ifdef __ENABLE_DEBUG_PRINTF_
				printf("PPOST /authentication.html \n");
				#endif
				if (bIsContinuationPacket((const char*)uip_appdata, uip_len) == 1)
				{
					#ifdef __ENABLE_DEBUG_PRINTF_
					printf("POST by two packets \n");
					#endif
					flag_post_continuation = 1;
					flag_post_to_login_continuation = 1;					
				}
				else 
				{
					flag_check_password = 1;
				}	
			}				
			#endif
			
			if (flag_pass_authentication == 1)  // start of general POST method
			{
				if (strncmp((const char *)uip_appdata, "POST /parameter/", strlen("POST /parameter/")) == 0) 
				{				
					if (bIsContinuationPacket((const char*)uip_appdata, uip_len) == 1)
					{
						#ifdef __ENABLE_DEBUG_PRINTF_
						printf("POST by two packets \n");
						#endif
						flag_post_continuation = 1;
					}
					else
					{
						flag_post_continuation = 0;
						// no need to clear memory json_key and json_value since one httpd_appcall entrance parses json once						
						if (parsing_json((const char *)uip_appdata, uip_len, json_key, json_value) == 1)
						{
							flag_post_appdata_found_json = 1;
						}	
						else
						{
							flag_post_appdata_found_json = 0;
						}
					}
										
					if (strncmp((const char *)uip_appdata, "POST /parameter/page_1.html", strlen("POST /parameter/page_1.html")) == 0) 
					{
						post_to_modify_parameter = 1;
					}
					else if (strncmp((const char *)uip_appdata, "POST /parameter/page_2.html", strlen("POST /parameter/page_2.html")) == 0) 
					{
						post_to_modify_parameter = 2;
					}
					else if (strncmp((const char *)uip_appdata, "POST /parameter/sec_vib.html", strlen("POST /parameter/sec_vib.html")) == 0) 
					{
						post_to_modify_parameter = 3;
					}
					else if (strncmp((const char *)uip_appdata, "POST /parameter/post_vib.html", strlen("POST /parameter/post_vib.html")) == 0) 
					{
						post_to_modify_parameter = 4;
					}
					else if (strncmp((const char *)uip_appdata, "POST /parameter/pre_vib.html", strlen("POST /parameter/pre_vib.html")) == 0) 
					{
						post_to_modify_parameter = 5;
					}
					else
					{
						post_to_modify_parameter = 0;					
					}							
				}
				else if (strncmp((const char *)uip_appdata, "POST /fpga/save_parameter.html", strlen("POST /fpga/save_parameter.html")) == 0) 
				{
					//
					// to do: transfer paramter to FPGA
					// 
						
					transfer_header_flag = 1;
				}				
			}  // end of general POST method
			else  // if (flag_pass_authentication == 1)	
			{
				#ifdef __ENABLE_DEBUG_PRINTF_
				printf("POST for NULL \n");
				#endif
				transfer_header_flag = 1;
			}
		
		}
		else if (flag_post_continuation == 1)
		{
			#ifdef __ENABLE_DEBUG_PRINTF_
			printf("2nd POST packet \n");
			#endif			
			flag_post_continuation = 0;
			if (flag_post_to_login_continuation == 1)
			{
				flag_post_to_login_continuation = 0;
				flag_check_password = 1;
			}
			else if (post_to_modify_parameter > 0)
			{
				// no need to clear memory json_key and json_value since one httpd_appcall entrance parses json once
				if (parsing_json((const char *)uip_appdata, uip_len, json_key, json_value) == 1)
				{
					flag_post_appdata_found_json = 1;
					#ifdef __ENABLE_DEBUG_PRINTF_
					printf("post_to_modify_parameter: found json_key  %s \n", json_key);
					#endif	
				}
				else
				{
					flag_post_appdata_found_json = 0;
					#ifdef __ENABLE_DEBUG_PRINTF_
					printf("post_to_modify_parameter: can NOT found json_key \n");
					#endif	
				}
			}
			else
			{
				#ifdef __ENABLE_DEBUG_PRINTF_
				printf("unknown POST 2nd packet \n");
				#endif
				transfer_len = 0;
			}
		}
		else
		{
			#ifdef __ENABLE_DEBUG_PRINTF_
			printf("exceptional request %s \n", uip_appdata);
			#endif
			transfer_len = 0;
		}
			
		if ((flag_post_continuation == 0) && (flag_post_to_login_continuation == 0))
		{
			if (flag_check_password)
			{
				flag_check_password = 0;
				if (bVerifyPassword((const char *)uip_appdata, uip_len))			
				{
					#ifdef __ENABLE_DEBUG_PRINTF_
					printf("login successfully \n");
					#endif
					flag_pass_authentication = 1;
					transfer_header_flag = 1;  // python script needs header to judge status
					
					transfer_ptr = (u8_t *)main_page_1_hex + 2;
					transfer_len = sizeof(main_page_1_hex) - 2;					
					transfer_header_type = RESPONSE_HEADER_TYPE_GZIP;	
				}
				else
				{
					transfer_ptr = (u8_t *)login_hex + 2;
					transfer_len = sizeof(login_hex) - 2;
					transfer_header_flag = 1;
					transfer_header_type = RESPONSE_HEADER_TYPE_GZIP_404;
				}
			}
		}
			
		if ((post_to_modify_parameter > 0) && (flag_post_continuation == 0) && (flag_post_appdata_found_json == 1))
		{			
			if (post_to_modify_parameter == 1) 
			{	
				update_parameter_setting_page_1(json_key, json_value);
			}
			else if (post_to_modify_parameter == 2) 
			{
				update_parameter_setting_page_2(json_key, json_value);
			}
			else if (post_to_modify_parameter == 3) 
			{
				update_parameter_setting_page_seg_vib(json_key, json_value);
			}
			else if (post_to_modify_parameter == 4) 
			{			
				update_parameter_setting_page_post_vib(json_key, json_value);
			}
			else if (post_to_modify_parameter == 5) 
			{			
				update_parameter_setting_page_pre_vib(json_key, json_value);
			}
			else
			{
				transfer_header_type = RESPONSE_HEADER_TYPE_404;
			}		

			transfer_header_flag = 1;
			post_to_modify_parameter = 0;
			flag_post_appdata_found_json = 0;			
		}
		
		if ((transfer_header_flag == 1) && (flag_post_continuation == 0))  // if there was next post packet, do not send response header
		{
			#ifdef __ENABLE_DEBUG_PRINTF_
			printf("RESPONSE HEADER TYPE %d \n", transfer_header_type);
			#endif
			uip_send((volatile u8_t *)http_header_array[transfer_header_type], strlen(http_header_array[transfer_header_type]));				
			transfer_header_flag = 0;
			if (transfer_len > 0)
			{
				hs->connect_flag = 3;
			}
			else
			{
				hs->connect_flag = 2;
			}
		}
		else if (transfer_len > 0)
		{
			hs->connect_flag = send_response();					
		}
    }
	else if (uip_acked() || uip_poll())  // merge uip_acked() and uip_pool() flow to reduce code size
	{	
		if (uip_poll())
		{			
			if (hs->connect_flag == 2)
			{
				uip_close();
				hs->connect_flag = 0;
				hs->time_out = 0;
			}
			else if (hs->connect_flag == 1)
			{
				if ((++hs->time_out) > 10)
				{
					hs->connect_flag = 2;
				}			
			}
		}
	
		if (hs->connect_flag == 3) 
		{			
            if (transfer_len > 0)
			{
				hs->connect_flag = send_response();				
			}
		}
		
		if (flag_chunked_transfer == 1)
		{
			flag_chunked_transfer = 0;
			char *tmp_ptr = http_table_buf;							
			int sprintf_len = 0;
			int total_sprintf_len = 0;
			for (int i = 100; i > 0; i--)
			{
				if (i > 1)
					sprintf_len = sprintf(tmp_ptr, "\"%d\",", i);
				else
					sprintf_len = sprintf(tmp_ptr, "\"%d\"", i);
				tmp_ptr += sprintf_len;
				total_sprintf_len += sprintf_len;
			}						
							
			sprintf_len = sprintf(tmp_ptr, "] }");
			tmp_ptr += sprintf_len;
			total_sprintf_len += sprintf_len;
									
			transfer_len = sprintf((char *)http_output_buf, "%s%x\r\n%s\r\n0\r\n\r\n", http_header_array[RESPONSE_HEADER_TYPE_JSON], strlen(http_table_buf), http_table_buf);
			transfer_ptr = (u8_t *)http_output_buf;
			hs->connect_flag = send_response();	
		}
	}
    else if (uip_closed())
    {
		#ifdef __ENABLE_DEBUG_PRINTF_
        printf("http server closed !\n");
		#endif
    }
	else
	{
		#ifdef __ENABLE_DEBUG_PRINTF_
        printf("http unknown status !\n");
		#endif
	}
}

char send_response(void)
{
	if (transfer_len > MAX_HTML_SCRIPT_PER_PACKET)
	{				
		uip_send((volatile u8_t *)transfer_ptr, MAX_HTML_SCRIPT_PER_PACKET);			
		transfer_ptr += MAX_HTML_SCRIPT_PER_PACKET;
		transfer_len = transfer_len - MAX_HTML_SCRIPT_PER_PACKET;
		return 3;
	}
	else
	{
		uip_send((volatile u8_t *)transfer_ptr, transfer_len);	
		transfer_len = 0;
		return 2;
	}	
}

#ifdef __SUPPORT_LOGIN_PAGE_
// assume password length is 8
// assume password exists in the last 8 bytes of postData
char bVerifyPassword(const char *packet_data, int packet_len)
{
	int i = 0;
	char client_password[9];
	unsigned char decoded_password[20];
	
	#ifdef __ENCODED_PASSWORD_	
	char json_key[JSON_KEY_BUFFER_SIZE] = {0};
	char json_value[JSON_VALUE_BUFFER_SIZE] = {0};
	if (parsing_json(packet_data, packet_len, json_key, json_value))
	{					
		if (strncmp((const char *)json_key, (const char *)LOGIN_KEY, strlen(LOGIN_KEY)) == 0)
		{
			base64_decode(json_value, strlen(json_value), decoded_password, 8);
			if (strncmp((const char *)decoded_password, (const char *)LOGIN_PASSWORD, strlen(LOGIN_PASSWORD)) == 0)
			{
				return 1;
			}
		}
	}

	return 0;	
	
	#else			
	for (i = 0; i < 8; i++)
	{
		client_password[i] = postData[(postDataLen - 8) + i];
	}
	client_password[8] = '\0';
				
	if (strncmp((const char *)client_password, (const char *)LOGIN_PASSWORD, strlen(LOGIN_PASSWORD)) == 0)
	{	
		return 1;
	}
	else
	{
		return 0;
	}
	#endif	
}
#endif

char parsing_json(const char *post_data, int data_len, char *json_key, char *json_value)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int json_state = 0;
	int return_val = 0;
	
	// json_state:
	// 0: search for '{'
	// 1: found '{', then look for left '\"' of key
	// 2: found left '\"' of key, look for first character of key
	// 3: collect key character
	// 4: look for ':' between key and value
	// 5: found ':', then look for left '\"' of value
	// 6: found left '\"' of value, look for first character of value
	// 7: collect value character
	// 8: found json, ending

	for (i = 0; i < data_len; i++)
	{
		if ((post_data[i] == '\r') || (post_data[i] == '\n'))
		{
			json_state = 0;
		}
		else if (post_data[i] == '{')
		{
			if (json_state == 0)
			{
				json_state = 1;
			}
			else
			{
				json_state = 0;
			}
		}
		else if (post_data[i] == '\"')
		{
			if (json_state == 0)
			{
				;
			}
			else if (json_state == 1)
			{
				json_state = 2;
			}
			else if (json_state == 2)
			{
				// NULL string
				json_state = 0;
			}
			else if (json_state == 3)
			{
				json_state = 4;  // look for a ':'
			}
			else if (json_state == 5)
			{
				json_state = 6;  // to gether json_value[]
			}
			else if (json_state == 7)
			{
				json_state = 8;  // end
				return_val = 1;	
				break;
			}
			else
			{
				json_state = 0;				
			}
		}
		else if (((post_data[i] >= '0') && (post_data[i] <= 'z') && (post_data[i] != ':')))  // to accept more character for encoded password
		{
			if (json_state == 0)
			{
				;
			}
			else if (json_state == 2)
			{
				json_state = 3;
				json_key[j] = post_data[i];
				j++;
			}
			else if (json_state == 3)
			{
				json_key[j] = post_data[i];
				j++;
			}		
            else if (json_state == 6)
			{
				json_state = 7;
				json_value[k] = post_data[i];
				k++;
			}
			else if (json_state == 7)
			{
				json_value[k] = post_data[i];
				k++;
			}
            else if ((json_state == 1) || (json_state == 5))
			{		
				json_state = 0;
			}						
		}
		else if (post_data[i] == ':')
		{
			if (json_state == 0)
			{
				;
			}
			else if (json_state == 4)
			{
				json_state = 5;
			}
			else
			{
				;
			}
		}
		else
		{
			if (json_state > 0)
			{
				if (post_data[i] != ' ')
				{
					;
				}
			}
		}
	}		
	
	return return_val;
}

char parseURL(char *html_filename)
{
	int i = strlen("GET /html/");
							
	for (int j = 0; j < 64; j++)  // assume max URL length is 64
	{								
		if (uip_appdata[i+j] == 0x2E)  // found '.' and then get extension filename
		{
			if ((uip_appdata[i+j+1] == 'h') && (uip_appdata[i+j+2] == 't') && (uip_appdata[i+j+3] == 'm') && (uip_appdata[i+j+4] == 'l'))
			{
			memcpy(&(html_filename[j+1]), (const void *)&(uip_appdata[i+j+1]), 4);  // get extension filename, expection is "html"
			#ifdef __ENABLE_DEBUG_PRINTF_
			printf("GET html: %s \n", html_filename);
			#endif
			return 1;
			}
			else
			{
				break;
			}
		}
		else
		{
			html_filename[j] = uip_appdata[i+j];
		}
	}
	
	return 0;
}

void initParameter(void)
{
#if 0	
	parameter_page_1_value_array[0] = read_mode_set();  // mode_select = 1, means time
	switch (parameter_page_1_value_array[0]) {
		case 2:
			parameter_page_1_value_array[1] = read_distance_relative_set_display();
			break;
		case 3:
			parameter_page_1_value_array[1] = read_distance_absolute_set_display();
			break;
		case 5:
			parameter_page_1_value_array[1] = read_energy_set_display();
			break;
		default:
			parameter_page_1_value_array[1] = read_time_set_stage_one_display();
			break;
	}
	
	parameter_page_1_value_array[2] = 1;  // pre_vib = 1, means enable
	parameter_page_1_value_array[3] = 1;  // post_vib = 1, means enable
	parameter_page_1_value_array[4] = 10;  // pressure_time
	parameter_page_1_value_array[5] = read_pressure_display();  // trigger_pressure
	parameter_page_1_value_array[6] = 0;  // vib_mode = 0, means fixed mode
	parameter_page_1_value_array[7] = read_amplitude_set_display();  // vib_value
	
	parameter_page_2_value_array[0] = 1;  // defect_boundary = 1, means enable
	parameter_page_2_value_array[1] = read_timeout_set_display();  // abort_time
	parameter_page_2_value_array[2] = read_freq_display();  // start_freq
	
	parameter_page_sec_vib_value_array[0] = 1;  // sec_vib_mode = 1, means time
	parameter_page_sec_vib_value_array[1] = 1;  // sec_vib_value = 1
	parameter_page_sec_vib_value_array[2] = 10;  // vib_a = 10, means 10%
	parameter_page_sec_vib_value_array[3] = 10;  // vib_b = 10, means 10%
#endif	
}

//char *parameter_page_1_key_array[] = {
//	"mode_select",
//	"mode_value",
//	"pre_vib",
//	"post_vib",
//	"pressure_time",
//	"trigger_pressure",
//	"vib_mode",
//	"vib_amp"
//};
void update_parameter_setting_page_1(char *json_key, char *json_value)
{	
	if (strncmp((const char *)json_key, "mode_select", strlen("mode_select")) == 0)				
	{
		//MODE 1 TIME (default)
		//MODE 2 DISTANCE COLLAPSE/RELATIVE
		//MODE 3 DOSTANCE ABSOLUTE
		//MODE 5 ENERGY
		switch (atoi(json_value)) {
		case 2:
			write_mode_set(2);
			break;
		case 3:
			write_mode_set(3);
			break;
		case 5:
			write_mode_set(5);
			break;
		default:
			write_mode_set(1);
			break;
		}				
	}
	else if (strncmp((const char *)json_key, "mode_value", strlen("mode_value")) == 0)				
	{
		switch (read_mode_set()) {
		case 1:
			write_time_set_stage_one_set(atoi(json_value));
			break;
		case 2:
			write_distance_relative_set(atoi(json_value));
			break;
		case 3:
			write_distance_absolute_set(atoi(json_value));
			break;
		case 5:
			write_energy_set(atoi(json_value));
			break;
		default:
			break;
		}				
	}
	//else if (strncmp((const char *)json_key, "sec_vib_mode", strlen("sec_vib_mode")) == 0)	
	else if (strncmp((const char *)json_key, "vib_mode", strlen("vib_mode")) == 0)	
	{
		write_stage2_mode_address_set(atoi(json_value));
	}
	//else if (strncmp((const char *)json_key, "sec_vib_value", strlen("sec_vib_value")) == 0)	
	else if (strncmp((const char *)json_key, "vib_amp", strlen("vib_amp")) == 0)	
	{
		write_amplitude_set(atoi(json_value));
	}
	else if (strncmp((const char *)json_key, "trigger_pressure", strlen("trigger_pressure")) == 0)	
	{
		write_force_set(atoi(json_value));
	}
	else if (strncmp((const char *)json_key, "pressure_time", strlen("pressure_time")) == 0)				
	{
		write_hold_time_set(atoi(json_value));
	}
}

//char *parameter_page_2_key_array[] = {
//	"defect_boundary",
//	"abort_time",	
//	"start_freq"
//};
void update_parameter_setting_page_2(char *json_key, char *json_value)
{
	if (strncmp((const char *)json_key, "abort_time", strlen("abort_time")) == 0)	
	{
		write_timeout_set(atoi(json_value));
	}
}

//char *parameter_page_sec_vib_key_array[] = {
//	"sec_vib_mode",
//	"sec_vib_value",
//	"vib_a",
//	"vib_b"
//};
void update_parameter_setting_page_seg_vib(char *json_key, char *json_value)
{
	if (strncmp((const char *)json_key, "sec_vib_mode", strlen("sec_vib_mode")) == 0)	
	{
		int kk = atoi(json_value); 
		if (kk == 3)		
		{
			write_stage2_mode_address_set(3);			
		}
		else if (kk == 4)
		{
			write_stage2_mode_address_set(4);
		}
		else if (kk == 6)
		{
			write_stage2_mode_address_set(6);
		}
	}
	else if (strncmp((const char *)json_key, "sec_vib_value", strlen("sec_vib_value")) == 0)	
	{
		if (read_stage2_mode_address_display() == 3)
		{
			write_time_set_stage2(atoi(json_value));
		}
		else if (read_stage2_mode_address_display() == 4)
		{
			write_distance_set_stage2(atoi(json_value));
		}
		else if (read_stage2_mode_address_display() == 6)
		{
			write_energy_set_stage2(atoi(json_value));
		}
	}
	else if (strncmp((const char *)json_key, "vib_a", strlen("vib_a")) == 0)	
	{
		write_amplitudeA_set_stage2(atoi(json_value));
	}
	else if (strncmp((const char *)json_key, "vib_b", strlen("vib_b")) == 0)	
	{
		write_amplitudeB_set_stage2(atoi(json_value));
	}	
}

//char *parameter_page_sec_vib_key_array[] = {
//	"post_vib_mode",
//	"post_vib_value",
//	"post_vib_amp",
//	"post_vib_time"
//};
void update_parameter_setting_page_post_vib(char *json_key, char *json_value)
{
	if (strncmp((const char *)json_key, "post_vib_mode", strlen("post_vib_mode")) == 0)	
	{
		write_mode_after_stage_set(atoi(json_value));
	}
	else if (strncmp((const char *)json_key, "post_vib_value", strlen("post_vib_value")) == 0)	
	{
		if (read_mode_after_stage_display() == 4)
		{			
			write_time_after_trigger_set(atoi(json_value));
		}
		else if (read_mode_after_stage_display() == 5)
		{
			write_distance_after_trigger_set(atoi(json_value));
		}
	}
	else if (strncmp((const char *)json_key, "post_vib_amp", strlen("post_vib_amp")) == 0)	
	{
		write_power_after_stage_set(atoi(json_value));
	}
	else if (strncmp((const char *)json_key, "post_vib_time", strlen("post_vib_time")) == 0)	
	{
		write_time_on_after_stage_set(atoi(json_value));
	}
}

//char *parameter_page_sec_vib_key_array[] = {
//	"pre_vib_mode",
//	"pre_vib_value",
//	"pre_vib_amp"
//};
void update_parameter_setting_page_pre_vib(char *json_key, char *json_value)
{
	if (strncmp((const char *)json_key, "pre_vib_mode", strlen("pre_vib_mode")) == 0)	
	{
		write_mode_early_stage_set(atoi(json_value));		
	}
	else if (strncmp((const char *)json_key, "pre_vib_value", strlen("pre_vib_value")) == 0)	
	{
		if (read_mode_early_stage_display() == 1)
		{			
			write_time_early_trigger_set(atoi(json_value));
		}
		else if (read_mode_early_stage_display() == 2)
		{
			write_distance_early_trigger_set(atoi(json_value));
		}
	}
	else if (strncmp((const char *)json_key, "pre_vib_amp", strlen("pre_vib_amp")) == 0)	
	{
		write_power_early_stage_set(atoi(json_value));
	}
}

char bIsContinuationPacket(const char *packet_data, int packet_len)
{
	if ((packet_data[packet_len - 2] == 0x0d) && (packet_data[packet_len - 1] == 0x0a))
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

void pack_css_data(u8_t *css_hex_ptr, int css_hex_len)
{
	int i = 0;
	int header_len = sprintf(http_output_buf, "%s", http_header_array[RESPONSE_HEADER_TYPE_CSS]);
	for (i = 0; i < css_hex_len; i++)
	{
		http_output_buf[header_len + i] = css_hex_ptr[i];
	}
	
}

#ifdef __ENCODED_PASSWORD_	
static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char decoding_table[256] = { 0 };
static int mod_table[] = {0, 2, 1};
	
void build_decoding_table() {
    for (int i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}

void base64_decode(const char *data, unsigned char input_length,
                   unsigned char *decoded_data, unsigned char output_length) 
{
	//printf("base64_decode(): data %s, input_length %d, output_length %d \n", data, input_length, output_length);
	
    build_decoding_table();    
    
    //if (data[input_length - 1] == '=') output_length--;
    //if (data[input_length - 2] == '=') output_length--;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);

        if (j < output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }
	//printf("base64_decode(): decoded_data %s  output_length %d \n", decoded_data, output_length);
}
#endif  // #ifdef __ENCODED_PASSWORD_

/**
  * @brief  EMAC Tx interrupt handler.
  * @param  None
  * @return None
  */
void EMAC_TX_IRQHandler(void)
{
    // Clean up Tx resource occupied by previous sent packet(s)
    EMAC_SendPktDone();
}

/**
  * @brief  EMAC Rx interrupt handler.
  * @param  None
  * @return None
  */
void EMAC_RX_IRQHandler(void)
{
    while (1)
    {
        // Get all recv data
        if (EMAC_RecvPkt(rxbuf, (uint32_t *)&u32PktLen) == 0)
            break;

        // Clean up Rx resource occupied by previous received packet
        EMAC_RecvPktDone();
    }
}

void webserver_init(void)
{
	uip_ipaddr_t    ipaddr;
	
	/* Unlock protected registers */
    SYS_UnlockReg();
	
    CLK_EnableModuleClock(EMAC_MODULE);
    // Configure MDC clock rate to HCLK / (127 + 1) = 1.5 MHz if system is running at 192 MHz
    CLK_SetModuleClock(EMAC_MODULE, 0, CLK_CLKDIV3_EMAC(127));
	
	SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB1MFP_Msk | SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk | SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk | SYS_GPB_MFPL_PB7MFP_Msk);	
	SYS->GPB_MFPL |= SYS_GPB_MFPL_PB1MFP_EMAC_RMII_RXERR |
	                 SYS_GPB_MFPL_PB2MFP_EMAC_RMII_CRSDV |
                     SYS_GPB_MFPL_PB3MFP_EMAC_RMII_RXD1 |
					 SYS_GPB_MFPL_PB4MFP_EMAC_RMII_RXD0 |
                     SYS_GPB_MFPL_PB5MFP_EMAC_RMII_REFCLK |
                     SYS_GPB_MFPL_PB7MFP_EMAC_RMII_TXEN;
	SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB8MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk | SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB11MFP_Msk);	
    SYS->GPB_MFPH |= SYS_GPB_MFPH_PB11MFP_EMAC_RMII_MDC |
                     SYS_GPB_MFPH_PB10MFP_EMAC_RMII_MDIO |
                     SYS_GPB_MFPH_PB9MFP_EMAC_RMII_TXD0 |
                     SYS_GPB_MFPH_PB8MFP_EMAC_RMII_TXD1;
					 
    // Enable high slew rate on all RMII TX output pins
    //PB->SLEWCTL = (GPIO_SLEWCTL_HIGH << GPIO_SLEWCTL_HSREN7_Pos) |
    //              (GPIO_SLEWCTL_HIGH << GPIO_SLEWCTL_HSREN8_Pos) |
    //              (GPIO_SLEWCTL_HIGH << GPIO_SLEWCTL_HSREN9_Pos);
	
	/* Lock protected registers */
    SYS_LockReg();
	
	// Select RMII interface by default
    EMAC_Open(ethaddr.addr);
	EMAC_PhyInit();
    NVIC_EnableIRQ(EMAC_TX_IRQn);
    NVIC_EnableIRQ(EMAC_RX_IRQn);
    EMAC_ENABLE_RX();
    EMAC_ENABLE_TX();  // to call EMAC_PhyInit()
	
	/* set IP address local host */
    uip_ipaddr(ipaddr, 192, 168, 0, 2);
    uip_sethostaddr(ipaddr);
    /* set  IP address of default router */
    uip_ipaddr(ipaddr, 192, 168, 0, 100);
    uip_setdraddr(ipaddr);
    /* set net mask */
    uip_ipaddr(ipaddr, 255, 255, 255, 0);
    uip_setnetmask(ipaddr);

    uip_setethaddr(ethaddr);
    /* initalize Http server and LEDs */
	initParameter();
    httpd_init();	
}

void uip_log(char *m)
{
	#ifdef __ENABLE_DEBUG_PRINTF_
    printf("uIP log message: %s\n", m);
	#endif
}

uint32_t uip_write(void)
{
	#ifdef __TEST_MMSL_BLE_PCB_
	//printf("uip_write() %d: %x %x\n", uip_len, uip_buf[0], uip_buf[1]);
	#endif	
	
    memcpy(txbuf, uip_buf, 40 + UIP_LLH_LEN);

    if (uip_len > (40 + UIP_LLH_LEN))
        memcpy(&txbuf[40 + UIP_LLH_LEN], (const void *)uip_appdata, uip_len - 40 - UIP_LLH_LEN);

    return EMAC_SendPkt(txbuf, uip_len);
}

uint32_t uip_read(void)
{
    int len = 0;
	#ifdef __ENABLE_DEBUG_NETWORKING_
	printf("uip_read %d %d SysTick %d \n", curTime, prevTime, SysTick->VAL);
	#endif
    while ((curTime == prevTime) && u32PktLen == 0);

    if (u32PktLen != 0)
    {
        NVIC_DisableIRQ(EMAC_RX_IRQn);

        if (u32PktLen != 0)
        {
            memcpy(uip_buf, rxbuf, u32PktLen);
            len = u32PktLen;
            u32PktLen = 0;
        }

        NVIC_EnableIRQ(EMAC_RX_IRQn);
		prevTime = curTime;  // for test
    }
    else
    {
        prevTime++;
    }

    return len;
}

void webserver_loop(void)
{
	int             i, arptimer = 0;
	
	/* Let the network device driver read an entire IP packet
       into the uip_buf. If it must wait for more than 0.5 seconds, it
       will return with the return value 0. If so, we know that it is
       time to call upon the uip_periodic(). Otherwise, the EMAC has
       received an IP packet that is to be processed by uIP. */
    uip_len = uip_read();

    if (uip_len == 0)
    {
        for (i = 0; i < UIP_CONNS; i++)
        {
            uip_periodic(i);

            /* If the above function invocation resulted in data that
               should be sent out on the network, the global variable
               uip_len is set to a value > 0. */
            if (uip_len > 0)
            {
                uip_arp_out();
                uip_write();
            }
        }

#if UIP_UDP
        for (i = 0; i < UIP_UDP_CONNS; i++)
        {
            uip_udp_periodic(i);

            /* If the above function invocation resulted in data that
               should be sent out on the network, the global variable
               uip_len is set to a value > 0. */
            if (uip_len > 0)
            {
                uip_arp_out();
                uip_write();
            }
        }

#endif /* UIP_UDP */

        /* Call the ARP timer function every 10 seconds. */
        if (++arptimer == 20)
        {
				#ifdef __ENABLE_DEBUG_NETWORKING_
				printf("uip_arp_timer SysTick %d \n", SysTick->VAL);
				#endif
                uip_arp_timer();
                arptimer = 0;
        }
    }
    else
    {		
        if (BUF->type == htons(UIP_ETHTYPE_IP))
        {							
            uip_arp_ipin();
            uip_input();

            /* If the above function invocation resulted in data that
               should be sent out on the network, the global variable
               uip_len is set to a value > 0. */
            if (uip_len > 0)
            {
                uip_arp_out();
                uip_write();
            }
        }
        else if (BUF->type == htons(UIP_ETHTYPE_ARP))
        {
            uip_arp_arpin();

            /* If the above function invocation resulted in data that
               should be sent out on the network, the global variable
               uip_len is set to a value > 0. */
            if (uip_len > 0)
            {
                uip_write();
            }
        }
    }
}

