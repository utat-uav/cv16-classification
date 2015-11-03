function [BH,mean_dist]=sc_compute(Bsamp,Tsamp,mean_dist,nbins_theta,nbins_r,r_inner,r_outer,out_vec);
% Bsamp = X'1/2 
% Tsamp = empty array of length X1/2 transposed (row 1xn)
% mean_dist = empty array, inputted
% nbins_theta, nbins_r, r_inner, r_outer = parameters from CompareContext
% out_vec = empty array (out1/2) of length X1/2 (row 1xn)

% Outputs: BH - 
%          mean_dist - mean distance, used for length normalization
%                      if not supplied, then it is computed from the data


% [BH,mean_dist]=sc_compute(Bsamp,Tsamp,mean_dist,nbins_theta,nbins_r,r_inner,r_outer,out_vec);
%
% compute (r,theta) histograms for points along boundary 
%
% Bsamp is 2 x nsamp (x and y coords.)
% Tsamp is 1 x nsamp (tangent theta)
% out_vec is 1 x nsamp (0 for inlier, 1 for outlier)
%
% mean_dist is the mean distance, used for length normalization
% if it is not supplied, then it is computed from the data
%
% outliers are not counted in the histograms, but they do get
% assigned a histogram


% nsamp stores the number of columns in X
nsamp=size(Bsamp,2);

% If a 0 is inputted, it sets in_vec to 1, else 0
% From CompareContext, all of them are 0, so in_vec will always = 1
in_vec=out_vec==0;

% Polarizes the X array

% compute r,theta arrays (real component only)
% Calls dist2 function using the transposed Bsamp (X) input twice, returns
% an array containing the squared distance between the sets of points, of
% the form (x.^2)' + x.^2 - 2x*x' -> effectively seeing xij*xji - (xij)^2
% since (x.^2)' + x.^2 forms a symmetric matrix

% Takes the magnitude distance of something...
r_array=real(sqrt(dist2(Bsamp',Bsamp'))); % real is needed to
                                          % prevent bug in Unix version
                     
% Creates an array for the theta (atan(y/x))                                        
%                atan ---------------------y values ------------------- / ------------------ x values ---------------------                                        
theta_array_abs=atan2(Bsamp(2,:)'*ones(1,nsamp)-ones(nsamp,1)*Bsamp(2,:),Bsamp(1,:)'*ones(1,nsamp)-ones(nsamp,1)*Bsamp(1,:))';

% OK STOP WHY????
% So Tsamp is literally still a vector of 0's. Meaning you are assigning
% theta_array to be theta_array_abs - 0*1.... why is this needed....

% THIS LINE LITERALLY DOES NOTHING BUT DUPLICATE AND THEN RENAME THE
% VARIABLE FROM THE PREVIOUS LINE... NEED TO TEST TO MAKE SURE, BUT SHOULD
% PROBABLY GET RID OF IT IF NOTHING IS GOING ON HERE SINCE IT IS A MASSIVE
% WASTE OF MEMORY AND TIME AS IT HAPPENS TWICE PER IMAGE PER TEST SHAPE
theta_array=theta_array_abs-Tsamp'*ones(1,nsamp);

% create joint (r,theta) histogram by binning r_array and
% theta_array

% normalize distance by mean, ignoring outliers
if isempty(mean_dist)
   tmp=r_array(in_vec,:);
   tmp=tmp(:,in_vec);
   mean_dist=mean(tmp(:));
end

% r_array_n is now the normalized version of r_array
r_array_n=r_array/mean_dist;

% use a log. scale for binning the distances
% Believe his helps to create a noticeable difference between the likeliest
% match and the rest of the pack (creates exponential base using logspace
% to do this, sort of)
% This actually indexes 5 points between (log10(1/8) and log10(2)
r_bin_edges=logspace(log10(r_inner),log10(r_outer),5);
% Create an n by n array of zeros, where n is the size of logical array X
r_array_q=zeros(nsamp,nsamp);
for m=1:nbins_r
   % Iteratively increments the zero array by a boolean value as to whether
   % or not the normalized magnitude distance array (rarray_n) is less than
   % the log value of an indexed value between r_inner and r_outer 
   % (weighted) index. The indexed value increments through each iteration, 
   % so later ones have a larger impact than greater ones. Higher values
   % indicate that the normalized matrix is upper bounded by the weighted
   % index range (TRYING TO FIGURE OUT EXACTLY WHAT THIS MEANS...)
   
   % In this example: 
   %    n_bins_r = 5;
   %    r_inner = 1/8;
   %    r_outer = 2;
   r_array_q=r_array_q+(r_array_n<r_bin_edges(m));
end
% If the normalized value is less than log10(r_inner [this is lower bound]))
% then fz will be 0. This means it was too small to enter the range. 
% Otherwise it is true.
fz=r_array_q>0; % flag all points inside outer boundary

% put all angles in [0,2pi) range
theta_array_2 = rem(rem(theta_array,2*pi)+2*pi,2*pi);
% quantize to a fixed set of angles (bin edges lie on 0,(2*pi)/k,...2*pi
theta_array_q = 1+floor(theta_array_2/(2*pi/nbins_theta));

nbins=nbins_theta*nbins_r;
BH=zeros(nsamp,nbins);
for n=1:nsamp
   fzn=fz(n,:)&in_vec;
   Sn=sparse(theta_array_q(n,fzn),r_array_q(n,fzn),1,nbins_theta,nbins_r);
   BH(n,:)=Sn(:)';
end