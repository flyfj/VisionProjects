function obj_manifold = comp_pca_manifold( obj_feats, obj_fns )
%COMP_PCA Summary of this function goes here
%   input features for a object, 
%   compute mixure of pca to approximate a manifold
%   each row is feature from an object view

% kmeans to get clusters
cls_num = 6;
[idx, centers] = kmeans(obj_feats, cls_num);

% for each cluster, compute pca
obj_manifold = cell(cls_num, 1);
for i=1:size(centers)
    members = obj_feats(find(idx==i), :);
    [obj_eig, obj_mean] = comp_obj_pca(members);
    obj_manifold{i}.fns = obj_fns(find(idx==i));
    obj_manifold{i}.eig = obj_eig;
    obj_manifold{i}.mean = obj_mean;
    disp(['finished ' num2str(i) 'th cluster pca']);
end

end

