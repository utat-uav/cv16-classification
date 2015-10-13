function filtered = NoiseFilter (threshold, target)
test = imread(target);     
% convert it into double type
test = rgb2gray(test);
filtered = zeros(size(test));    
for i = 1:length(test)
        for j = 1:length(i)
            filtered(i, j) = test(i, j)>threshold;
        end
end

test = uint8(test);
figure;
imshow(test);
title('Original');

filtered = uint8(filtered);
figure;
imshow(filtered);
title('Filtered');
end

