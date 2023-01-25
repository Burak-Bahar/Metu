`timescale 1ns / 1ps

module ROM (
input [2:0] addr, 
output reg [7:0] dataOut);

	// write your code below
	always@(*)
	begin
	if(addr== 3'b000)begin dataOut= 8'b00000000; end
	else if(addr==3'b001)begin dataOut= 8'b01010101; end
	else if(addr==3'b010)begin dataOut= 8'b10101010; end
	else if(addr==3'b011)begin dataOut= 8'b00110011; end
	else if(addr==3'b100)begin dataOut= 8'b11001100; end
	else if(addr==3'b101)begin dataOut= 8'b00001111; end
	else if(addr==3'b110)begin dataOut= 8'b11110000; end
	else begin dataOut= 8'b11111111; end
	end
endmodule


module XOR_RAM (
input mode, 
input [2:0] addr, 
input [7:0] dataIn, 
input [7:0] mask, 
input CLK, 
output reg [7:0] dataOut);
	
	// write your code below

	reg [7:0] dat[7:0];
	initial
	begin
		dataOut = 0;
		dat[0][7:0]= 0;
		dat[1][7:0]= 0;
		dat[2][7:0]= 0;
		dat[3][7:0]= 0;
		dat[4][7:0]= 0;
		dat[5][7:0]= 0;
		dat[6][7:0]= 0;
		dat[7][7:0]= 0;
	end
	
	always@(posedge CLK) //write
	begin
		if(mode==0)begin
			if(addr== 3'b000)begin dat[0][7:0]= dataIn^mask; end
			else if(addr==3'b001)begin dat[1][7:0]= dataIn^mask; end
			else if(addr==3'b010)begin dat[2][7:0]= dataIn^mask; end
			else if(addr==3'b011)begin dat[3][7:0]= dataIn^mask; end
			else if(addr==3'b100)begin dat[4][7:0]= dataIn^mask; end
			else if(addr==3'b101)begin dat[5][7:0]= dataIn^mask; end
			else if(addr==3'b110)begin dat[6][7:0]= dataIn^mask; end
			else begin dat[7][7:0]= dataIn^mask; end
		end
	end

	always@(mode or addr ) //read
	begin
	if(mode==1)begin
   	if(addr== 3'b000)begin dataOut= dat[0][7:0]; end
		else if(addr==3'b001)begin dataOut= dat[1][7:0]; end
		else if(addr==3'b010)begin dataOut= dat[2][7:0]; end
		else if(addr==3'b011)begin dataOut= dat[3][7:0]; end
		else if(addr==3'b100)begin dataOut= dat[4][7:0]; end
		else if(addr==3'b101)begin dataOut= dat[5][7:0]; end
		else if(addr==3'b110)begin dataOut= dat[6][7:0]; end
		else begin dataOut= dat[7][7:0]; end
	end
	end
		
	
endmodule


module EncodedMemory (
input mode, 
input [2:0] index, 
input [7:0] number, 
input CLK, 
output [7:0] result);
	
	// DO NOT EDIT THIS MODULE
	
	wire [7:0] mask;
	
	ROM R(index, mask);
	XOR_RAM XR(mode, index, number, mask, CLK, result);

endmodule
