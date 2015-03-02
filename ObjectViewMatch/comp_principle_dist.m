function dist = comp_principle_dist( pca1, pca2 )
%COMP_PRINCIPLE_DIST Summary of this function goes here
%   Detailed explanation goes here

m = pca1' * pca2;
s = svd(m);
dist = s(1);

end

