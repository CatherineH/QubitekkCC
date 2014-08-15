QubitekkCC
==========

This is a repository for the firmware for the Qubitekk Coincidence Counter. All code in this repository is released under the MIT license. Qubitekk has chosen to release the firmware and many details on the assembly of the device in order to facilitate the modification of the device for specific experimental needs.

The Qubitekk Coincidence Counter uses a Terasic DE0-Nano FPGA development board and a Digi Rabbit 3400 Microprocessor. Instructions on how to compile and transfer firmware can be found in the appendix of the user manual for the CC1 - CC1/doc/Qubitekk_CC1_usermanual.pdf  

Coincidence Counter (CC1)
==========

The unit ships with the Coincidence Counter firmware. This firmware allows for counting the detection signals on two channels, as well as the coincidences between those channels to windows up to 7 ns. The unit can be interacted with through the buttons and LCD screen, or through the USB port through a virtual COM port. 

This device uses a SCPI-compatible communication protocol. A python driver for this protocol has been incorporated into the InstrumentKit project (available at https://github.com/Galvant/InstrumentKit).

Time Difference Histogram (TDH)
==========

The Time Difference Histogram is an alternate version of the firmware. Instead of counting singles and coincidences, it produces histograms of the time differences between detection signals. This is a work in progress.


