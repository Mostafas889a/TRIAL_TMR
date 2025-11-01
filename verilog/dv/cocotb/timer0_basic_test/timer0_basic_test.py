from caravel_cocotb.caravel_interfaces import test_configure, report_test
import cocotb
from cocotb.triggers import ClockCycles

async def wait_for_pulses(env, count, label):
    for _ in range(count):
        await env.wait_mgmt_gpio(1)
        await env.wait_mgmt_gpio(0)
    cocotb.log.info(f"[TEST] {label}")

@cocotb.test()
@report_test
async def timer0_basic_test(dut):
    caravelEnv = await test_configure(dut, timeout_cycles=500000)

    cocotb.log.info("[TEST] Starting timer0_basic_test")

    await caravelEnv.release_csb()
    cocotb.log.info("[TEST] Wait for firmware configuration")
    await wait_for_pulses(caravelEnv, 1, "firmware ready")

    cocotb.log.info("[TEST] Wait for Timer0 PWM configuration")
    await wait_for_pulses(caravelEnv, 1, "Timer0 configured")

    cocotb.log.info("[TEST] Monitoring GPIO[5] (PWM0) and GPIO[6] (PWM1) for 5000 cycles")
    await ClockCycles(caravelEnv.clk, 10000)

    pwm0_high = 0
    pwm0_low = 0
    pwm1_high = 0
    pwm1_low = 0

    for i in range(5000):
        await ClockCycles(caravelEnv.clk, 1)

        pwm0_val = caravelEnv.monitor_gpio(5, 5).integer
        pwm1_val = caravelEnv.monitor_gpio(6, 6).integer

        if pwm0_val == 1:
            pwm0_high += 1
        else:
            pwm0_low += 1

        if pwm1_val == 1:
            pwm1_high += 1
        else:
            pwm1_low += 1

    duty_pwm0 = (pwm0_high / (pwm0_high + pwm0_low)) * 100
    duty_pwm1 = (pwm1_high / (pwm1_high + pwm1_low)) * 100

    cocotb.log.info(f"[TEST] Timer0 PWM0 (GPIO 5): {pwm0_high} high, {pwm0_low} low, duty={duty_pwm0:.2f}%")
    cocotb.log.info(f"[TEST] Timer0 PWM1 (GPIO 6): {pwm1_high} high, {pwm1_low} low, duty={duty_pwm1:.2f}%")

    test_passed = True

    if pwm0_high > 100 and pwm0_low > 100:
        cocotb.log.info("[TEST] Timer0 PWM0 is toggling - PASS")
    else:
        cocotb.log.error("[TEST] Timer0 PWM0 is NOT toggling - FAIL")
        test_passed = False

    if pwm1_high > 100 and pwm1_low > 100:
        cocotb.log.info("[TEST] Timer0 PWM1 is toggling - PASS")
    else:
        cocotb.log.error("[TEST] Timer0 PWM1 is NOT toggling - FAIL")
        test_passed = False

    if 10 < duty_pwm0 < 90:
        cocotb.log.info(f"[TEST] Timer0 PWM0 duty cycle {duty_pwm0:.2f}% is within range - PASS")
    else:
        cocotb.log.error(f"[TEST] Timer0 PWM0 duty cycle {duty_pwm0:.2f}% is out of range - FAIL")
        test_passed = False

    if 10 < duty_pwm1 < 90:
        cocotb.log.info(f"[TEST] Timer0 PWM1 duty cycle {duty_pwm1:.2f}% is within range - PASS")
    else:
        cocotb.log.error(f"[TEST] Timer0 PWM1 duty cycle {duty_pwm1:.2f}% is out of range - FAIL")
        test_passed = False

    if test_passed:
        cocotb.log.info("[TEST] timer0_basic_test complete - PASS")
    else:
        cocotb.log.error("[TEST] timer0_basic_test complete - FAIL")
