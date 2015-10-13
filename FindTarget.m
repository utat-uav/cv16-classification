function [n blobs] = FindTarget(Im,thresh, CHANNEL)
%% Color Conversion, Background and Threshold
disp('Image Proc Begin');
tic
%% Convert to HSV space and choose channel
HSV = rgb2hsv(Im);
intensity = HSV(:,:,CHANNEL);

%% Remove dominant background region
[hist, values] = imhist(intensity);  %Calculate background 
[frq,maxind] = max(hist);
bckgrd = values(maxind);
foreground = abs(double(intensity) - bckgrd);  
bin = foreground>thresh;
%%
disp('Image Proc End');
toc


%% Blob Analysis
[l w ~] = size(Im);
area = l*w;
minArea = 0.0001; %Min area
maxArea = 0.05; 
maxBlobs = 10;
maxAR = 4;

hblob = vision.BlobAnalysis('MaximumCount', maxBlobs,'ExcludeBorderBlobs', true,'MinimumBlobArea',round(minArea*area), 'MaximumBlobArea', round(maxArea*area),'Connectivity',8);
[~, ~, BBox] = step(hblob, bin);

%Quick filter
AR = double(BBox(:,4))./ double(BBox(:,3));        % Screen Blobs based on Aspect Ratio
ind = find(AR < maxAR & AR >1/maxAR);
n = length(ind);
blobs = BBox(ind,1:4);
