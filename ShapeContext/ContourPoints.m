function X = ContourPoints(file)
% Reads the file into a 650x600x3 file representation
jpeg = imread(file);

% Converts the image to grayscale (likely for simplicity)
Im = rgb2gray(jpeg);

% Edge is the edge detection method:
%   - 'Canny' - Finds edges by looking for local maxima of the gradient of 
%     I. The edge function calculates the gradient using the derivative of 
%     a Gaussian filter. This method uses two thresholds to detect strong 
%     and weak edges, including weak edges in the output if they are 
%     connected to strong edges. By using two thresholds, the Canny method 
%     is less likely than the other methods to be fooled by noise, and more
%     likely to detect true weak edges. Not supported on a GPU.
%   - ‘log' (Laplacian of Gaussian) - Finds edges by looking for zero-
%     crossings after filtering I with a Laplacian of Gaussian filter.
%   - 'Prewitt' - Finds edges using the Prewitt approximation to the 
%     derivative. It returns edges at those points where the gradient of I 
%     is maximum.
%   - 'Roberts' - Finds edges using the Roberts approximation to the 
%     derivative. It returns edges at those points where the gradient of I 
%     is maximum.
%   - 'Sobel' - Finds edges using the Sobel approximation to the derivative. 
%     It returns edges at those points where the gradient of I is maximum.
%   - 'zerocross' - Finds edges by looking for zero-crossings after 
%     filtering I with a filter that you specify.

% Currently using 'sobel', but may want to test a few other ones to see how
% they compare

% Returns a few things, most notable is bw: a binary image in the form of a
% logical array  the size of Im % with 1's where it detects an edge and 0's
% everywhere else

% More info: http://www.mathworks.com/help/images/ref/edge.html
[bw,~,Gv,Gh] = edge(Im,'sobel');

% Stores a nx2 matrix of all the x and y indices that correspond to a 1 in
% the logical image array (determined to be contour)
[x y] = find(bw>0);

% Store x and y in an array X and return it
X = [x y];
