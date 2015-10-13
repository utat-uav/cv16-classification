clear all;
close all;
clc;
% Xsh = ContourPoints('Hexagon.jpg');
% shapes = {'Rectangle';'Triangle';'Circle'; 'Hexagon'; 'Rhombus'};
% for k = 1:length(shapes)
%     testfile = strcat(shapes(k), '.jpg');
%     CompareContext(Xsh,testfile{1})
% end

Target = imread('TriangleG.jpg');
AnalyzeTarget(Target,3,0.1);