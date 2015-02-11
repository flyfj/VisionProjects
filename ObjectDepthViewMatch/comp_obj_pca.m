function [ eig_vecs, obj_mean ] = comp_obj_pca( obj_imgs )
%COMP_OBJ_SUBSPACE Summary of this function goes here
%   each row of obj_imgs is a image

obj_mean = mean(obj_imgs, 1);
obj_imgs = obj_imgs - repmat(obj_mean, size(obj_imgs,1), 1);

covmat = cov(obj_imgs);
[eig_vecs, eig_vals] = eig(covmat);
% maintain 90%
% eig_vals = eig_vals(:);

eig_vecs = eig_vecs';
valid_num = min(size(eig_vecs,1), 100);
eig_vecs = eig_vecs(1:valid_num, :);

end

