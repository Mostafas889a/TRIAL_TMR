# Verification Plan: Dual Timer Caravel Integration

## Project Overview
Integration of 2x CF_TMR32 timer/PWM peripherals into Caravel SoC with Wishbone bus interface.

**Date**: 2025-11-01  
**Design Under Test**: user_project with dual CF_TMR32 instances  
**Framework**: Caravel-Cocotb  
**Clock Frequency**: 40 MHz (25ns period)

---

## Design Specification Summary

### Address Map
| Peripheral | Base Address | Range | Wishbone Decode |
|------------|--------------|-------|-----------------|
| Timer0 (CF_TMR32) | 0x30000000 | 64KB | adr_i[19:16] = 0x0 |
| Timer1 (CF_TMR32) | 0x30010000 | 64KB | adr_i[19:16] = 0x1 |
| Invalid | - | - | adr_i[19:16] >= 0x2 |

### GPIO Pad Assignments
| Signal | Pad | Direction | Function |
|--------|-----|-----------|----------|
| tmr0_pwm0 | io[5] | Output | Timer0 PWM Channel 0 |
| tmr0_pwm1 | io[6] | Output | Timer0 PWM Channel 1 |
| tmr0_fault | io[7] | Input | Timer0 Fault Input |
| tmr1_pwm0 | io[8] | Output | Timer1 PWM Channel 0 |
| tmr1_pwm1 | io[9] | Output | Timer1 PWM Channel 1 |
| tmr1_fault | io[10] | Input | Timer1 Fault Input |

### Interrupt Mapping
- `user_irq[0]` ← Timer0 IRQ
- `user_irq[1]` ← Timer1 IRQ
- `user_irq[2]` ← Unused (tied to 0)

---

## Verification Strategy

### Test Hierarchy
1. **Peripheral-Level Tests** - Individual timer validation
2. **Integration Tests** - Multi-timer operation
3. **System Tests** - Full Caravel integration

### Coverage Goals
- **Functional Coverage**: 
  - Wishbone bus transactions (read/write to all registers)
  - Timer counting modes (up, down, up-down)
  - PWM generation (both channels)
  - Interrupt generation
  - GPIO pad connectivity
  
- **Integration Coverage**:
  - Address decoder correctness
  - Bus arbitration (sequential access to both timers)
  - Invalid address handling (return 0xDEADBEEF)
  - Concurrent timer operation

---

## Test Plan

### Test 1: Timer0 Basic Operation (`timer0_basic_test`)
**Objective**: Verify Timer0 Wishbone access, PWM generation, and GPIO output

**Test Sequence**:
1. Firmware configures GPIO pads (5,6,7)
2. Enable user interface
3. Configure Timer0 for PWM mode
4. Set RELOAD, CMPX, CMPY registers
5. Enable PWM channels 0 and 1
6. Signal testbench (mgmt_gpio pulse)

**Python Testbench**:
- Wait for firmware ready signal
- Monitor GPIO[5] (pwm0) and GPIO[6] (pwm1) for 5000 cycles
- Calculate duty cycles
- Verify both channels toggle with expected duty cycle (>10%, <90%)

**Pass Criteria**:
- ✓ Both PWM outputs toggle
- ✓ Duty cycle within expected range
- ✓ No bus errors

**Expected Duty Cycle**: ~50% (based on CF_TMR32_configureExamplePWM)

---

### Test 2: Timer1 Basic Operation (`timer1_basic_test`)
**Objective**: Verify Timer1 Wishbone access, PWM generation, and GPIO output

**Test Sequence**: (Similar to Timer0)
1. Firmware configures GPIO pads (8,9,10)
2. Enable user interface
3. Configure Timer1 for PWM mode
4. Set RELOAD, CMPX, CMPY registers
5. Enable PWM channels 0 and 1
6. Signal testbench (mgmt_gpio pulse)

**Python Testbench**:
- Wait for firmware ready signal
- Monitor GPIO[8] (pwm0) and GPIO[9] (pwm1) for 5000 cycles
- Calculate duty cycles
- Verify both channels toggle

**Pass Criteria**:
- ✓ Both PWM outputs toggle
- ✓ Duty cycle within expected range
- ✓ No bus errors

---

### Test 3: Dual Timer Integration (`dual_timer_test`)
**Objective**: Verify concurrent operation of both timers

**Test Sequence**:
1. Firmware configures all GPIO pads (5,6,7,8,9,10)
2. Enable user interface
3. Configure Timer0 with specific PWM parameters (e.g., 40% duty)
4. Configure Timer1 with different PWM parameters (e.g., 60% duty)
5. Enable both timers simultaneously
6. Signal testbench (mgmt_gpio pulse)

**Python Testbench**:
- Wait for firmware ready signal
- Monitor all 4 PWM outputs (GPIO[5:6], GPIO[8:9]) for 5000 cycles
- Calculate individual duty cycles
- Verify independent operation (different duty cycles)

**Pass Criteria**:
- ✓ All 4 PWM outputs toggle independently
- ✓ Timer0 and Timer1 duty cycles differ
- ✓ No cross-talk or interference
- ✓ Bus transactions to both timers succeed

---

### Test 4: Address Decoder Validation (`address_decode_test`)
**Objective**: Verify Wishbone address decoder logic

**Test Sequence**:
1. Firmware writes to Timer0 registers (0x30000000 base)
2. Firmware reads back Timer0 registers
3. Firmware writes to Timer1 registers (0x30010000 base)
4. Firmware reads back Timer1 registers
5. Firmware attempts write to invalid address (0x30020000)
6. Firmware reads from invalid address (should return 0xDEADBEEF)
7. Signal results via mgmt_gpio pulses

**Pass Criteria**:
- ✓ Correct data read from Timer0 registers
- ✓ Correct data read from Timer1 registers
- ✓ Invalid reads return 0xDEADBEEF
- ✓ Invalid writes acknowledged but discarded

---

### Test 5: Interrupt Handling (`interrupt_test`)
**Objective**: Verify timer interrupt generation and routing

**Test Sequence**:
1. Firmware enables Timer0 interrupts (timeout, match)
2. Firmware configures Timer0 for short period
3. Firmware waits for interrupt
4. Firmware clears interrupt
5. Firmware enables Timer1 interrupts
6. Firmware waits for Timer1 interrupt
7. Signal testbench with results

**Python Testbench**:
- Monitor user_irq[0] and user_irq[1] signals
- Verify interrupts asserted at expected times
- Verify interrupt clearing

**Pass Criteria**:
- ✓ Timer0 IRQ asserts on user_irq[0]
- ✓ Timer1 IRQ asserts on user_irq[1]
- ✓ Interrupts clear when firmware writes IC register

---

## Test Deliverables

Each test will produce:
1. **Firmware binary** (`<test_name>.hex`)
2. **Simulation log** (`<test_name>.log`)
3. **Waveform dump** (`waves.vcd`)
4. **Pass/Fail indicator** (`passed` or `failed` file)

---

## Test Execution Plan

### Phase 1: Individual Timer Tests
```bash
cd /workspace/TRIAL_TMR/verilog/dv/cocotb
caravel_cocotb -t timer0_basic_test -tag rtl_verification
caravel_cocotb -t timer1_basic_test -tag rtl_verification
```

### Phase 2: Integration Tests
```bash
caravel_cocotb -t dual_timer_test -tag rtl_verification
caravel_cocotb -t address_decode_test -tag rtl_verification
caravel_cocotb -t interrupt_test -tag rtl_verification
```

### Phase 3: Regression (All Tests)
```bash
caravel_cocotb -tl timer_testlist.yaml -tag regression
```

---

## Success Criteria

### Minimum Acceptance:
- [ ] All 5 tests pass in RTL simulation
- [ ] No Wishbone protocol violations
- [ ] All PWM outputs toggle correctly
- [ ] Address decoder handles invalid addresses
- [ ] Interrupts route correctly to user_irq

### Desired Coverage:
- [ ] 100% register access coverage
- [ ] All timer counting modes exercised
- [ ] Both PWM channels per timer verified
- [ ] Fault input functionality tested
- [ ] Concurrent timer operation validated

---

## Known Limitations & Risks

1. **Timing Accuracy**: PWM duty cycle measurements are cycle-approximate due to sampling
2. **Fault Input**: Fault functionality not tested in basic plan (can be added)
3. **Prescaler**: Not extensively tested (uses default configuration)
4. **Dead-time**: Advanced PWM features not covered in basic tests

---

## Tools & Environment

- **Simulator**: Icarus Verilog (via caravel-cocotb)
- **Framework**: Caravel-Cocotb
- **Language**: Python 3.x + Cocotb, C (RISC-V firmware)
- **PDK**: Sky130A
- **Clock**: 40 MHz (25ns period)

---

## References

- CF_TMR32 IP Documentation: `/workspace/TRIAL_TMR/ip/CF_TMR32/`
- Caravel User Project Template: `/nc/templates/caravel_user_project/`
- Caravel-Cocotb Documentation: (loaded via microagent)
- Register Map: `docs/register_map.md`
- Pad Map: `docs/pad_map.md`

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-11-01 | NativeChips Agent | Initial verification plan |

