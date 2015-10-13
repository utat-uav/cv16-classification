function sc_cost = CompareContext(X1, file2)
%% Parameters
theta_bins = 12;
r_bins = 5;
r1 = 1/8;
r2 = 2;
e = 0;

%% Extract Contours
%X1 = ContourPoints(file1);

n1 = length(X1);
T1 = zeros(n1,1);
out1 = zeros(1,n1);

X2 = ContourPoints(file2);
n2 = length(X2);
T2 = zeros(n2,1);%T2 = pi/2*ones(n2,1);
out2 = zeros(1,n2);


[BH1,~]=sc_compute(X1',T1',[],theta_bins,r_bins,r1,r2,out1);
[BH2,~]=sc_compute(X2',T2',[],theta_bins,r_bins,r1,r2,out2);

%% Shape Distance
 costmat=hist_cost_2(BH1,BH2);
 n = max(n1,n2);
 costmat2 = e*ones(n,n);
 costmat2(1:n1, 1:n2) = costmat;

%[~,sc_cost] = hungarian(costmat2);

% sc_cost = sc_cost/n;
 % calculate shape context cost
   [a1,b1]=min(costmat,[],1);
   [a2,b2]=min(costmat,[],2);
   sc_cost=max(mean(a1),mean(a2));