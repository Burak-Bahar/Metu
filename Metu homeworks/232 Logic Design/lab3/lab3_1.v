`timescale 1ns / 1ps
module kb(
	input K,
	input B,
	input clk,
	output reg Q
	);
    
	initial begin
		Q = 0;
	end
	 
	// write your code here
	always@(posedge clk)
	begin
	if(~K&~B)begin Q = ~Q; end
	else if(~K&B) begin Q = 0; end
	else if(K&~B) begin Q = 1; end
	else if(K&B) begin Q = Q; end
	end
endmodule


module ic232(
	input A0, 
	input A1, 
	input A2, 
	input clk,
	output Q0,
	output Q1,
	output Z
	);
	// write your code here
	kb a(~A2 & ~(A0 | A1),A1,clk,Q0);
	kb b(~A2,A2 ^ (~A0 |A1),clk,Q1);
	assign Z = (Q1^~Q0);
endmodule
