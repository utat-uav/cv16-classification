shapes = {
    'A'; 
    'Circle'; 
    'Rhombus'; 
    'Triangle'; 
    'Rectangle'; 
    'Hexagon';
    'Rhombus';
    'wA'; 
    'wB'; 
    'wG';};

%'triangle45deg.jpg'

s = IdentifyImage('test.jpg', shapes);

fprintf(['\nFINAL SHAPE :' ,s, '\n']);