`timescale 1ns / 1ps
module QueueCounter(
	input [7:0] llmt,
	input [7:0] ulmt,
	input en,
	input mode, // 1:delete, 0:insert
	input clk,
	output reg [7:0] head,
	output reg [7:0] tail,
	output reg empty,
	output reg full);
	
	//you may write your code here
	initial
	begin
		head = llmt;
		tail = llmt;
		empty=1;
		full=0;
	end
	//you may write your code here
	always@(posedge clk )
	begin
	if(en==1)begin
		if(mode==0)begin
			if(empty==1)begin 
			tail=tail+1; 
			empty=0; 
			end
			else if(tail==ulmt)begin
				if(head==llmt)begin 
				full=1; 
				tail=llmt; 
				end
				else begin 
				tail=llmt; 
				end
			end
			else if(full==0)begin 
				tail=tail+1; 
				if(head%tail==0 && empty==0)begin 
				full=1;
				end
			end
		end
		else if(mode==1)begin
			if(full==1)begin 
			head=head+1; 
			full=0;
			end
			else if(ulmt==head)begin
				if(tail==llmt)begin 
				empty=1; 
				head=llmt; 
				end
				else begin 
				head=llmt; 
				end
			end
			else if(empty==0)begin  
			   head=head+1;
				if(head%tail==0)begin 
				empty=1;
				end
			end
		end
	end
	end
endmodule


module Numerator(
	input clinic, 
	input mode, 
	input en, 
	input clk,
	output reg[7:0] ann,
	output reg[7:0] print);

	//write your code here
	wire [7:0]a;
	wire E,F,K,L;
	wire [7:0]b;
	wire [7:0]c;
	wire [7:0]d;
	QueueCounter cli1(5,9,en,mode,~clinic & clk,a,b,E,F);
	QueueCounter cli2(10,14,en,mode,clinic & clk,c,d,K,L);
 
	always@(*)begin
		if(clinic==0)begin 
			ann=a;
			print=b;
			end
		else if(clinic==1)begin
			ann=c;
			print=d;
			end
	end

endmodule 