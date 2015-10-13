function GetShape (TargetIm)
% Get BW shape silouhette from image segment of shape

% IMPORTANT!!! --------------------------------------
%       Convex hulls won't work for shapes with
%   concave features.
% ---------------------------------------------------

% TODO: dynamic threshold ?based on edge? instead of fixed 100
bwIm = rgb2gray(TargetIm);
bwShape = bwIm > 100;

imHull = bwconvhull(bwShape, 'union');
imshow(imHull);

% edgeHull = edge(imHull, 'canny', 0.6);
% imshow(edgeHull);

%figure;
%bwLetter = rgb2gray(LetterIm);
%xorIm = xor(imHull, bwLetter);
%imshow(xorIm);
%figure;
%xor2Im = xor(xorIm, imHull);
%imshow(xor2Im);
