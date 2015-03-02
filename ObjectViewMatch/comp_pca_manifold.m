function obj_manifold = comp_pca_manifold( obj_feats, obj_fns )
%COMP_PCA Summary of this function goes here
%   input features for a object, 
%   compute mixure of pca to approximate a manifold
%   each row is feature from an object view

% kmeans to get clusters
cls_num = 6;
[idx, centers] = kmeans(obj_feats, cls_num, 'Replicates', 5);

% for each cluster, compute pca
obj_manifold = cell(cls_num, 1);
for i=1:size(centers)
    member_ids = find(idx==i);
    members = obj_feats(member_ids, :);
    [~, obj_manifold{i}.mapping] = compute_mapping(members, 'PCA', 0.9);
%     [obj_eigs, obj_mean] = comp_obj_pca(members);
    obj_manifold{i}.fns = obj_fns(member_ids);
    %obj_manifold{i}.eigs = obj_eigs;
    %obj_manifold{i}.mean = obj_mean;
    disp(['finished ' num2str(i) 'th cluster pca']);
end

% visualize clusters
% visualize_clusters(obj_manifold);
% pause

end

