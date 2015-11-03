function sc_cost = CompareContext(X1, file2)
% Takes as input X1 (array of contour points - X in main fcn), and file2 - 
% the iterated test file for reference. Outputs

%% Parameters
theta_bins = 12;
r_bins = 5;
r1 = 1/8;
r2 = 2;
e = 0;

%% Extract Contours
% Don't need this line - it is already done to the X1 array inputted
% Also there is no file1 - this was likely replaced by the eventual X1 with
% contour already done, and then forgotten about
%X1 = ContourPoints(file1);

% Goes through X1, creates arrays T1 and out1 that get sent to sc_compute
n1 = length(X1);
T1 = zeros(n1,1);
out1 = zeros(1,n1);

% Pushes file2 through the ContourPoints function and creates an output
% array X2 (similar to X1, but for the reference file)
% Goes through X2, creates arrays T2 and out2 that get sent to sc_compute

% NOTE: This will make everything much slower when testing multiple files
% as it has to do ContourPoints many times for each reference file. Having
% it do this in the main function once and store the results for easy
% access might save a lot of time
X2 = ContourPoints(file2);
n2 = length(X2);
T2 = zeros(n2,1);%T2 = pi/2*ones(n2,1);
out2 = zeros(1,n2);


% Pushes each transposed contour array (X1, X2), the constant parameters, 
% and the T1/2 (transposed) and out1/2 empty arrays through to sc_compute 
% function. The returned values, stored in BH1 and BH2 for 1 and 2 
% respectively, are 
[BH1,~]=sc_compute(X1',T1',[],theta_bins,r_bins,r1,r2,out1);
[BH2,~]=sc_compute(X2',T2',[],theta_bins,r_bins,r1,r2,out2);

%% Shape Distance
 costmat=hist_cost_2(BH1,BH2);
 n = max(n1,n2);
 costmat2 = e*ones(n,n);
 costmat2(1:n1, 1:n2) = costmat;

 % WHY IS THIS FUNCTION COMMENTED OUT?
%[~,sc_cost] = hungarian(costmat2);

% sc_cost = sc_cost/n;
 % calculate shape context cost
   [a1,b1]=min(costmat,[],1);
   [a2,b2]=min(costmat,[],2);
   sc_cost=max(mean(a1),mean(a2));