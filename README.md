# Hybrid motor and test bench control system

## Operation flow

### Test mode


### Launch mode

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
(To be confirmed) dsPIC

### Sensors

- P1: Used to measure the flow rate of the fuel to the combustion chamber
- P2: Used to measure the flow rate of the fuel to the combustion chamber
- P3: Pressure inside the fuel tank
- P4: Pressure inside the combustion chamber
- C1: Continuity test of the igniter
- F1: (test mode only) The load cell that measure the force of the motor during tests

### Actuators

- A: First valve to control the filling of the fuel tank
- B: Purge valve for the fill line of the fuel tank
- C: Second valve to control the filling of the fuel tank
- D: Disconnect control of the fill line from the fuel tank
- E: Purge valve for the fuel tank
- F: Valve to control the fuel flow to the combustion chamber


## Misc notes

- pistons that disconnect the fill line from the fuel tank must retain their state unless it is changed to avoid a change of state when the line is disconnected and signals don't pass anymore. However, that same piston should ideally not remain in the "disconnect" state too long
- the control of the pistons must be in the tower (fill line) because they are outside the rocket
- should have a second MCU to synthetize rocket packets from rocket data from avionics and motor control data from the motor control system

