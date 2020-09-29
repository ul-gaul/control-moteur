# Hybrid motor and test bench control system

## Operation flow

1. B off, A and C open
2. if fuel tank is filled and pressure >= 750 psi, go to 3, else go to 1
3. C and A are off
4. wait for launch signal
5. D is activated to disconnect the fill line from the fuel tank
6. Igniter is ignited
7. Wait for a delay
8. F is open
9. Rocket is launched

## Components

### Processing units

The processing units chosen were the PIC32MZ2048.

The requirements for the motor control MCU are as follows:

- 1 SPI bus, for the SD card
- At least 6 GPIOs, probably a couple more to be safe
- 1 UART bus to send data to the launch pad
- Interrupt support on the UART bus if commands are to be received
	from the control station
- FATfs support
- at least 4KB of RAM
- 4 ADC
- (bus for the load cell)
- (GPIOs for the continuity test)


The requirements for the RF communication management MCU are as follows:
- 2 UART bus, 3 would be nice though
- Interrupt support on two UART ports, maybe needed on the third one if
	commands are to be received from the control station.
- Good error detection capabilities, CRC would be best


### Sensors

Note: pressure sensors output an analog signal that has a range of 0 - Vref V.
This means the maximum output value is equal to the Vdd of the sensor.

- P1: Used to measure the flow rate of the fuel to the combustion chamber
- P2: Used to measure the flow rate of the fuel to the combustion chamber
- P3: Pressure inside the fuel tank
- P4: Pressure inside the combustion chamber
- C1: Continuity test of the igniter
- F1: (test mode only) The load cell that measure the force of the motor during tests

### Actuators

The motor control system control 6 actuators:

- A: First valve to control the filling of the fuel tank
- B: Purge valve for the fill line of the fuel tank
- C: Second valve to control the filling of the fuel tank
- D: Disconnect control of the fill line from the fuel tank
- E: Purge valve for the fuel tank
- F: Valve to control the fuel flow to the combustion chamber

## Misc notes

- pistons that disconnect the fill line from the fuel tank must retain their
	state unless it is changed to avoid a change of state when the line is
	disconnected and signals don't pass anymore. However, that same piston
	should ideally not remain in the "disconnect" state too long
- the control of the pistons must be in the tower (fill line) because they
	are outside the rocket

