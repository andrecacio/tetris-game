# Embedded Tetris (ARM Cortex-M3 / LPC17xx)

A bare-metal C implementation of a Tetris-like game engine running directly on an NXP LPC1768 microcontroller. This project demonstrates low-level hardware interaction without an underlying operating system.

## ⚙️ Technical Highlights
- **Hardware Interrupts:** Custom ISRs (Interrupt Service Routines) for hardware timers (TIMER1) and Repetitive Interrupt Timer (RIT) for strictly timed game physics and debounced input polling.
- **Register-Level Programming:** Direct manipulation of memory-mapped registers (e.g., `LPC_GPIO`, `LPC_PINCON`, `LPC_ADC`) for GPIO configuration and peripheral control.
- **Analog Peripherals:** - Real-time game speed modulation using the built-in ADC (Analog-to-Digital Converter) via hardware polling.
  - Sound synthesis via DAC (Digital-to-Analog Converter) for game events.
- **Memory & Performance Optimization:** Efficient use of static arrays and bitwise operations to maintain game state and render the GLCD interface within the tight memory constraints of the MCU.

## 🛠️ Stack & Tools
- **Language:** C (Bare-metal)
- **Hardware:** NXP LPC17xx (ARM Cortex-M3)
- **Peripherals Interfaced:** GLCD (Graphic LCD), Joystick, Pushbuttons, Potentiometer (ADC), Speaker (DAC).
