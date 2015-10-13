clear all;
close all;
clc;
% Xsh = ContourPoints('Hexagon.jpg');
% shapes = {'Rectangle';'Triangle';'Circle'; 'Hexagon'; 'Rhombus'};
% for k = 1:length(shapes)
%     testfile = strcat(shapes(k), '.jpg');
%     CompareContext(Xsh,testfile{1})
% end

%%
Im = imread('test.jpg');
CHANNEL = 1; %2, 3   
[n b] = FindTarget(Im,0.1,1);
[H W] = size(Im);
 for i = 1:n
    x = b(i,1) + b(i,3)/2;
    y = b(i,2) + b(i,4)/2;
    w = b(i,3)*1.2/2;
    h = b(i,4)*1.2/2;
    xL = max(0,x-w);
    xR = min(x+w, W);
    yT = max(0,y-h);
    yB = min(y+h,H);
    Target = Im(yT:yB, xL:xR,:);
%     figure
%     imshow(Target);
     AnalyzeTarget(Target,1,0.1);
 end
%%
%  Target = imread('asd.jpg');
%  AnalyzeTarget(Target,1,0.1);