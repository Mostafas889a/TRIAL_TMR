#include <firmware_apis.h>
#include "CF_TMR32.h"

#define TIMER1_BASE 0x30010000
#define TIMER1 ((CF_TMR32_TYPE_PTR)TIMER1_BASE)

static void send_pulse(int count);

void main(void) {
    ManagmentGpio_outputEnable();
    ManagmentGpio_write(0);
    enableHkSpi(0);

    GPIOs_configure(8, GPIO_MODE_USER_STD_OUTPUT);
    GPIOs_configure(9, GPIO_MODE_USER_STD_OUTPUT);
    GPIOs_configure(10, GPIO_MODE_USER_STD_INPUT_PULLUP);

    GPIOs_loadConfigs();
    User_enableIF();

    send_pulse(1);

    CF_TMR32_configureExamplePWM(TIMER1);
    CF_TMR32_PWM0Enable(TIMER1);
    CF_TMR32_PWM1Enable(TIMER1);
    CF_TMR32_enable(TIMER1);

    send_pulse(1);

    while(1);
}

static void send_pulse(int count) {
    for (int i = 0; i < count; i++) {
        ManagmentGpio_write(1);
        ManagmentGpio_write(0);
    }
}
