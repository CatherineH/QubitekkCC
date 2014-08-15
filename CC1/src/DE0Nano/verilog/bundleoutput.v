module bundleoutput(

	input wire [21:0]GPs,
	input CLOCK_50,
	output [21:0] FPGA
);
reg [21:0] data;

always @(posedge CLOCK_50)
begin
	data[21:0] <=GPs[21:0];
end

assign FPGA[21:0] = data[21:0];
endmodule