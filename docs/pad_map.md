# GPIO Pad Map Documentation

## Overview

This document describes the mapping between the user project peripherals and the Caravel ASIC GPIO pads (`mprj_io[37:0]`).

## Pad Assignment Strategy

- **Reserved Pads**: `mprj_io[4:0]` are reserved for Caravel management functions (not used)
- **Available Pads**: `mprj_io[37:5]` are available for user project
- **Default Assignments**: Starting from `mprj_io[5]` for clarity and consistency

## Default Pad Assignments

| Pad Index | Signal Name | Direction | Peripheral | Description |
|-----------|-------------|-----------|------------|-------------|
| mprj_io[5] | tmr0_pwm0 | Output | Timer0 | Timer0 PWM Channel 0 output |
| mprj_io[6] | tmr0_pwm1 | Output | Timer0 | Timer0 PWM Channel 1 output |
| mprj_io[7] | tmr0_fault | Input | Timer0 | Timer0 PWM fault input |
| mprj_io[8] | tmr1_pwm0 | Output | Timer1 | Timer1 PWM Channel 0 output |
| mprj_io[9] | tmr1_pwm1 | Output | Timer1 | Timer1 PWM Channel 1 output |
| mprj_io[10] | tmr1_fault | Input | Timer1 | Timer1 PWM fault input |
| mprj_io[37:11] | (unused) | - | - | Available for future expansion |

## Signal Descriptions

### Timer0 Signals

#### tmr0_pwm0 (mprj_io[5])
- **Direction**: Output (Push-pull)
- **Function**: PWM Channel 0 output from Timer0
- **Controlled by**: CMPX register and PWM0CFG register
- **Default State**: Low when PWM disabled
- **OEB Setting**: `mprj_io_oeb[5] = 1'b0` (output enabled)

#### tmr0_pwm1 (mprj_io[6])
- **Direction**: Output (Push-pull)
- **Function**: PWM Channel 1 output from Timer0
- **Controlled by**: CMPY register and PWM1CFG register
- **Default State**: Low when PWM disabled
- **OEB Setting**: `mprj_io_oeb[6] = 1'b0` (output enabled)

#### tmr0_fault (mprj_io[7])
- **Direction**: Input
- **Function**: External fault signal for Timer0 PWM protection
- **Active Level**: Active high (fault condition when high)
- **Response**: Disables PWM outputs when asserted
- **OEB Setting**: `mprj_io_oeb[7] = 1'b1` (input mode)

### Timer1 Signals

#### tmr1_pwm0 (mprj_io[8])
- **Direction**: Output (Push-pull)
- **Function**: PWM Channel 0 output from Timer1
- **Controlled by**: CMPX register and PWM0CFG register
- **Default State**: Low when PWM disabled
- **OEB Setting**: `mprj_io_oeb[8] = 1'b0` (output enabled)

#### tmr1_pwm1 (mprj_io[9])
- **Direction**: Output (Push-pull)
- **Function**: PWM Channel 1 output from Timer1
- **Controlled by**: CMPY register and PWM1CFG register
- **Default State**: Low when PWM disabled
- **OEB Setting**: `mprj_io_oeb[9] = 1'b0` (output enabled)

#### tmr1_fault (mprj_io[10])
- **Direction**: Input
- **Function**: External fault signal for Timer1 PWM protection
- **Active Level**: Active high (fault condition when high)
- **Response**: Disables PWM outputs when asserted
- **OEB Setting**: `mprj_io_oeb[10] = 1'b1` (input mode)

## Pad Configuration in RTL

### Output Pad Configuration (PWM Outputs)

For each PWM output (e.g., Timer0 PWM0 on mprj_io[5]):

```verilog
// Connect PWM output to pad
assign mprj_io_out[5] = tmr0_pwm0;

// Enable output driver (OEB active low)
assign mprj_io_oeb[5] = 1'b0;
```

### Input Pad Configuration (Fault Inputs)

For each fault input (e.g., Timer0 fault on mprj_io[7]):

```verilog
// Connect pad input to fault signal
assign tmr0_fault = mprj_io_in[7];

// Disable output driver (configure as input)
assign mprj_io_out[7] = 1'b0;
assign mprj_io_oeb[7] = 1'b1;
```

### Unused Pads Configuration

For unused pads (mprj_io[37:11]):

```verilog
// Tie outputs low
assign mprj_io_out[37:11] = 27'b0;

// Configure as inputs (safe default)
assign mprj_io_oeb[37:11] = {27{1'b1}};
```

## Complete Pad Connections in user_project_wrapper.v

```verilog
// Timer0 connections
assign mprj_io_out[5] = tmr0_pwm0;       // Timer0 PWM0 output
assign mprj_io_oeb[5] = 1'b0;            // Output enable

assign mprj_io_out[6] = tmr0_pwm1;       // Timer0 PWM1 output
assign mprj_io_oeb[6] = 1'b0;            // Output enable

assign tmr0_fault = mprj_io_in[7];       // Timer0 fault input
assign mprj_io_out[7] = 1'b0;            // Input mode
assign mprj_io_oeb[7] = 1'b1;            // Input enable

// Timer1 connections
assign mprj_io_out[8] = tmr1_pwm0;       // Timer1 PWM0 output
assign mprj_io_oeb[8] = 1'b0;            // Output enable

assign mprj_io_out[9] = tmr1_pwm1;       // Timer1 PWM1 output
assign mprj_io_oeb[9] = 1'b0;            // Output enable

assign tmr1_fault = mprj_io_in[10];      // Timer1 fault input
assign mprj_io_out[10] = 1'b0;           // Input mode
assign mprj_io_oeb[10] = 1'b1;           // Input enable

// Unused pads (safe defaults)
assign mprj_io_out[37:11] = 27'b0;       // Tie low
assign mprj_io_oeb[37:11] = {27{1'b1}};  // Configure as inputs
```

## Caravel OEB Conventions

**Important**: Caravel uses an **active-low** output enable:
- `mprj_io_oeb = 1'b0` → Output mode (driver enabled)
- `mprj_io_oeb = 1'b1` → Input mode (driver disabled)

## Testing and Verification

### PWM Output Testing

To verify PWM outputs are correctly connected:

1. Configure Timer0 for 50% duty cycle PWM on PWM0
2. Monitor mprj_io[5] in simulation/on board
3. Verify square wave with expected frequency and duty cycle
4. Repeat for all PWM outputs

### Fault Input Testing

To verify fault inputs are correctly connected:

1. Drive mprj_io[7] high (fault condition)
2. Verify Timer0 PWM outputs are disabled
3. Clear fault via PWMFC register
4. Verify PWM outputs resume operation
5. Repeat for Timer1 fault input

## Customizing Pad Assignments

To change pad assignments:

1. **Update this documentation** with new pad assignments
2. **Modify user_project_wrapper.v** to reflect new connections
3. **Update testbenches** to use new pad indices
4. **Update firmware headers** if pad assignments are exposed to software
5. **Re-run verification** to ensure connections are correct

### Example: Moving Timer0 PWM0 to mprj_io[15]

```verilog
// New assignment
assign mprj_io_out[15] = tmr0_pwm0;
assign mprj_io_oeb[15] = 1'b0;

// Free up old pad (mprj_io[5])
assign mprj_io_out[5] = 1'b0;
assign mprj_io_oeb[5] = 1'b1;
```

## Pin Multiplexing Considerations

Currently, no pin multiplexing is implemented. Each signal has a dedicated pad.

If pin multiplexing is required in the future:
1. Add multiplexing control logic in user_project
2. Expose control via Wishbone registers
3. Update this document with multiplexing modes
4. Implement safe defaults (e.g., all outputs disabled at reset)

## Physical Layout Considerations

When placing the design in Caravel:
1. PWM outputs should be placed on the same side for easier PCB routing
2. Fault inputs can be grouped together
3. Consider signal integrity for high-frequency PWM signals
4. Place decoupling capacitors near PWM output pads

## Summary Table - Quick Reference

| Signal | Pad | Dir | Default OEB | Default Out | Notes |
|--------|-----|-----|-------------|-------------|-------|
| tmr0_pwm0 | 5 | Out | 0 | PWM | Timer0 CH0 |
| tmr0_pwm1 | 6 | Out | 0 | PWM | Timer0 CH1 |
| tmr0_fault | 7 | In | 1 | 0 | Timer0 fault |
| tmr1_pwm0 | 8 | Out | 0 | PWM | Timer1 CH0 |
| tmr1_pwm1 | 9 | Out | 0 | PWM | Timer1 CH1 |
| tmr1_fault | 10 | In | 1 | 0 | Timer1 fault |
| (unused) | 37:11 | - | 1 | 0 | Safe default |

## References

- Caravel GPIO Configuration: `/nc/templates/caravel_user_project/verilog/rtl/user_project_wrapper.v`
- CF_TMR32 IP Documentation: `/nc/ip/CF_TMR32/v2.1.0-nc/README.md`
- Caravel Documentation: https://caravel-harness.readthedocs.io/

---

**Document Version**: 1.0  
**Last Updated**: 2025-11-01  
**Design**: Dual Timer Integration
