function EzCompare( TargetFile, ns )
% ns - number of sigmas (std dev) to compare against
%     default = 2;

if nargin < 2
   ns = 2;
end

Xshape = ContourPoints(TargetFile);

% plot(Xsh(:,2),Xsh(:,1),'.');
shapes = {'Rectangle';'Triangle';'Circle'; 'Hexagon'; 'Rhombus'};
shape_score = zeros(length(shapes),1);
maxscore = 1e-5;
maxshape = '';
for k = 1:length(shapes)
    testfile = strcat(shapes(k), '.jpg');
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

for j = 1:length(sh_indices)
   idx = sh_indices(j);
   fprintf('The shape may be a %s, with a score of %0.4f\n', shapes{idx}, shape_score(idx));
end
