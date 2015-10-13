function name = IdentifyImage(filename, shapes)

%Process Test Image
TargetIm = rgb2gray(imread(filename));
TargetIm = TargetIm > 100;
TargetIm = double(TargetIm);
imshow(TargetIm);

minscore = 10;
for k = 1:length(shapes)
    %Process Reference Image
    file2 = shapes(k);
    file2 = [file2{1},'.jpg'];

    RefIm = rgb2gray(imread(file2));
    RefIm = RefIm > 100;
    RefIm = double(RefIm);
    
    %Compare
    fprintf(['Comparing: ', filename, ' with ', file2]);
    score = CompareContext(TargetIm, RefIm);
    fprintf(['   Score :', num2str(score),'\n']);
    if (score < minscore)
        shape = shapes{k};
        minscore = score;
    end
end

name = shape;