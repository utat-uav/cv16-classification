function X = ContourPoints(file)
jpeg = imread(file);
Im = rgb2gray(jpeg);

[bw,~,Gv,Gh] = edge(Im,'sobel');
[x y] = find(bw>0);
X = [x y];



