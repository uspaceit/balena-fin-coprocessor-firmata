#include "spyfly.h"
#include "balena.h"

/*************************************************
 * READ_BUTTON# - 3
 * PC6
 * PIN 8
 ************************************************/
#define GPIO_PORT_READ_BUTTON         gpioPortC
#define GPIO_PIN_READ_BUTTON          6

/*************************************************
 * Power CTRL - 3
 * PC7
 * PIN 10
 ************************************************/
#define GPIO_PORT_POWER               gpioPortC
#define GPIO_PIN_POWER                7

/*************************************************
 * UI_LED - 3
 * PC10
 * PIN 16
 ************************************************/
#define GPIO_PORT_UI_LED               gpioPortC
#define GPIO_PIN_UI_LED                10

uint32_t nowtime;
uint32_t prevtime_led;
uint32_t prevtime_button;
unsigned int button_prev_state;
unsigned int button_state;
bool ui_led_state;

void spyflyInit() {
  GPIO_PinModeSet(GPIO_PORT_POWER, GPIO_PIN_POWER, gpioModeWiredAndPullUp, 1);
  GPIO_PinModeSet(GPIO_PORT_READ_BUTTON, GPIO_PIN_READ_BUTTON, gpioModeInput, 1);
  GPIO_PinModeSet(GPIO_PORT_UI_LED, GPIO_PIN_UI_LED, gpioModeWiredAnd, 1);
  GPIO_PinOutSet(GPIO_PORT_POWER, GPIO_PIN_POWER);
  GPIO_PinOutSet(GPIO_PORT_UI_LED, GPIO_PIN_UI_LED);

  prevtime_led = RTCDRV_GetWallClock();
  prevtime_button = RTCDRV_GetWallClock();
  ui_led_state = false;
  button_prev_state = 0;
  button_state = 0;
}

void flicker(GPIO_Port_TypeDef port, unsigned int pin, int times, uint32_t ms) {
  for (int i = 0; i < times; i++) {
    GPIO_PinOutClear(port, pin);
    RTCDRV_Delay(ms);
    GPIO_PinOutSet(port, pin);
    RTCDRV_Delay(ms);
    GPIO_PinOutClear(port, pin);
  }
}

void spyflyRun() {
  nowtime = RTCDRV_GetWallClock();
  printf("########################\n");
  printf("Spyfly main running %d\n", nowtime);
  printf("GPIO_PORT_READ_BUTTON state: %d\n\n", digitalRead(6));

  if ((nowtime-prevtime_led) > 10) {
    if (ui_led_state) {
      GPIO_PinOutClear(GPIO_PORT_UI_LED, GPIO_PIN_UI_LED);
    } else {
      GPIO_PinOutSet(GPIO_PORT_UI_LED, GPIO_PIN_UI_LED);
    }
    prevtime_led = nowtime;
  }

  button_state = GPIO_PinInGet(GPIO_PORT_READ_BUTTON, GPIO_PIN_READ_BUTTON);
  if (button_state && button_prev_state) {
    if ((nowtime-prevtime_button) > 5) {
      // simulating shut down
      flicker(GPIO_PORT_UI_LED, GPIO_PIN_UI_LED, 5, 100);
    }
  } else {
    prevtime_button = nowtime;
  }
  button_prev_state = button_state;
}