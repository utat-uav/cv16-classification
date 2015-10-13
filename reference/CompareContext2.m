function sc_cost = CompareContext(TargetIm, RefImg)
%% Parameters
theta_bins = 12;
r_bins = 5;
r1 = 1/8;
r2 = 2;
e = 0;

%% Extract Contours
[x y t ~] = bdry_extract_3(TargetIm);
[xref yref tref ~] = bdry_extract_3(RefImg);
X1 = [x y];
X2 = [xref yref];
n1 = length(X1);
n2 = length(X2);

[BH1,~]=sc_compute(X1',t',[],theta_bins,r_bins,r1,r2,zeros(1,n1));
[BH2,~]=sc_compute(X2',tref',[],theta_bins,r_bins,r1,r2,zeros(1,n2));

%% Shape Distance
 costmat=hist_cost_2(BH1,BH2);
 if (0)
    n = max(n1,n2);
    costmat2 = e*ones(n,n);
    costmat2(1:n1, 1:n2) = costmat;
    [~,sc_cost] = hungarian(costmat2);
    sc_cost = sc_cost/n;
 else
   [a1,~]=min(costmat,[],1);
   [a2,~]=min(costmat,[],2);
   sc_cost=max(mean(a1),mean(a2));
 end