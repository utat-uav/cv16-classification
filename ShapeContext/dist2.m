function n2 = dist2(x, c)
%DIST2	Calculates squared distance between two sets of points.
%
%	Description
%	D = DIST2(X, C) takes two matrices of vectors and calculates the
%	squared Euclidean distance between them.  Both matrices must be of
%	the same column dimension.  If X has M rows and N columns, and C has
%	L rows and N columns, then the result has M rows and L columns.  The
%	I, Jth entry is the  squared distance from the Ith row of X to the
%	Jth row of C.
%
%	See also
%	GMMACTIV, KMEANS, RBFFWD
%

%	Copyright (c) Christopher M Bishop, Ian T Nabney (1996, 1997)

% Stores the size (rows, columns) of x in ndata, dimx
[ndata, dimx] = size(x);

% Stores the size (rows, columns) of c in ncentres, dimc
[ncentres, dimc] = size(c);

% The column sizes of both input arrays needs to be the same, otherwise it
% will just throw an error and exit the program
if dimx ~= dimc
	error('Data dimension does not match dimension of centres')
end

% Returns an array n2, the math behind it is as follows:
%   (1) [ix1 column of sum of squares of each item in ith row]^T +
%   (2) ... [ix1 column of sum of squares of each item in ith row] -
%   (3) ... 2(x matrix multiplied by the transposed c)
%    -> where inputs are the same: equivalent to (x.^2)' + x.^2 - 2.x*x'
n2 = (ones(ncentres, 1) * sum((x.^2)', 1))' + ...
  		ones(ndata, 1) * sum((c.^2)',1) - ...
  		2.*(x*(c'));
