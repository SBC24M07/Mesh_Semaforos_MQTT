/* Mesh Internal Communication Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_mesh.h"
#include "mesh_traffic_lights.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

/*******************************************************
 *                Constants
 *******************************************************/
/* RGB configuration on ESP-WROVER-KIT board */
#define LED_PIN_1 GPIO_NUM_0 
#define LED_PIN_2 GPIO_NUM_2
#define LED_PIN_3 GPIO_NUM_4

/*******************************************************
 *                Variable Definitions
 *******************************************************/
static bool s_light_inited = false;
static uint8_t state = 0x00;

static const char *TAG = "mesh_traffic_lights";

/*******************************************************
 *                Function Definitions
 *******************************************************/
esp_err_t traffic_light_init(void)
{
    if (s_light_inited == true) {
        return ESP_OK;
    }
    s_light_inited = true;
    
    gpio_reset_pin(LED_PIN_1);
    gpio_reset_pin(LED_PIN_2);
    gpio_reset_pin(LED_PIN_3);

    gpio_set_direction(LED_PIN_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_PIN_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_PIN_3, GPIO_MODE_OUTPUT);

    traffic_light_set(TRAFFIC_LIGHT_INIT);
    return ESP_OK;
}

esp_err_t traffic_light_set(int color)
{
    switch (color) {
    case TRAFFIC_LIGHT_RED:
        /* Red */
        gpio_set_level(LED_PIN_1, 1);
        gpio_set_level(LED_PIN_2, 0);
        gpio_set_level(LED_PIN_3, 0);
        break;
    case TRAFFIC_LIGHT_YELLOW:
        /* Yellow */
        gpio_set_level(LED_PIN_1, 0);
        gpio_set_level(LED_PIN_2, 1);
        gpio_set_level(LED_PIN_3, 0);
        break;
    case TRAFFIC_LIGHT_GREEN:
        /* Green */
        gpio_set_level(LED_PIN_1, 0);
        gpio_set_level(LED_PIN_2, 0);
        gpio_set_level(LED_PIN_3, 1);
        break;
    case TRAFFIC_LIGHT_INIT:
        /* can't say */
        gpio_set_level(LED_PIN_1, 1);
        gpio_set_level(LED_PIN_2, 1);
        gpio_set_level(LED_PIN_3, 1);
        break;
    case TRAFFIC_LIGHT_WARNING:
        /* warning */
        gpio_set_level(LED_PIN_1, 1);
        gpio_set_level(LED_PIN_2, 1);
        gpio_set_level(LED_PIN_3, 0);
        break;
    default:
        /* off */
        gpio_set_level(LED_PIN_1, 0);
        gpio_set_level(LED_PIN_2, 0);
        gpio_set_level(LED_PIN_3, 0);
    }
	
	state = color;
	ESP_LOGI(TAG, "Semaforo establecido: %i", color);
    return ESP_OK;
}

void traffic_light_connected_indicator(int state)
{
    switch (state) {
	case 1:
        traffic_light_set(TRAFFIC_LIGHT_RED);
        break;
    case 2:
        traffic_light_set(TRAFFIC_LIGHT_YELLOW);
        break;
    case 3:
        traffic_light_set(TRAFFIC_LIGHT_GREEN);
        break;
    case 6:
        traffic_light_set(TRAFFIC_LIGHT_WARNING);
        break;
    default:
        traffic_light_set(0);
    }
}

void traffic_light_disconnected_indicator(void)
{
    traffic_light_set(TRAFFIC_LIGHT_WARNING);
}

esp_err_t traffic_light_process(mesh_addr_t *from, uint8_t *buf, uint16_t len)
{	
    mesh_traffic_light_ctl_t *in = (mesh_traffic_light_ctl_t *) buf;
    if (!from || !buf || len < sizeof(mesh_traffic_light_ctl_t)) {
        return ESP_FAIL;
    }
 
    
    if (in->cmd == CMD_TRAFFIC_LIGHT ) {
        if (in->set) {
			traffic_light_set(in->state);
        } else {
            traffic_light_set(0);
        }
    }
    
    return ESP_OK;
}
