# Register Map Documentation

## Address Space Overview

The user project implements two CF_TMR32 timer/PWM peripherals accessible via the Wishbone bus interface. Each peripheral occupies a 64 KB address window.

| Peripheral | Base Address | End Address | Size | Description |
|-----------|--------------|-------------|------|-------------|
| Timer0 (TMR0) | 0x30000000 | 0x3000FFFF | 64 KB | Timer/PWM Channel 0 |
| Timer1 (TMR1) | 0x30010000 | 0x3001FFFF | 64 KB | Timer/PWM Channel 1 |

## Address Decoding

The Wishbone address decoder uses bits [19:16] to select between peripherals:
- `adr_i[19:16] == 4'h0` → Timer0
- `adr_i[19:16] == 4'h1` → Timer1
- Other values → Invalid (returns 0xDEADBEEF on read, ACK but discard on write)

## CF_TMR32 Register Map

Both Timer0 and Timer1 implement identical register maps at their respective base addresses.

### Functional Registers

| Register | Offset | Reset Value | Access | Width | Description |
|----------|--------|-------------|--------|-------|-------------|
| TMR | 0x0000 | 0x00000000 | RO | 32-bit | Current timer value |
| RELOAD | 0x0004 | 0x00000000 | RW | 32-bit | Timer reload/terminal count value |
| PR | 0x0008 | 0x00000000 | RW | 16-bit | Prescaler: Count frequency = clk/(PR+1) |
| CMPX | 0x000C | 0x00000000 | RW | 32-bit | Compare register X (PWM0) |
| CMPY | 0x0010 | 0x00000000 | RW | 32-bit | Compare register Y (PWM1) |
| CTRL | 0x0014 | 0x00000000 | RW | 32-bit | Control register |
| CFG | 0x0018 | 0x00000000 | RW | 32-bit | Configuration register |
| PWM0CFG | 0x001C | 0x00000000 | RW | 32-bit | PWM channel 0 configuration |
| PWM1CFG | 0x0020 | 0x00000000 | RW | 32-bit | PWM channel 1 configuration |
| PWMDT | 0x0024 | 0x00000000 | RW | 32-bit | PWM deadtime register |
| PWMFC | 0x0028 | 0x00000000 | W1C | 32-bit | PWM fault clear register |

### Interrupt Registers

| Register | Offset | Reset Value | Access | Width | Description |
|----------|--------|-------------|--------|-------|-------------|
| IM | 0xFF00 | 0x00000000 | RW | 32-bit | Interrupt mask (1=enable, 0=disable) |
| MIS | 0xFF04 | 0x00000000 | RO | 32-bit | Masked interrupt status |
| RIS | 0xFF08 | 0x00000000 | RO | 32-bit | Raw interrupt status |
| IC | 0xFF0C | 0x00000000 | W1C | 32-bit | Interrupt clear (write 1 to clear) |

### Clock Gating Register

| Register | Offset | Reset Value | Access | Width | Description |
|----------|--------|-------------|--------|-------|-------------|
| GCLK | 0xFF10 | 0x00000000 | RW | 1-bit | Clock gate enable (1=enable, 0=disable) |

**Note**: Clock gating is NOT used in this Caravel integration per system requirements.

## Register Details

### TMR Register (Offset: 0x0000)

**Read-Only** - Returns current timer count value.

| Bits | Field | Type | Reset | Description |
|------|-------|------|-------|-------------|
| 31:0 | TMR | RO | 0x00000000 | Current timer count |

### RELOAD Register (Offset: 0x0004)

**Read-Write** - Timer reload value.

| Bits | Field | Type | Reset | Description |
|------|-------|------|-------|-------------|
| 31:0 | RELOAD | RW | 0x00000000 | Up count: terminal count; Down count: initial value |

### PR Register (Offset: 0x0008)

**Read-Write** - Prescaler configuration.

| Bits | Field | Type | Reset | Description |
|------|-------|------|-------|-------------|
| 15:0 | PR | RW | 0x0000 | Prescaler value: f_count = f_clk / (PR + 1) |
| 31:16 | Reserved | - | - | Reserved (return 0 on read) |

### CMPX Register (Offset: 0x000C)

**Read-Write** - Compare register X for PWM0 generation.

| Bits | Field | Type | Reset | Description |
|------|-------|------|-------|-------------|
| 31:0 | CMPX | RW | 0x00000000 | PWM0 compare threshold |

### CMPY Register (Offset: 0x0010)

**Read-Write** - Compare register Y for PWM1 generation.

| Bits | Field | Type | Reset | Description |
|------|-------|------|-------|-------------|
| 31:0 | CMPY | RW | 0x00000000 | PWM1 compare threshold |

### CTRL Register (Offset: 0x0014)

**Read-Write** - Timer control.

Refer to CF_TMR32 IP documentation for detailed bit field definitions. Key functions include:
- Timer enable/disable
- Count direction (up/down/up-down)
- One-shot vs periodic mode
- Timer start/stop control

### CFG Register (Offset: 0x0018)

**Read-Write** - Timer configuration.

Refer to CF_TMR32 IP documentation for detailed bit field definitions.

### PWM0CFG Register (Offset: 0x001C)

**Read-Write** - PWM channel 0 configuration.

Refer to CF_TMR32 IP documentation for detailed bit field definitions. Includes:
- PWM enable
- Polarity inversion
- Output enable

### PWM1CFG Register (Offset: 0x0020)

**Read-Write** - PWM channel 1 configuration.

Refer to CF_TMR32 IP documentation for detailed bit field definitions. Includes:
- PWM enable
- Polarity inversion
- Output enable

### PWMDT Register (Offset: 0x0024)

**Read-Write** - PWM deadtime/deadband configuration for half-H bridge applications.

Refer to CF_TMR32 IP documentation for detailed bit field definitions.

### PWMFC Register (Offset: 0x0028)

**Write-1-to-Clear** - PWM fault clear register.

| Bits | Field | Type | Reset | Description |
|------|-------|------|-------|-------------|
| 0 | FC | W1C | 0 | Write 1 to clear PWM fault condition |
| 31:1 | Reserved | - | - | Reserved |

### Interrupt Mask Register (IM, Offset: 0xFF00)

**Read-Write** - Enable/disable interrupt sources.

| Bits | Field | Type | Reset | Description |
|------|-------|------|-------|-------------|
| [Various] | - | RW | 0x00000000 | 1=Enable interrupt, 0=Disable |

Refer to CF_TMR32 IP documentation for specific interrupt bit assignments.

### Masked Interrupt Status (MIS, Offset: 0xFF04)

**Read-Only** - Masked (enabled) interrupt status.

Returns the logical AND of RIS and IM registers.

### Raw Interrupt Status (RIS, Offset: 0xFF08)

**Read-Only** - Raw interrupt status (before masking).

### Interrupt Clear Register (IC, Offset: 0xFF0C)

**Write-1-to-Clear** - Clear interrupt flags.

Write 1 to a bit position to clear both the corresponding RIS and MIS flag.

## Interrupt Sources

Each timer can generate interrupts on the following events (refer to CF_TMR32 documentation for complete list):
- Timer overflow/underflow
- Match on CMPX
- Match on CMPY
- PWM fault condition

## Usage Examples

### Example 1: Configure Timer0 for 1ms Periodic Interrupt

Assuming wb_clk_i = 25 MHz:

```c
// Base addresses
#define TIMER0_BASE 0x30000000
#define TIMER1_BASE 0x30010000

// Register offsets
#define TMR_OFFSET    0x0000
#define RELOAD_OFFSET 0x0004
#define PR_OFFSET     0x0008
#define CTRL_OFFSET   0x0014
#define IM_OFFSET     0xFF00
#define IC_OFFSET     0xFF0C

// Configure for 1ms tick (25MHz / 25000 = 1kHz)
*(volatile uint32_t*)(TIMER0_BASE + PR_OFFSET) = 24;      // Prescaler: 25MHz/25 = 1MHz
*(volatile uint32_t*)(TIMER0_BASE + RELOAD_OFFSET) = 999; // 1MHz/1000 = 1kHz (1ms)
*(volatile uint32_t*)(TIMER0_BASE + IM_OFFSET) = 0x01;    // Enable overflow interrupt
*(volatile uint32_t*)(TIMER0_BASE + CTRL_OFFSET) = 0x03;  // Enable timer, periodic mode
```

### Example 2: Configure Timer1 for 50% Duty Cycle PWM at 10kHz

```c
// Configure for 10kHz PWM (25MHz / 2500 = 10kHz)
*(volatile uint32_t*)(TIMER1_BASE + PR_OFFSET) = 0;        // No prescaler
*(volatile uint32_t*)(TIMER1_BASE + RELOAD_OFFSET) = 2499; // 25MHz/2500 = 10kHz
*(volatile uint32_t*)(TIMER1_BASE + CMPX_OFFSET) = 1250;   // 50% duty cycle
*(volatile uint32_t*)(TIMER1_BASE + PWM0CFG_OFFSET) = 0x01; // Enable PWM0
*(volatile uint32_t*)(TIMER1_BASE + CTRL_OFFSET) = 0x03;   // Enable timer, up-count mode
```

### Example 3: Read Current Timer Value

```c
uint32_t current_count = *(volatile uint32_t*)(TIMER0_BASE + TMR_OFFSET);
```

### Example 4: Clear Interrupt

```c
// In interrupt handler
*(volatile uint32_t*)(TIMER0_BASE + IC_OFFSET) = 0xFFFFFFFF; // Clear all interrupts
```

## Bus Protocol Notes

### Wishbone Transaction Timing

- **Read Latency**: 1 cycle (registered output)
- **Write Latency**: 1 cycle
- **ACK Assertion**: Exactly 1 cycle per valid transaction
- **Byte Enables**: Supported via `wbs_sel_i[3:0]`

### Invalid Address Handling

- **Read from invalid address**: Returns `0xDEADBEEF`, asserts ACK
- **Write to invalid address**: ACK asserted, data discarded

### Byte-Lane Writes

The Wishbone interface supports byte-lane writes via `wbs_sel_i`:
- `sel_i[0]` → byte 0 (bits 7:0)
- `sel_i[1]` → byte 1 (bits 15:8)
- `sel_i[2]` → byte 2 (bits 23:16)
- `sel_i[3]` → byte 3 (bits 31:24)

## Integration Notes

1. **Clock Domain**: All registers operate in the `wb_clk_i` domain
2. **Reset**: Synchronous active-high reset via `wb_rst_i`
3. **Interrupts**: Timer0 and Timer1 IRQs are OR'd together and mapped to `user_irq[0]`
4. **PWM Outputs**: Connected to Caravel GPIO pads (see pad_map.md)
5. **PWM Fault Input**: Connected to Caravel GPIO pads (see pad_map.md)

## Reference Documentation

For complete register bit field definitions and operational details, refer to:
- `/nc/ip/CF_TMR32/v2.1.0-nc/CF_TMR32.pdf`
- `/nc/ip/CF_TMR32/v2.1.0-nc/README.md`

---

**Document Version**: 1.0  
**Last Updated**: 2025-11-01  
**IP Version**: CF_TMR32 v2.1.0-nc
