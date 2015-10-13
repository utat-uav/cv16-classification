function name = AnalyzeTarget( TargetIm , CHANNEL ,thresh)
% Channel is 1, 2, or 3

HSV = rgb2hsv(TargetIm);
intensity = HSV(:,:,CHANNEL);

% Remove First Dominant Region - Either background or Target Shape
[hist, values] = imhist(intensity);  %Calculate background 
[frq,maxind] = max(hist);
bckgrd = values(maxind);
foreground = abs(double(intensity) - bckgrd);                      
bin = foreground>thresh;
bwedge = edge(bin,'sobel',0);
[x y] = find(bwedge>0);

%Extract shape profile
Xsh = [x y];
imshow(bin);
figure


