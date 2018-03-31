# phoenix-arduino

Convert a Gravis Phoenix Joystick to USB using an Arduino

The Gravis Phoenix is a 4-axis programmable Joystick with 24 buttons. Up to 4 buttons could be programmed to behave as standard joystick buttons, while the remainder could behave as if a key was pressed on the keyboard. To accomplish this, the joystick connected via both the 15 pin joystick port and a AT or PS/2 keyboard passthrough.
![Gravis Phoenix](images/gravis_phoenix.jpg "Gravis Phoenix")

It's no longer practical to use this joystick with modern computers due to the lack of a joystick port. Even USB joystick adapters do not make it possible to use the DOS boot disk configuration utility in order to reprogram the buttons.

This project will investigate removing the main logic board of the phoenix and replacing it with an Arduino which will also act as a USB input device. The [UnoJoy](https://github.com/AlanChatham/UnoJoy) project includes both Arduino code and host drivers in order to send axes positions and button states to a computer.

The initial focus will be on understanding the Phoenix's circuitry in order to make the proper connections and readings with the Arduino. To do this, I disassembled the joystick in order to investigate its components.

## Components

![Gravis Phoenix Exposed Bottom View](images/phoenix_bottom_open.jpg)

The Phoenix contains 5 Printed Circuit Boards (PCBs)
1. Micro PCB: Connects to the computer via joystick and keyboard connectors. Also connects to the Slider PCB via 6 pins and Wing PCB via 10 pins. This is the part we will remove and replace with an Arduino to modernize the joystick.

2. Slider PCB: Contains 2 potentiometers (X and Y axes) and a RJ11 connector which connects to the Handle PCB.
![Slider PCB](images/slider_pcb.png)

3. Handle PCB: Contains 8 buttons and connects to the Slider PCB via a RJ11 cable. Uses a HC165 8-bit parallel load shift register to make 8 button states available over the 4 wire connection.
![Handle Top](images/handle_disassembly1.png)
![Handle PCB w/ Buttons](images/handle_disassembly2.png)
![Handle PCB](images/handle_disassembly3.png)
![Handle PCB Diagram](images/handle_pcb.png)

4. Wing PCB: Contains 8 buttons and 2 potentiometers to read the throttle and rudder axes. Connects to the Throttle PCB via a 4 wire ribbon cable. Uses a HC165 8-bit parallel load shift register to allow the Micro PCB to read the button states.
![Wing PCB](images/wing_pcb.png)

5. Throttle PCB: Contains 8 buttons. I have not opened this component to map out the traces, but it's extremely likely that it's functionally the same as the Handle PCB with a different layout. This means another HC165 8-bit parallel load shift register.
![Wing and Throttle](images/wing_throttle.png)

## HC165

Since the Phoenix uses 3 HC165 chips, it will be important to understand how it operates. The [SN74HC165 Data Sheet](images/SN74HC165.pdf) shows the pinout and describes the operation. There is also an [HC165 Arduino code sample](https://playground.arduino.cc/Code/ShiftRegSN74HC165N) which is available for experimentation. However, note in the Handle PCB that the CLK and Q_H(read) pins are actually connected to the same wire. Presumably this is a cost saving measure.

Briefly setting the SH/!LD pin LOW will cause the HC165 to load 8 bits in parallel from pins A-H. When SH/!LD is set HIGH, values on A-H is ignored and the value from H is immediately available on Q_H. At this point, each LOW to HIGH transition on CLK/Q_H is seen by the CLK pin and causes the HC165 to make the next data bit available (G,F,...). If the button is not pressed, the signal will remain HIGH. If the button is pressed, the voltage will be forced LOW, which is ok since it won't be interpreted as a clock transition. Here is a sample oscilloscope trace of the CLK/READ wire of the Handle/Slider PCBs while connected to the Micro PCB and powered via the joystick port. In this case the 5th button was pressed, showing substantially longer LOW voltage during that time.
![HC165 Clock/Read Oscilloscope Trace](images/clock_read_oscilloscope.png)
