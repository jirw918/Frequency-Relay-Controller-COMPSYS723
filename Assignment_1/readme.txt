
Frequency Relay Controller (FPGA + Nios II)
**************************************************************************************

For Compys723 Assignment 1 UoA
Authors:
	Jevaan Irwin
	Sarthak Negi 

**************************************************************************************
1. Overview

This project implements a frequency relay controller on an FPGA platform using a Nios II soft-core processor. The system monitors an input signal, performs frequency analysis, and responds based on predefined conditions.

User interaction and system feedback are provided via:

VGA output for visual display
PS/2 input for user control
Push buttons for hardware-level interaction

The design integrates both hardware (Quartus) and embedded software (Nios II Eclipse) components.

**************************************************************************************
2. System Architecture

The system consists of a combination of FPGA-based hardware peripherals and embedded software running on the Nios II processor.

Key Components
Nios II Processor
Executes the main control logic
Handles peripheral communication
PIO (Parallel I/O) Interfaces
Push button inputs
External signal interfacing
Interrupt System
Handles real-time events (e.g., frequency detection, button presses)
VGA Controller
Displays system status and outputs
PS/2 Interface
Enables keyboard-based user input
Operation Summary
Input signal is monitored via FPGA peripherals
Frequency-related data is processed by the Nios II processor
Interrupts trigger real-time responses
Results are displayed via VGA and/or acted upon by the controller

**************************************************************************************
3. Hardware Requirements
FPGA development board (compatible with Quartus Prime 18.1)
USB Blaster programming cable
VGA display
VGA-to-HDMI converter (if required, must be powered)
PS/2 keyboard
Power supply for FPGA board

**************************************************************************************
4. Software Requirements
Quartus Prime 18.1
Nios II Embedded Design Suite (EDS)
Project files provided in this repository

	Note: Ensure the project directory path contains no spaces to avoid toolchain issues.

**************************************************************************************
5. Instructions for running assignment on DE2-115 dev board

1. Hardware Setup

Ensure all required peripherals are connected before powering the board:
	Connect the USB Blaster cable to the appropriate port on the FPGA board and host computer.
	Connect the VGA cable to the FPGA board.
		If using a VGA-to-HDMI converter, ensure the converter is powered.
	Connect the PS/2 cable (if required for input).
	Connect the power supply to the FPGA board.

2. Software Preparation

Download the project .zip file.
Extract the contents to a directory without spaces in the file path

3. Programming the FPGA (Quartus)

Open Quartus Prime 18.1 Programmer.
	Add the programming file:
		freq_relay_controller.sof
	Under Hardware Setup:
		Select USB Blaster.
	Click Start to program the FPGA.

Keep the Programmer open after configuration (do not close it).

4. Running the Nios II Software (Eclipse)

Open Nios II Eclipse.
	Set the workspace directory to the project’s software subfolder.

Rebuild BSP
	right click project1.bsp, Nios II-> Generate BSP
Rebuild Project
	Select Project -> Build All (Ctrl + B)

Open Run Configurations:
	Select Nios II Hardware.
	Verify the following:
		The correct executable is selected:
 			project1.elf
		Under Target Connection, ensure:
			USB Blaster (localhost) is detected.
	Click Run

The FPGA should now be configured and the Nios II software should be successfully running on the board.

**************************************************************************************
7. Usage

Once the system is running:

The VGA display will show system output and status
The PS/2 keyboard allows user interaction (if implemented)
Push buttons may trigger hardware-level events or interrupts

Frequency threshold can be configured with the keyboard:
	Q: decrements the frequency threshold by 0.5Hz
	A: Increments the frequency threshold by 0.5Hz

	The Frequency threshold can be configured from 45Hz to 52Hz

ROC threshold can be congpfigured with the keyboard:
	W: Decrements the ROC threshold by 0.5. 
	S: Increments the ROC threshold by 0.5. 


