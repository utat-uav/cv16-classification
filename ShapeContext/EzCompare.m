function EzCompare( TargetFile )

Xshape = ContourPoints(TargetFile);

% plot(Xsh(:,2),Xsh(:,1),'.');
shapes = {'Rectangle';'Triangle';'Circle'; 'Hexagon'; 'Rhombus'};
maxscore = 1e-5;
maxshape = '';
for k = 1:length(shapes)
    testfile = strcat(shapes(k), '.jpg');
    score = CompareContext(Xshape,testfile{1});
    fprintf('Shape: %12s, \t Score: %0.4f\n', shapes{k}, score);
    if (score > maxscore)
      maxshape = shapes{k};
      maxscore = score;
    end
end

fprintf('The shape is a %s, with a score of %0.4f\n', maxshape, maxscore);

