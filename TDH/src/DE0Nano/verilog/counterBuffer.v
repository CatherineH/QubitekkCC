module counterBuffer(input [20:0]counter,input enablepulse, output [20:0]q,output clear);

reg [20:0] data;
reg clear_tmp=1'b0;

always @ (negedge enablepulse)
begin
	data[20:0]<=counter[20:0];
	clear_tmp <=1'b1;
end
assign clear = clear_tmp;
assign q = data;
endmodule