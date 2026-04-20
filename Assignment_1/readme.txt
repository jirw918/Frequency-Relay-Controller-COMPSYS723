Instructions for running assignment on DE2-115 dev board

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
Open Run Configurations:
	Select Nios II Hardware.
	Verify the following:
		The correct executable is selected:
 			project1.elf
		Under Target Connection, ensure:
			USB Blaster (localhost) is detected.
	Click Run.

The FPGA should now be configured and the Nios II software should be successfully running on the board.
