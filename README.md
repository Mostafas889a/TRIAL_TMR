# Caravel User Project: Dual Timer Integration

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

## Project Overview

**Initial User Prompt:**
> Integrate a custom user project into the Caravel SoC with the following peripherals: 2 x timer

This project integrates two timer peripherals (CF_TMR32) into the Caravel SoC framework, providing timer functionality with PWM capabilities accessible via the Wishbone bus interface.

## Project Status

🚧 **Status**: In Progress - Project Setup Phase

**Current Stage**: Project Setup
**Overall Progress**: 10%

### Completion Checklist
- [x] Caravel template copied
- [x] Initial documentation created
- [ ] IP cores linked
- [ ] User project RTL developed
- [ ] User project wrapper created
- [ ] Verification tests created
- [ ] Tests passing
- [ ] Documentation complete

## Objectives

1. **Timer Integration**: Integrate 2 instances of CF_TMR32 timer/PWM IP core
2. **Wishbone Interface**: Implement compliant Wishbone B4 Classic bus interface
3. **Address Map**: Assign non-overlapping 64KB address windows for each timer
4. **Verification**: Create comprehensive cocotb tests for functional verification
5. **Documentation**: Provide complete register maps, pad assignments, and integration notes

## Design Specifications

### Timer Peripherals
- **IP Core**: CF_TMR32 (32-bit Timer with PWM functionality)
- **Quantity**: 2 instances (Timer0, Timer1)
- **Source**: Pre-verified IP from /nc/ip/CF_TMR32

### Address Map
| Peripheral | Base Address | Address Range | Size |
|-----------|--------------|---------------|------|
| Timer0 | 0x30000000 | 0x30000000 - 0x3000FFFF | 64 KB |
| Timer1 | 0x30010000 | 0x30010000 - 0x3001FFFF | 64 KB |

### Wishbone Interface
- Protocol: Wishbone B4 Classic
- Data Width: 32-bit
- Address Width: 32-bit
- Byte Enable: 4-bit
- Single clock domain (wb_clk_i)
- Synchronous active-high reset (wb_rst_i)

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│            Caravel SoC (Management Core)                │
└────────────────────┬────────────────────────────────────┘
                     │ Wishbone Bus
                     │
┌────────────────────┴────────────────────────────────────┐
│          user_project_wrapper                           │
│  ┌──────────────────────────────────────────────────┐  │
│  │           user_project                            │  │
│  │  ┌────────────────┐      ┌────────────────┐     │  │
│  │  │ Address Decode │      │  WB Multiplexer│     │  │
│  │  └───────┬────────┘      └────────┬───────┘     │  │
│  │          │                        │             │  │
│  │    ┌─────┴────┐            ┌─────┴────┐        │  │
│  │    │ Timer0   │            │ Timer1   │        │  │
│  │    │(CF_TMR32)│            │(CF_TMR32)│        │  │
│  │    └──────────┘            └──────────┘        │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

## Technology Stack

- **HDL**: Verilog-2005
- **IP Cores**: CF_TMR32 v1.1.0
- **Bus Protocol**: Wishbone B4 Classic
- **Verification**: cocotb + caravel-cocotb
- **Synthesis**: Yosys
- **PnR**: OpenLane 2
- **PDK**: SKY130

## Project Structure

```
TRIAL_TMR/
├── docs/                      # Project documentation
│   ├── register_map.md        # Peripheral register definitions
│   ├── pad_map.md            # GPIO pad assignments
│   └── integration_notes.md   # Integration guide
├── ip/                        # IP core links (via ipm_linker)
├── rtl/                       # RTL source files
│   ├── user_project.v        # Top-level user project
│   └── user_project_wrapper.v # Caravel wrapper
├── verilog/                   # Verilog hierarchy
│   ├── dv/                   # Design verification
│   │   └── cocotb/           # Cocotb testbenches
│   ├── rtl/                  # RTL sources
│   └── gl/                   # Gate-level netlists
├── fw/                        # Firmware support
│   ├── user_periph.h         # Peripheral definitions
│   └── smoke.c               # Smoke test
├── syn/                       # Synthesis scripts
├── openlane/                  # OpenLane configurations
└── README.md                  # This file
```

## Development Plan

### Phase 1: Project Setup ✅ (In Progress)
- [x] Copy Caravel user project template
- [x] Initialize documentation structure
- [ ] Link CF_TMR32 IP cores using ipm_linker
- [ ] Create initial documentation (register_map, pad_map, integration_notes)

### Phase 2: RTL Development
- [ ] Create user_project.v with Wishbone address decoder
- [ ] Instantiate 2x CF_TMR32 with Wishbone wrappers
- [ ] Implement Wishbone multiplexing logic
- [ ] Create user_project_wrapper.v
- [ ] Connect timer outputs to GPIO pads
- [ ] Lint all RTL with Verilator

### Phase 3: Verification
- [ ] Trigger Caravel-cocotb microagent
- [ ] Trigger TMR32 verification microagent
- [ ] Create Timer0 functional test
- [ ] Create Timer1 functional test
- [ ] Create system integration test
- [ ] Run all caravel-cocotb tests
- [ ] Verify Wishbone protocol compliance
- [ ] Generate waveforms and coverage reports

### Phase 4: Verification Evaluation
- [ ] Delegate to VerificationEvaluatorAgent
- [ ] Address evaluation feedback
- [ ] Confirm all tests passing

### Phase 5: Documentation
- [ ] Complete register_map.md
- [ ] Complete pad_map.md
- [ ] Complete integration_notes.md
- [ ] Create firmware examples
- [ ] Final README update

## Key Design Decisions

### 1. IP Core Selection
- **Chosen**: CF_TMR32 from NativeChips verified IPs
- **Rationale**: Pre-verified, production-ready timer with PWM functionality
- **Version**: v1.1.0

### 2. Address Map Layout
- 64 KB windows per peripheral (standard Caravel practice)
- Base address: 0x30000000 (user project space)
- Sequential allocation for simplicity

### 3. Clock Strategy
- Single clock domain (wb_clk_i)
- No clock gating (not supported in current flow)
- Clock enables used for power management

### 4. Reset Strategy
- Synchronous active-high reset (Caravel standard)
- Reset synchronizer at top level

### 5. GPIO Allocation
- Timer outputs (PWM) connected to mprj_io
- Exact pad assignments TBD based on timer features

## Requirements Verification Matrix

| Requirement | Status | Notes |
|------------|--------|-------|
| 2x Timer peripherals | 🚧 In Progress | Using CF_TMR32 IP |
| Wishbone interface | 📋 Planned | B4 Classic protocol |
| Address decode | 📋 Planned | 64KB windows each |
| IRQ support | 📋 Planned | user_irq[2:0] |
| Functional verification | 📋 Planned | cocotb tests |
| Documentation | 🚧 In Progress | Templates created |

**Legend**: ✅ Complete | 🚧 In Progress | 📋 Planned | ❌ Blocked

## Dependencies

- CF_TMR32 IP core (v1.1.0) - Pre-installed at /nc/ip
- Caravel user project template
- ipm_linker tool for IP management
- cocotb verification framework
- caravel-cocotb test infrastructure

## Getting Started

### Prerequisites
- Access to /nc/ip directory with CF_TMR32
- OpenLane 2 environment
- cocotb and caravel-cocotb installed

### Quick Start
(To be updated as project progresses)

## References

- Caravel Documentation: https://caravel-harness.readthedocs.io/
- Wishbone B4 Specification
- CF_TMR32 IP Documentation: /nc/ip/CF_TMR32/docs/

## Timeline

- **Start Date**: 2025-11-01
- **Current Phase**: Project Setup
- **Target Completion**: TBD

## Contact & Support

For questions or issues related to this integration, refer to the documentation in the `docs/` directory.

---

**Last Updated**: 2025-11-01
**Version**: 0.1.0
**Status**: Project Setup Phase
