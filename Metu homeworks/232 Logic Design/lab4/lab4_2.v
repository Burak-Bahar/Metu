`timescale 1ns / 1ps
module IntersectionSimulator(
	input [2:0] mode, //1xx:display, 000:remA, 001:remB, 010:addA, 011:addB
	input [4:0] plateIn,
	input action,
	input clk,
	output reg  greenForA,
	output reg  greenForB,
	output reg 	rushHourWarning,
	output reg [3:0]  hour,
	output reg [5:0]  minute,	
	output reg [5:0]  second,
	output reg	am_pm, //0:am, 1:pm
	output reg [4:0]  numOfCarsA,
	output reg [4:0]  numOfCarsB,
	output reg [6:0]  remainingTime,
	output reg [4:0]  blackListDisplay
	);
	
	//You may write your code anywhere
	reg [4:0] plateA [0:29];
	reg [4:0] plateB [0:29];
	reg [4:0] blacklist [0:29];
	reg [6:0] redlightA;
	reg [6:0] redlightB;
	reg B; //b green
	reg [4:0] roada;
	reg [4:0] roadb;
	reg [4:0] blackl;
	reg [4:0] rema;
	reg [4:0] remb;
	integer i;
	reg change;
	integer a;
	initial begin
		greenForA=1;
		greenForB=0;
		rushHourWarning=0;
		hour=6;
		minute=0;
		second=0;
		am_pm=0;
		numOfCarsA=0;
		numOfCarsB=0;
		remainingTime= 50;
		blackListDisplay=0;
		redlightA = 40;
		redlightB = 50;
		B=0;
		//...
		roada=0;
		roadb=0;
		blackl=0;
		rema=0;
		remb=0;
		change =0;
	end


	always@(posedge action)
	begin
		if(mode == 3'b000)begin	//remove from road A make the head move
			if(numOfCarsA > 0) begin numOfCarsA = numOfCarsA-1; rema = rema+1; end
			if(rema == 30)begin rema =0;end //return to first place to look for further heads
			if(greenForA ==0 && blackl<30)begin // add to black list 
				blacklist[blackl][4:0] =  plateA[rema][4:0]; blackl = blackl+1;
			end
		end
		else if(mode == 3'b001)begin // remove from road B make the head move
			if(numOfCarsB > 0) begin numOfCarsB = numOfCarsB-1; remb = remb+1;end
			if(remb == 30)begin remb =0; end //return to first place to look for further heads
			if(greenForB ==0 && blackl<30)begin // add to black list 
				blacklist[blackl][4:0] = plateB[remb][4:0]; blackl = blackl+1;
			end
		end
		else if(mode == 3'b010 && plateIn != 0)begin //add car to road A and plate 
			if(numOfCarsA < 30) begin numOfCarsA = numOfCarsA+1; plateA[roada][4:0] = plateIn; roada=roada+1;
				if(roada == 30)begin roada=0; end //return to start from end
			end
		end
		else if(mode == 3'b011 && plateIn != 0)begin  //add car to road B and plate
			if(numOfCarsB < 30) begin numOfCarsB = numOfCarsB+1; plateB[roadb][4:0] = plateIn; roadb=roadb+1; 
				if(roadb == 30)begin roadb=0; end //return to start from end
			end
		end
		/**/if(hour ==12 && minute ==0 && second== 0 && am_pm==0)begin 
				for(a=0; a < 30 ; a=a+1)begin 
					blacklist[a] = 0;
				end
			end
		
	end


	always @(posedge clk)
	begin
		if(mode == 3'b100 || mode == 3'b101 || mode == 3'b110 ||mode == 3'b111 )begin //display blacklist
			if(change ==0)begin i=0; change =1;	end
			blackListDisplay[4:0] = blacklist[i][4:0];
			i=i+1;
			if(i ==30)begin
				i=0;
			end
	  
		end
		
		
		else begin //time red light busy hours
			// time
			second = second+1;
			remainingTime = remainingTime-1;
			change =0;
			if(second == 60)begin second = 0; minute = minute+1; end
			if(minute == 60)begin minute = 0; hour = hour+1; end
			if(hour ==12 && minute ==0 && second== 0) begin am_pm = ~am_pm; end //midnight midday
			if(hour ==13)begin hour = 1;  end 
			// rush hour
			if((hour==7 && minute==0 && second==0 && am_pm==0) || (hour==5 && minute==0 && second==0 && am_pm==1))begin
				rushHourWarning = 1;
			end
			if((hour==8 && minute==59 && second==59 && am_pm==0) || (hour==6 && minute==59 && second==59 && am_pm==1))begin
				rushHourWarning = 0;
			end
			
			if(hour ==12 && minute ==0 && second== 0 && am_pm==0)begin
				blackListDisplay[4:0]=0; 
			end
			// light switch
			if(greenForA==0 && greenForB==0)begin // if both red update the redlight duration for them
				if(B == 0)begin
					if(0 <= numOfCarsA <11)begin redlightA= redlightA+5; end
					else if(20 <= numOfCarsA)begin redlightA= redlightA-5; end
				end
				else if(B == 1)
					if(0 <= numOfCarsB <11)begin redlightB= redlightB+5; end
					else if(20 <= numOfCarsB)begin redlightB= redlightB-5; end
				end
				if(rushHourWarning == 1)begin
					if(redlightA < 30) begin redlightA=30; end
					else if(redlightA > 60) begin redlightA=60; end
					if(redlightB < 40) begin redlightB=40; end
					else if(redlightB > 70) begin redlightB=70;end
				end
				else begin
					if(redlightA < 40) begin redlightA=40; end
					else if(redlightA > 70) begin redlightA=70; end
					if(redlightB < 50) begin redlightB=50; end
					else if(redlightB > 80) begin redlightB=80;end
				end
				
				if(B == 1)begin greenForB=1; remainingTime = redlightA; end
				else if(B == 0)begin greenForA=1; remainingTime = redlightB; end
			end
			 // prepare for time switch 1 second later
			if(remainingTime == 0)begin // to end
				if(greenForA==1) begin B=1;end
				else if(greenForB==1) begin B=0; end
				greenForA=0; greenForB=0; 
			end
			
		end
		


endmodule
