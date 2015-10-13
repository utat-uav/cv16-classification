function GetLetter(TargetIm)

bwIm = rgb2gray(TargetIm);

% TODO need automatically determined threshold
logicalLetter = (bwIm > 200);
imHull = imfill(+logicalLetter);
imshow(imHull);