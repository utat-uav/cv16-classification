function name = AnalyzeTarget( TargetIm , CHANNEL ,thresh)

HSV = rgb2hsv(TargetIm);
intensity = HSV(:,:,CHANNEL);

%% Remove First Dominant Region - Either background or Target Shape
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

N = length(Xsh);
while(N>400)
    Xsh = [Xsh(1:2:end,1),Xsh(1:2:end,2)];
    N = length(Xsh);
end

% plot(Xsh(:,2),Xsh(:,1),'.');
shapes = {'Rectangle';'Triangle';'Circle'; 'Hexagon'; 'Rhombus'};
minscore = 10;
name = ' ';
for k = 1:length(shapes)
    testfile = strcat(shapes(k), '.jpg');
    score = CompareContext(Xsh,testfile{1});
    if (score < minscore)
        shape = shapes{k};
        minscore = score;
    end
end

name = shape
end

