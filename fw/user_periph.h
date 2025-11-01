/*
 * SPDX-FileCopyrightText: 2025 ChipFoundry
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _USER_PERIPH_H_
#define _USER_PERIPH_H_

#include <stdint.h>

#define TIMER0_BASE 0x30000000
#define TIMER1_BASE 0x30010000

#define TMR_OFFSET       0x0000
#define RELOAD_OFFSET    0x0004
#define PR_OFFSET        0x0008
#define CMPX_OFFSET      0x000C
#define CMPY_OFFSET      0x0010
#define CTRL_OFFSET      0x0014
#define CFG_OFFSET       0x0018
#define PWM0CFG_OFFSET   0x001C
#define PWM1CFG_OFFSET   0x0020
#define PWMDT_OFFSET     0x0024
#define PWMFC_OFFSET     0x0028
#define IM_OFFSET        0xFF00
#define MIS_OFFSET       0xFF04
#define RIS_OFFSET       0xFF08
#define IC_OFFSET        0xFF0C
#define GCLK_OFFSET      0xFF10

#define TMR0_TMR        (*(volatile uint32_t*)(TIMER0_BASE + TMR_OFFSET))
#define TMR0_RELOAD     (*(volatile uint32_t*)(TIMER0_BASE + RELOAD_OFFSET))
#define TMR0_PR         (*(volatile uint32_t*)(TIMER0_BASE + PR_OFFSET))
#define TMR0_CMPX       (*(volatile uint32_t*)(TIMER0_BASE + CMPX_OFFSET))
#define TMR0_CMPY       (*(volatile uint32_t*)(TIMER0_BASE + CMPY_OFFSET))
#define TMR0_CTRL       (*(volatile uint32_t*)(TIMER0_BASE + CTRL_OFFSET))
#define TMR0_CFG        (*(volatile uint32_t*)(TIMER0_BASE + CFG_OFFSET))
#define TMR0_PWM0CFG    (*(volatile uint32_t*)(TIMER0_BASE + PWM0CFG_OFFSET))
#define TMR0_PWM1CFG    (*(volatile uint32_t*)(TIMER0_BASE + PWM1CFG_OFFSET))
#define TMR0_PWMDT      (*(volatile uint32_t*)(TIMER0_BASE + PWMDT_OFFSET))
#define TMR0_PWMFC      (*(volatile uint32_t*)(TIMER0_BASE + PWMFC_OFFSET))
#define TMR0_IM         (*(volatile uint32_t*)(TIMER0_BASE + IM_OFFSET))
#define TMR0_MIS        (*(volatile uint32_t*)(TIMER0_BASE + MIS_OFFSET))
#define TMR0_RIS        (*(volatile uint32_t*)(TIMER0_BASE + RIS_OFFSET))
#define TMR0_IC         (*(volatile uint32_t*)(TIMER0_BASE + IC_OFFSET))
#define TMR0_GCLK       (*(volatile uint32_t*)(TIMER0_BASE + GCLK_OFFSET))

#define TMR1_TMR        (*(volatile uint32_t*)(TIMER1_BASE + TMR_OFFSET))
#define TMR1_RELOAD     (*(volatile uint32_t*)(TIMER1_BASE + RELOAD_OFFSET))
#define TMR1_PR         (*(volatile uint32_t*)(TIMER1_BASE + PR_OFFSET))
#define TMR1_CMPX       (*(volatile uint32_t*)(TIMER1_BASE + CMPX_OFFSET))
#define TMR1_CMPY       (*(volatile uint32_t*)(TIMER1_BASE + CMPY_OFFSET))
#define TMR1_CTRL       (*(volatile uint32_t*)(TIMER1_BASE + CTRL_OFFSET))
#define TMR1_CFG        (*(volatile uint32_t*)(TIMER1_BASE + CFG_OFFSET))
#define TMR1_PWM0CFG    (*(volatile uint32_t*)(TIMER1_BASE + PWM0CFG_OFFSET))
#define TMR1_PWM1CFG    (*(volatile uint32_t*)(TIMER1_BASE + PWM1CFG_OFFSET))
#define TMR1_PWMDT      (*(volatile uint32_t*)(TIMER1_BASE + PWMDT_OFFSET))
#define TMR1_PWMFC      (*(volatile uint32_t*)(TIMER1_BASE + PWMFC_OFFSET))
#define TMR1_IM         (*(volatile uint32_t*)(TIMER1_BASE + IM_OFFSET))
#define TMR1_MIS        (*(volatile uint32_t*)(TIMER1_BASE + MIS_OFFSET))
#define TMR1_RIS        (*(volatile uint32_t*)(TIMER1_BASE + RIS_OFFSET))
#define TMR1_IC         (*(volatile uint32_t*)(TIMER1_BASE + IC_OFFSET))
#define TMR1_GCLK       (*(volatile uint32_t*)(TIMER1_BASE + GCLK_OFFSET))

#define CTRL_EN         (1 << 0)
#define CTRL_START      (1 << 1)
#define CTRL_PERIODIC   (1 << 2)
#define CTRL_DIR_UP     (0 << 3)
#define CTRL_DIR_DOWN   (1 << 3)
#define CTRL_DIR_UPDOWN (2 << 3)

#define PWM_EN          (1 << 0)
#define PWM_INV         (1 << 1)

#define IRQ_TO          (1 << 0)
#define IRQ_MX          (1 << 1)
#define IRQ_MY          (1 << 2)

static inline void timer0_init(void) {
    TMR0_CTRL = 0;
    TMR0_RELOAD = 0;
    TMR0_PR = 0;
    TMR0_CMPX = 0;
    TMR0_CMPY = 0;
    TMR0_PWM0CFG = 0;
    TMR0_PWM1CFG = 0;
    TMR0_IM = 0;
    TMR0_IC = 0xFFFFFFFF;
}

static inline void timer1_init(void) {
    TMR1_CTRL = 0;
    TMR1_RELOAD = 0;
    TMR1_PR = 0;
    TMR1_CMPX = 0;
    TMR1_CMPY = 0;
    TMR1_PWM0CFG = 0;
    TMR1_PWM1CFG = 0;
    TMR1_IM = 0;
    TMR1_IC = 0xFFFFFFFF;
}

static inline void timer0_enable(void) {
    TMR0_CTRL |= (CTRL_EN | CTRL_START);
}

static inline void timer1_enable(void) {
    TMR1_CTRL |= (CTRL_EN | CTRL_START);
}

static inline void timer0_disable(void) {
    TMR0_CTRL &= ~(CTRL_EN | CTRL_START);
}

static inline void timer1_disable(void) {
    TMR1_CTRL &= ~(CTRL_EN | CTRL_START);
}

static inline uint32_t timer0_read(void) {
    return TMR0_TMR;
}

static inline uint32_t timer1_read(void) {
    return TMR1_TMR;
}

static inline void timer0_pwm_config(uint32_t reload, uint32_t cmpx, uint32_t cmpy) {
    TMR0_RELOAD = reload;
    TMR0_CMPX = cmpx;
    TMR0_CMPY = cmpy;
}

static inline void timer1_pwm_config(uint32_t reload, uint32_t cmpx, uint32_t cmpy) {
    TMR1_RELOAD = reload;
    TMR1_CMPX = cmpx;
    TMR1_CMPY = cmpy;
}

static inline void timer0_pwm_enable(uint8_t ch0_en, uint8_t ch1_en) {
    if (ch0_en) TMR0_PWM0CFG = PWM_EN;
    if (ch1_en) TMR0_PWM1CFG = PWM_EN;
}

static inline void timer1_pwm_enable(uint8_t ch0_en, uint8_t ch1_en) {
    if (ch0_en) TMR1_PWM0CFG = PWM_EN;
    if (ch1_en) TMR1_PWM1CFG = PWM_EN;
}

static inline void timer0_irq_enable(uint32_t mask) {
    TMR0_IM = mask;
}

static inline void timer1_irq_enable(uint32_t mask) {
    TMR1_IM = mask;
}

static inline void timer0_irq_clear(uint32_t mask) {
    TMR0_IC = mask;
}

static inline void timer1_irq_clear(uint32_t mask) {
    TMR1_IC = mask;
}

static inline uint32_t timer0_irq_status(void) {
    return TMR0_MIS;
}

static inline uint32_t timer1_irq_status(void) {
    return TMR1_MIS;
}

#endif
