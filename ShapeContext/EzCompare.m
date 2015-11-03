function EzCompare( ns )

% Ok so right off the bat, I imagine the image will need to be an input
% argument of the function. This is for later once everything starts coming
% together, but good to be aware. AND if it is changed to 2 input
% parameters, then NEED TO CHANGE THE "if nargin < 2: ns = 2; end"
% condition to account for the 2 input arguments


% ns - number of sigmas (std dev) to compare against
%     default = 2;


% For testing - going through all files for faster testing script
Files = {'A.jpg', 'B.jpg', 'Circle.jpg', 'G.jpg', 'Hexagon.jpg', 'real_triangle.jpg', 'Rectangle.jpg', 'Rhombus.jpg', 'Triangle.jpg', 'Triangle45deg.jpg', 'Triangleinverted.jpg', 'TriangleSideways.jpg', 'wA.jpg', 'wB.jpg', 'wBtwisted.jpg', 'wG.jpg'};
% EXCLUDED FOR ERROR'S SAKE - G2.jpg, and Rrotated.jpg - may just not be
% agreeing with the 650x600x3 file size expected from imread in
% ContourPoints. Probably just need to have it properly re-sized


% THIS IS THE ONE WE WILL ACTUALLY USE - Only for the inputted image
%TargetFile = 'G.jpg';


% THIS IS FOR TESTING PURPOSES (Tests more and faster) - Go through all the test files
for Indx = 1:length(Files)
    TargetFile = Files{Indx};
    fprintf('Current file being tested: %s\n', TargetFile);
    
    
    % Where the number of arguments inputted into this function is less
    % than 2, the standard deviation is set to 2
    % NOTE: I dropped it to 1.5 and it made a decision for nearly all
    % images and was right for the vast majority, but it also decided on
    % some twice due to them beoth being in the threshold. TINKERING WITH
    % THIS (and the output decision based on threshold) MAY HELP
    if nargin < 2
       ns = 2;
    end

    
    % Goes to the ContourPoints function with the current image and outputs
    % an array of all the x and y indices in the logical image array
    % created in CoutourPoints corresponding to the location of a contour
    % in the image. 
    
    % Based on the edge detection method selected in ContourPoints, so may
    % want to toy around with it for a short bit to see if it has much of
    % an impact in providing a better contour (keep in mind runtime)
    Xshape = ContourPoints(TargetFile);

    
    % plot(Xsh(:,2),Xsh(:,1),'.');
    
    % List of the possible shapes it will try to match the image with
    % NOTE: When a new shape is inputted, make sure that the name entered
    % into this list is the filename (minus the jpeg extension) as it
    % concatenates a .jpeg onto it later and tries to open it. Making a new
    % name that does not match any existing file will likely cause an error
    shapes = {'Rectangle';'Triangle';'Circle'; 'Hexagon'; 'Rhombus'; 'A'; 'B'; 'G'};
    
    % Array to track the scores of the shapes (cost functions I believe)
    shape_score = zeros(length(shapes),1);
    
    % Keep track of the shape with the highest score
    % WHY IS THIS NOT BEING USED? It seems like the kind of thing that
    % should be...
    maxscore = 1e-5;
    maxshape = '';
    
    % Go through all the reference shapes
    for k = 1:length(shapes)
        % Get the filename of the shape based on the name in the shape list
        testfile = strcat(shapes(k), '.jpg');
        
        % Goes to function CompareContext with the array of all points
        % ideteified as a contour for the test image, and the reference
        % image (undergoing iteration) as inputs
        score = CompareContext(Xshape,testfile{1});
        
        fprintf('Shape: %12s, \t Score: %0.4f\n', shapes{k}, score);
        shape_score(k) = score;
    %     if (score > maxscore)
    %       maxshape = shapes{k};
    %       maxscore = score;
    %     end
    end

    % calculate std dev and find sigma outliers
    sh_stdev = std(shape_score);
    sh_indices = find(abs(shape_score - median(shape_score)) > ns*sh_stdev);
    
    flag = 0;
    for j = 1:length(sh_indices)
       flag = 1;
       idx = sh_indices(j);
       fprintf('The shape may be a %s, with a score of %0.4f\n', shapes{idx}, shape_score(idx));
    end
    if flag == 0
        fprintf('No matches were found.\n');
    end
    fprintf('\n\n\n');
end
