function name = AnalyzeTarget( TargetIm )
name = {' '; ' '; ' '};
cform = makecform('srgb2lab');
lab_TargetIm = applycform(TargetIm, cform);

ab = double(lab_TargetIm(:,:,2:3));
nrows = size(ab, 1);
ncols = size(ab, 2);
ab = reshape(ab, nrows*ncols, 2);

nColors = 3;
% Repeat clustering 3 times
[cluster_idx cluster_center] = kmeans(ab, nColors, 'distance', 'sqEuclidean', 'emptyaction', 'drop' , 'Replicates',8);

pixel_labels = reshape(cluster_idx, nrows, ncols);

% Separate image by colour
segmented_images = cell(1,3);

for z = 1:nColors
    msk = im2bw(TargetIm, 0); %Pure white
    msk(pixel_labels ~= z) = 0;
    segmented_images{z} = msk;
    figure;
    imshow(msk);
    
    shapes = {'Rectangle';'Triangle';'Circle'; 'Hexagon'; 'Rhombus';'A';'B';'G'};
    minscore = 10;
    for k = 1:length(shapes)
        testfile = strcat(shapes(k), '.jpg');
        im2 = imread(testfile{1});
        bwim2 = rgb2gray(im2);
        ref = double(bwim2>10);
        msk = double(msk);
        score = CompareContext(msk,ref);
        if (score < minscore)
            shape = shapes{k};
            minscore = score;
        end
    end

    name{z} = shape;
end

end

