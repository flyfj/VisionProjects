function [ obj_manifolds, obj_fns ] = prepare_depth_db_manifold( totrain )
%PREPARE_DEPTH_DB_MANIFOLD Summary of this function goes here
%   Detailed explanation goes here

if totrain == 1
    % load raw features
    
    
    % for each object, compute manifold
    % manifold structure: name: object name; data: cluster data, mean, eigvectors and
    % member files
    obj_names = keys(obj_map)';
    db_manifolds = cell(length(obj_names), 1);
    for i=1:length(obj_names)
        feat_ids = obj_map(obj_names{i});
        db_manifolds{i}.name = obj_names{i};
        db_manifolds{i}.data = comp_pca_manifold(db_hog(feat_ids, :), db_fns(feat_ids));
        disp([num2str(i) '/' num2str(length(obj_names)) ' manifold learned.']);
    end

    save('db_manifolds.mat', 'db_manifolds', '-v7.3');
    
else
    

end

