# Integration Notes

## Overview

This document provides comprehensive integration guidance for the dual timer user project within the Caravel SoC framework.

## System Architecture

### Top-Level Hierarchy

```
caravel
  └── user_project_wrapper
        └── user_project
              ├── Wishbone Address Decoder
              ├── Wishbone Response Multiplexer
              ├── Timer0 (CF_TMR32_WB instance)
              └── Timer1 (CF_TMR32_WB instance)
```

### Module Descriptions

#### user_project_wrapper
- **Role**: Caravel interface wrapper
- **Responsibilities**:
  - Connects to Caravel Wishbone bus
  - Maps user_project signals to Caravel GPIO pads (mprj_io)
  - Handles power domain connections (VPWR/VGND)
- **File**: `verilog/rtl/user_project_wrapper.v`

#### user_project
- **Role**: Top-level user design
- **Responsibilities**:
  - Wishbone address decoding for 2 peripherals
  - Wishbone response multiplexing
  - Timer instantiation and interconnection
  - Interrupt aggregation
- **File**: `rtl/user_project.v` or `verilog/rtl/user_project.v`

#### CF_TMR32_WB (Timer0, Timer1)
- **Role**: Timer/PWM peripheral with Wishbone interface
- **Source**: Pre-verified IP from /nc/ip/CF_TMR32/v2.1.0-nc
- **Files**: Linked via ipm_linker, not copied

## Clock and Reset Architecture

### Clock Domain

**Single Clock Domain**: All logic operates in the Wishbone clock domain (`wb_clk_i`).

- **Source**: Caravel management SoC
- **Signal**: `wb_clk_i`
- **Typical Frequency**: 25 MHz (configurable)
- **No Clock Gating**: Per Caravel integration requirements
- **No Derived Clocks**: All logic uses wb_clk_i directly

### Reset Strategy

**Synchronous Active-High Reset**

- **Signal**: `wb_rst_i`
- **Type**: Synchronous, active-high (Caravel standard)
- **Assertion**: System-level reset controller
- **Propagation**: Direct connection to all sub-modules

```verilog
// Reset connection example
always @(posedge wb_clk_i) begin
    if (wb_rst_i) begin
        // Reset state
    end else begin
        // Normal operation
    end
end
```

### Timing Constraints

- **Target Clock Period**: 40 ns (25 MHz)
- **Setup Time**: Standard Caravel requirements
- **Hold Time**: Standard Caravel requirements
- **I/O Timing**: Registered outputs for clean timing

## Wishbone Bus Interface

### Protocol: Wishbone B4 Classic

#### Signal List

| Signal | Width | Direction | Description |
|--------|-------|-----------|-------------|
| wb_clk_i | 1 | Input | Wishbone clock |
| wb_rst_i | 1 | Input | Synchronous reset (active-high) |
| wbs_cyc_i | 1 | Input | Bus cycle indicator |
| wbs_stb_i | 1 | Input | Strobe (address/data valid) |
| wbs_we_i | 1 | Input | Write enable (1=write, 0=read) |
| wbs_sel_i | 4 | Input | Byte select/enable |
| wbs_adr_i | 32 | Input | Address bus |
| wbs_dat_i | 32 | Input | Write data bus |
| wbs_dat_o | 32 | Output | Read data bus |
| wbs_ack_o | 1 | Output | Transfer acknowledge |

#### Bus Timing

**Read Transaction**:
```
Cycle:     1       2       3
        ___     ___     ___
clk    |   |___|   |___|   |___
        _________________
cyc_i  |_________________|_____
        _________________
stb_i  |_________________|_____
                    _____
ack_o  ____________|     |_____
                    =====
dat_o  XXXXXXXXXXXXX DATA XXXXX
```

**Write Transaction**:
```
Cycle:     1       2       3
        ___     ___     ___
clk    |   |___|   |___|   |___
        _________________
cyc_i  |_________________|_____
        _________________
stb_i  |_________________|_____
        _________________
we_i   |_________________|_____
        =================
dat_i  ===== DATA =======XXXXX
                    _____
ack_o  ____________|     |_____
```

**Key Timing Rules**:
1. **ACK Latency**: Exactly 1 cycle after valid request
2. **Read Data**: Registered (available same cycle as ACK)
3. **Write Data**: Sampled when `we_i && cyc_i && stb_i`
4. **Byte Enables**: Supported via `sel_i[3:0]`

### Address Decoding Implementation

#### Address Space Allocation

```
User Project Space: 0x30000000 - 0x3FFFFFFF (256 MB)

Used Address Space:
  Timer0: 0x30000000 - 0x3000FFFF (64 KB)
  Timer1: 0x30010000 - 0x3001FFFF (64 KB)

Unused: 0x30020000 - 0x3FFFFFFF (254.875 MB available for expansion)
```

#### Decode Logic

**Peripheral Select Generation**:

```verilog
// Address decode bits: [19:16] for up to 16 peripherals
wire [3:0] periph_sel = wbs_adr_i[19:16];

// Timer0 select (periph_sel == 0)
wire tmr0_stb = (periph_sel == 4'h0) && wbs_stb_i;

// Timer1 select (periph_sel == 1)
wire tmr1_stb = (periph_sel == 4'h1) && wbs_stb_i;

// Invalid address
wire invalid_addr = (periph_sel >= 4'h2);
```

**Critical Rule**: **NEVER gate `wbs_cyc_i` with address decode**. Route `wbs_cyc_i` unmodified to all peripherals. Only gate `wbs_stb_i`.

```verilog
// CORRECT: Gate only STB
assign tmr0_stb = (periph_sel == 4'h0) && wbs_stb_i;

// WRONG: Do NOT gate CYC
// assign tmr0_cyc = (periph_sel == 4'h0) && wbs_cyc_i;  // NEVER DO THIS
```

#### Response Multiplexing

```verilog
// Collect ACKs from all peripherals
assign wbs_ack_o = tmr0_ack | tmr1_ack | invalid_ack;

// Multiplex read data
reg [31:0] wbs_dat_o;
always @(*) begin
    case (periph_sel)
        4'h0: wbs_dat_o = tmr0_dat_o;
        4'h1: wbs_dat_o = tmr1_dat_o;
        default: wbs_dat_o = 32'hDEADBEEF;  // Invalid address marker
    endcase
end
```

#### Invalid Address Handling

```verilog
// Generate ACK for invalid addresses to prevent bus hang
reg invalid_ack;
always @(posedge wb_clk_i) begin
    if (wb_rst_i)
        invalid_ack <= 1'b0;
    else if (invalid_addr && wbs_cyc_i && wbs_stb_i && !invalid_ack)
        invalid_ack <= 1'b1;
    else
        invalid_ack <= 1'b0;
end
```

### Byte-Lane Write Support

Each peripheral must honor `wbs_sel_i` for partial word writes:

```verilog
// Example: Byte-lane write to 32-bit register
always @(posedge wb_clk_i) begin
    if (wb_rst_i) begin
        my_reg <= 32'h0;
    end else if (write_enable) begin
        if (wbs_sel_i[0]) my_reg[7:0]   <= wbs_dat_i[7:0];
        if (wbs_sel_i[1]) my_reg[15:8]  <= wbs_dat_i[15:8];
        if (wbs_sel_i[2]) my_reg[23:16] <= wbs_dat_i[23:16];
        if (wbs_sel_i[3]) my_reg[31:24] <= wbs_dat_i[31:24];
    end
end
```

## Interrupt Architecture

### Interrupt Mapping

| Source | user_irq Bit | Description |
|--------|-------------|-------------|
| Timer0 IRQ | user_irq[0] | Timer0 interrupt (all sources OR'd) |
| Timer1 IRQ | user_irq[1] | Timer1 interrupt (all sources OR'd) |
| (unused) | user_irq[2] | Available for expansion |

### Interrupt Aggregation

```verilog
// Connect timer IRQs to Caravel user_irq
assign user_irq[0] = tmr0_irq;
assign user_irq[1] = tmr1_irq;
assign user_irq[2] = 1'b0;  // Unused
```

### Interrupt Characteristics

- **Type**: Level-high (active-high)
- **Clearing**: Write-1-to-clear via IC register in each timer
- **Masking**: Per-source masking via IM register in each timer
- **Latency**: Interrupt asserted within 2 clock cycles of event

### Software Interrupt Handling

```c
// Example interrupt handler
void timer0_isr(void) {
    uint32_t ris = *(volatile uint32_t*)(TIMER0_BASE + RIS_OFFSET);
    
    // Check specific interrupt sources
    if (ris & 0x01) {
        // Handle overflow interrupt
    }
    
    // Clear all serviced interrupts
    *(volatile uint32_t*)(TIMER0_BASE + IC_OFFSET) = ris;
}
```

## GPIO Pad Connections

Refer to [pad_map.md](pad_map.md) for complete pad assignments.

### Quick Reference

| Peripheral | Pads Used | Signals |
|-----------|-----------|---------|
| Timer0 | mprj_io[7:5] | pwm0, pwm1, fault |
| Timer1 | mprj_io[10:8] | pwm0, pwm1, fault |

### Connection Pattern in user_project_wrapper

```verilog
// Internal signals from user_project
wire tmr0_pwm0, tmr0_pwm1, tmr0_fault;
wire tmr1_pwm0, tmr1_pwm1, tmr1_fault;

// Timer0 pads
assign mprj_io_out[5] = tmr0_pwm0;
assign mprj_io_oeb[5] = 1'b0;
assign mprj_io_out[6] = tmr0_pwm1;
assign mprj_io_oeb[6] = 1'b0;
assign tmr0_fault = mprj_io_in[7];
assign mprj_io_out[7] = 1'b0;
assign mprj_io_oeb[7] = 1'b1;

// Timer1 pads
assign mprj_io_out[8] = tmr1_pwm0;
assign mprj_io_oeb[8] = 1'b0;
assign mprj_io_out[9] = tmr1_pwm1;
assign mprj_io_oeb[9] = 1'b0;
assign tmr1_fault = mprj_io_in[10];
assign mprj_io_out[10] = 1'b0;
assign mprj_io_oeb[10] = 1'b1;

// Unused pads (safe default)
assign mprj_io_out[37:11] = 27'b0;
assign mprj_io_oeb[37:11] = {27{1'b1}};
```

## Power Domains

### Power Pins

- **Digital Power**: VCCD1, VCCD2
- **Digital Ground**: VSSD1, VSSD2
- **Analog Power**: VDDA1, VDDA2 (not used in this design)
- **Analog Ground**: VSSA1, VSSA2 (not used in this design)

### user_project_wrapper Power Connections

```verilog
`ifdef USE_POWER_PINS
    .VPWR(vccd2),
    .VGND(vssd2)
`endif
```

## Design Constraints

### Verilog-2005 Compliance

**Required for Caravel**:
- Use only Verilog-2005 constructs (no SystemVerilog)
- No `always_ff`, `always_comb` (use `always @(posedge...)` and `always @(*)`)
- No `logic` type (use `reg` and `wire`)
- No `enum`, `struct`, `interface`

### No Latches

**All combinational logic must be latch-free**:
- Provide defaults for all outputs in `always @(*)`
- Complete `case` statements with `default`
- Complete `if-else` chains

### Single Clock Domain

**No clock domain crossings**:
- All logic in wb_clk_i domain
- No asynchronous FIFOs needed
- No clock gating (per requirements)

### No Tristate Logic

**Internal logic must not use tristate**:
- Use multiplexers for internal buses
- Tristate only at pad level (handled by Caravel)

## Synthesis and Linting

### Yosys Synthesis

Script location: `syn/yosys.ys`

**Key Requirements**:
- Check for inferred latches (must be zero)
- Verify all inputs/outputs are connected
- Report area and timing

**Expected Results**:
- No latches
- No unconnected ports
- Clean synthesis (no critical warnings)

### Verilator Linting

```bash
verilator --lint-only --Wall --Wno-EOFNEWLINE \
    -Iip/CF_TMR32/hdl/rtl \
    -Iip/CF_TMR32/hdl/rtl/bus_wrappers \
    -Iip/CF_IP_UTIL/hdl/rtl \
    verilog/rtl/user_project.v
```

**Required**:
- Zero errors
- Zero critical warnings
- Clean lint pass before proceeding to verification

## Verification with caravel-cocotb

### Test Structure

```
verilog/dv/cocotb/
  ├── timer0_test/
  │   ├── timer0_test.py
  │   ├── timer0_test.c
  │   └── Makefile
  ├── timer1_test/
  │   ├── timer1_test.py
  │   ├── timer1_test.c
  │   └── Makefile
  ├── dual_timer_test/
  │   ├── dual_timer_test.py
  │   ├── dual_timer_test.c
  │   └── Makefile
  ├── cocotb_tests.py
  └── design_info.yaml
```

### Test Requirements

**Timer0 Test**:
- Configure Timer0 for periodic interrupts
- Verify interrupt generation and clearing
- Test PWM0 and PWM1 outputs
- Verify fault handling

**Timer1 Test**:
- Same as Timer0 test but for Timer1
- Verify independent operation

**Dual Timer Test**:
- Configure both timers simultaneously
- Verify no bus conflicts
- Test interrupt priorities
- Verify independent PWM operation

### design_info.yaml

Must include:
- Top-level module: `caravel`
- Clock period
- Reset configuration
- DUT hierarchy path

### Running Tests

```bash
# Run all tests
make -C verilog/dv/cocotb test

# Run specific test
make -C verilog/dv/cocotb/timer0_test test

# View waveforms
gtkwave sim/timer0_test/timer0_test.vcd
```

## OpenLane Integration

### Two-Stage Hardening

**Stage 1**: Harden user_project (if required)
**Stage 2**: Harden user_project_wrapper with user_project as macro

For this design, if using soft integration:
- Only harden user_project_wrapper
- user_project RTL included directly

### OpenLane Configuration

Location: `openlane/user_project_wrapper/config.json`

**Key Settings**:
- `DESIGN_NAME`: "user_project_wrapper"
- `CLOCK_PORT`: "wb_clk_i"
- `CLOCK_PERIOD`: 25 (for 40MHz target)
- `VERILOG_FILES`: List all RTL sources
- `FP_PDN_MULTILAYER`: false

## Common Integration Issues

### Issue 1: Bus Hangs

**Symptom**: System stops responding to Wishbone transactions

**Causes**:
- Peripheral not acknowledging valid transactions
- `wbs_cyc_i` incorrectly gated with address decode
- Invalid addresses not acknowledged

**Solutions**:
- Ensure all peripherals generate ACK within 1-2 cycles
- Never gate `wbs_cyc_i` - route it unchanged to all slaves
- Implement invalid address ACK generator

### Issue 2: Incorrect Read Data

**Symptom**: Wrong data returned on Wishbone reads

**Causes**:
- Response multiplexer incorrect
- Multiple peripherals driving `wbs_dat_o`
- Unregistered read data

**Solutions**:
- Use case statement for clean multiplexing
- Ensure only one source drives output per address
- Register all read data outputs

### Issue 3: Interrupt Not Firing

**Symptom**: No interrupts received by software

**Causes**:
- IRQ not connected to user_irq
- Interrupt mask not configured
- Interrupt never cleared (stays latched)

**Solutions**:
- Verify user_irq connections in wrapper
- Check IM register configuration in firmware
- Implement proper W1C in interrupt handler

### Issue 4: PWM Not Toggling

**Symptom**: PWM outputs stuck at 0 or 1

**Causes**:
- Pad OEB configured wrong (input mode)
- Timer not enabled
- Compare values misconfigured

**Solutions**:
- Verify `mprj_io_oeb = 0` for outputs
- Check CTRL register enable bit
- Verify RELOAD > CMPX/CMPY

### Issue 5: Simulation vs. Silicon Mismatch

**Symptom**: Works in simulation but not on chip

**Causes**:
- Timing violations
- Asynchronous signals not synchronized
- Undefined initial states

**Solutions**:
- Check STA reports for violations
- Add synchronizers for async inputs (fault signals)
- Ensure all registers have reset values

## Design Validation Checklist

Before tape-out:

- [ ] All Wishbone transactions complete with ACK
- [ ] Invalid addresses handled gracefully
- [ ] No latches in synthesis reports
- [ ] Verilator lint clean
- [ ] All cocotb tests pass (RTL simulation)
- [ ] Gate-level simulation passes (post-synth)
- [ ] SDF simulation passes (timing annotated)
- [ ] Interrupts verified in hardware tests
- [ ] PWM waveforms verified on scope/LA
- [ ] Fault handling tested
- [ ] Power analysis complete
- [ ] DRC/LVS clean
- [ ] Documentation complete and reviewed

## References

### Internal Documentation
- [register_map.md](register_map.md) - Complete register descriptions
- [pad_map.md](pad_map.md) - GPIO pad assignments

### IP Documentation
- CF_TMR32 IP Manual: `/nc/ip/CF_TMR32/v2.1.0-nc/CF_TMR32.pdf`
- CF_TMR32 README: `/nc/ip/CF_TMR32/v2.1.0-nc/README.md`

### External References
- Caravel Documentation: https://caravel-harness.readthedocs.io/
- Wishbone B4 Specification: https://opencores.org/howto/wishbone
- cocotb Documentation: https://docs.cocotb.org/

---

**Document Version**: 1.0  
**Last Updated**: 2025-11-01  
**Project**: Dual Timer Caravel Integration
