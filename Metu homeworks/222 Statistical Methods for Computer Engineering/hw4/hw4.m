N=21025;      % size of the Monte Carlo simulation with alpha = 0.2 and error = 0.008
p=0.62;
q=0.38; 
TotalWeight=zeros(N,1); % a vector that keeps the total weight of the caught fish for each Monte Carlo run
for k=1:N;
	U = rand; i = 0; % first generate sample Y, the number of chunk produced in 5 day using sampling from Binomial
	weekweight=0;
    for day=1:5; %week days
		U = rand;
		F = nchoosek(50,0)*(q^(50));
		i=0;
		while (U>=F)
			i=i+1;
			F = F+(nchoosek(50,i)*(p^(i))*(q^(50-i)));
        end;
		Y = i;   % Y is the number of chunk
		weight = 0; % total weight of chunk for this day
	
		% now sample Y chunk weights using rejection sampling
		s = 0; t = 8; m = 0.32;  % the boundary of the rejection sampling rectangle
		for f=1:Y;
			CX = 0; CY = m; % initial coordinates for rejection sampling
			F = 0; % initial F at CX = 0
			while (CY > F);
				U = rand; V = rand;
				CX = s+(t-s)*U; CY = m*V;
			
				if CX <=2
					F = 0.07*CX;
				elseif CX <=5
					F = -0.02*((CX-4)^2) + 0.22;
				elseif CX <=7
					F = 0.08*(5-CX) + 0.2;
				else
					F = -0.04*CX + 0.32;
				end
            end;
			weight = weight + CX;	
        end;
	  	weekweight = weekweight + weight;
    end;
	TotalWeight(k) = weekweight;
end;
p_est = mean(TotalWeight>640);
expectedWeight = mean(TotalWeight);
stdWeight = std(TotalWeight);
fprintf('Estimated probability = %f\n',p_est);
fprintf('Expected weight = %f\n',expectedWeight);
fprintf('Standard deviation = %f\n',stdWeight);
