module stopstartpulse (startclock, stopclock, pulse );

input startclock;
input stopclock;

output pulse;

reg state;
reg data;

always @ (posedge startclock or posedge stopclock)
begin
	if(startclock)
	begin
		data <= 1'b1;
	end
	if(stopclock)
	begin
		data <= 1'b0;
	end
end

/*
always @ (posedge stopclock)
begin
	data <= 1'b0;		
end
*/
assign pulse = data;
endmodule