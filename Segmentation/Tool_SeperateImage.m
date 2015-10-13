function SeperateImage(TargetIm)

R = TargetIm(:,:,1);
G = TargetIm(:,:,2);
B = TargetIm(:,:,3);

HSV = rgb2hsv(TargetIm);

H = HSV(:,:,1);
S = HSV(:,:,2);
V = HSV(:,:,3);

%%

subplot(2,3,1), subimage(R);
title('Red');
subplot(2,3,2), subimage(G);
title('Green');
subplot(2,3,3), subimage(B);
title('Blue');

subplot(2,3,4), subimage(H);
title('Hue');
subplot(2,3,5), subimage(S);
title('Saturation');
subplot(2,3,6), subimage(V);
title('Intensity');

%%
figure;

subplot(2,3,1), imhist(R);
title('Red');
subplot(2,3,2), imhist(G);
title('Green');
subplot(2,3,3), imhist(B);
title('Blue');

subplot(2,3,4), imhist(H);
title('Hue');
subplot(2,3,5), imhist(S);
title('Saturation');
subplot(2,3,6), imhist(V);
title('Intensity');

