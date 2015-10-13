clear all;
close all;
clc;

pic = 'IMG001.jpg'
log = 'IMG001.txt'

FOCAL_LENGTH = 50; 
CCD_HEIGHT = 24; 
CCD_WIDTH =36; 
R =6371000;
imgW = 1200;
imgH = 800;

%%
Im1 = imread(pic);
Im = imresize(Im1,[800,1200]);
[lat, lon, alt, head] = readgps(log);

alphaX = 2*atan2(CCD_WIDTH, 2*FOCAL_LENGTH);
alphaY = 2*atan2(CCD_HEIGHT, 2*FOCAL_LENGTH);
W = 2*alt*tan(alphaX/2);
H = 2*alt*tan(alphaY/2);
CHANNEL = 1; %2, 3   
[n b] = FindTarget(Im,0.05,CHANNEL);
[H2 W2] = size(Im);
 for i = 1:n
    x = b(i,1) + b(i,3)/2;
    y = b(i,2) + b(i,4)/2;
    w = b(i,3)*1.2/2;
    h = b(i,4)*1.2/2;
    xL = max(0,x-w);
    xR = min(x+w, W2);
    yT = max(0,y-h);
    yB = min(y+h,H2);
    Target = Im(yT:yB, xL:xR,:);
    name = AnalyzeTarget(Target);
    	
    xd = ( double(x-imgW/2)/imgW ) *W;
	yd = ( double(y-imgH/2)/imgH ) *H;
    if (y==0)
        beta = 0;
    else
        beta = atan2(double(xd),double(yd));
    end

    if (xd >= 0) 
        beta = pi-beta;
    else
        beta = -pi-beta;
    end

	the = beta + head*pi/180;
	d = sqrt(xd^2+yd^2);
    lat1 = lat*pi/180;
    lon1 = lon*pi/180;
	lat2 = asin( sin(lat1)*cos(d/R) + cos(lat1)*sin(d/R)*cos(the));
	lon2 = (lon1 + atan2(sin(the)*sin(d/R)*cos(lat1), cos(d/R)-sin(lat1)*sin(lat2)));

	lat2 = (lat2)*180/pi;
	lon2 = (lon2)*180/pi;
    
    p = 1;
    q = 2;
    if(name{1} == name{2})
        q = 3;
    end
        
    sprintf('Target with %s and %s', name{p},name{q})
    sprintf('Located at %.5f and %.5f', lat2, lon2)
 end
