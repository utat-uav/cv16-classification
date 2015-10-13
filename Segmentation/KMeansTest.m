function KMeansTest (TargetIm)
% Run kmeans image segmentation on a RGB image

% Convert image from RGB to L*a*b*
cform = makecform('srgb2lab');
lab_TargetIm = applycform(TargetIm, cform);

% Classify colors in a*b* space using Kmeans clustering
ab = double(lab_TargetIm(:,:,2:3));
nrows = size(ab, 1);
ncols = size(ab, 2);
ab = reshape(ab, nrows*ncols, 2);

imshow(ab);

nSegments = 3;
% Repeat clustering 3 times
[cluster_idx cluster_center] = kmeans(ab, nSegments, 'distance', 'sqEuclidean', 'Replicates',5);

pixel_labels = reshape(cluster_idx, nrows, ncols);
figure;
imshow(pixel_labels,[]);

% Separate image by colour
segmented_images = cell(1,3);

for k = 1:nSegments
    % Using an rgb mask of the original image
    rgb_label = repmat(pixel_labels ~= k, [1 1 1]);
    mask_im = TargetIm; %Pure white
    mask_im(rgb_label) = 0;
    segmented_images{k} = mask_im;
end

figure;
subplot(1,3,1), subimage(segmented_images{1});
subplot(1,3,2), subimage(segmented_images{2});
subplot(1,3,3), subimage(segmented_images{3});

% Segment into seperate images
% mean_cluster_val = zeros(3,1);

% for k = 1:nColors
%     mean_cluster_val(k) = mean(cluster_center(k));
% end
% 
% [mean_cluster_val, idx] = sort(mean_cluster_val);
% 
% figure;
% for k = 1:nColors
%     color_cluster_num = idx(k);
%     L = lab_TargetIm(:,:,k);
%     color_idx = find(pixel_labels == color_cluster_num);
%     L_color = L(color_idx);
%     is_color = im2bw(L_color, graythresh(L_color));
%     
%     % mask image with is_color
%     label = repmat(uint8(0), [nrows ncols]);
%     label(color_idx(is_color == false)) = 1;
%     color_output = TargetIm;
%     color_output(label ~= 1) = 0;
%     subplot(1,3,k), subimage(color_output);
% end
