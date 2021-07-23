# Arduino-MIGMAG-Welder
Control for transformer welder based on Arduino Uno


# Features
- Adjustable Gas pre-, postflow
- Adjustable wire feeder speed
- Switch for dot welding
- Adjustable duration for dot weld and pause in between

# Original hardware configuration
- Migatronic MIG 300 Welder
- PWM Wirefeed
- Arduino UNO
- 5 Analog inputs (Pre-, Postflow; Wire speed; Dotting duration, Dotting pause)
- 2 Digital inputs (Button on welding gun, Switch for switching dot welding)
- 3 Digital Outputs (Main contactor, Gas valve, Motor control)

main contactor, gas valve run on 25V AC, Wirefeed motor at 24V DC. Usage of relays for 25V AC, MOSFET board for PWM motor control
