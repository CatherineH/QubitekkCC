// megafunction wizard: %LPM_MUX%VBB%
// GENERATION: STANDARD
// VERSION: WM1.0
// MODULE: LPM_MUX 

// ============================================================
// File Name: counterselect.v
// Megafunction Name(s):
// 			LPM_MUX
//
// Simulation Library Files(s):
// 			lpm
// ============================================================
// ************************************************************
// THIS IS A WIZARD-GENERATED FILE. DO NOT EDIT THIS FILE!
//
// 14.0.0 Build 200 06/17/2014 SJ Web Edition
// ************************************************************

//Copyright (C) 1991-2014 Altera Corporation. All rights reserved.
//Your use of Altera Corporation's design tools, logic functions 
//and other software and tools, and its AMPP partner logic 
//functions, and any output files from any of the foregoing 
//(including device programming or simulation files), and any 
//associated documentation or information are expressly subject 
//to the terms and conditions of the Altera Program License 
//Subscription Agreement, the Altera Quartus II License Agreement,
//the Altera MegaCore Function License Agreement, or other 
//applicable license agreement, including, without limitation, 
//that your use is for the sole purpose of programming logic 
//devices manufactured by Altera and sold by Altera or its 
//authorized distributors.  Please refer to the applicable 
//agreement for further details.

module counterselect (
	data0x,
	data1x,
	data2x,
	data3x,
	sel,
	result);

	input	[20:0]  data0x;
	input	[20:0]  data1x;
	input	[20:0]  data2x;
	input	[20:0]  data3x;
	input	[1:0]  sel;
	output	[20:0]  result;

endmodule

// ============================================================
// CNX file retrieval info
// ============================================================
// Retrieval info: PRIVATE: INTENDED_DEVICE_FAMILY STRING "Cyclone IV E"
// Retrieval info: PRIVATE: SYNTH_WRAPPER_GEN_POSTFIX STRING "0"
// Retrieval info: PRIVATE: new_diagram STRING "1"
// Retrieval info: LIBRARY: lpm lpm.lpm_components.all
// Retrieval info: CONSTANT: LPM_SIZE NUMERIC "4"
// Retrieval info: CONSTANT: LPM_TYPE STRING "LPM_MUX"
// Retrieval info: CONSTANT: LPM_WIDTH NUMERIC "21"
// Retrieval info: CONSTANT: LPM_WIDTHS NUMERIC "2"
// Retrieval info: USED_PORT: data0x 0 0 21 0 INPUT NODEFVAL "data0x[20..0]"
// Retrieval info: USED_PORT: data1x 0 0 21 0 INPUT NODEFVAL "data1x[20..0]"
// Retrieval info: USED_PORT: data2x 0 0 21 0 INPUT NODEFVAL "data2x[20..0]"
// Retrieval info: USED_PORT: data3x 0 0 21 0 INPUT NODEFVAL "data3x[20..0]"
// Retrieval info: USED_PORT: result 0 0 21 0 OUTPUT NODEFVAL "result[20..0]"
// Retrieval info: USED_PORT: sel 0 0 2 0 INPUT NODEFVAL "sel[1..0]"
// Retrieval info: CONNECT: @data 0 0 21 0 data0x 0 0 21 0
// Retrieval info: CONNECT: @data 0 0 21 21 data1x 0 0 21 0
// Retrieval info: CONNECT: @data 0 0 21 42 data2x 0 0 21 0
// Retrieval info: CONNECT: @data 0 0 21 63 data3x 0 0 21 0
// Retrieval info: CONNECT: @sel 0 0 2 0 sel 0 0 2 0
// Retrieval info: CONNECT: result 0 0 21 0 @result 0 0 21 0
// Retrieval info: GEN_FILE: TYPE_NORMAL counterselect.v TRUE
// Retrieval info: GEN_FILE: TYPE_NORMAL counterselect.inc FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL counterselect.cmp FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL counterselect.bsf TRUE
// Retrieval info: GEN_FILE: TYPE_NORMAL counterselect_inst.v FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL counterselect_bb.v TRUE
// Retrieval info: LIB_FILE: lpm