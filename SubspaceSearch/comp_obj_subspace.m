function [ eig_vecs, obj_mean ] = comp_obj_subspace( obj_imgs )
%COMP_OBJ_SUBSPACE Summary of this function goes here
%   each row of obj_imgs is a image

obj_mean = mean(obj_imgs, 1);
obj_imgs = obj_imgs - repmat(obj_mean, size(obj_imgs,1), 1);

covmat = cov(obj_imgs);
[eig_vecs, ~] = eig(covmat, size(obj_imgs,1));
eig_vecs = eig_vecs';

end

