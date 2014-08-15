module ChanBundle(
	input wire CH_0,
	input wire CH_1,
	input wire CLOCK_50,
	output [1:0]CH
	
);
reg [1:0] data;
always @(posedge CLOCK_50)
begin
	data[0] <= CH_0;
	data[1] <= CH_1;
end

assign CH[1:0] = data[1:0];

endmodule