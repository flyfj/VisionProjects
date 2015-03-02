function [ db_manifolds ] = comp_manifolds( totrain, saved_fn, method, feats, obj_ids )
%PREPARE_DEPTH_DB_MANIFOLD Summary of this function goes here
%   input a set of features, each has an object id
%   object id must start from 1 and continuous
%   return a set of manifolds for each object id

if totrain == 1
    
    disp('computing manifolds...');
    unique_obj_ids = unique(obj_ids);
    % for each object, compute manifold
    db_manifolds = cell(length(unique_obj_ids), 1);
    for i=1:length(unique_obj_ids)
        feat_ids = find(obj_ids == unique_obj_ids(i));
        db_manifolds{i}.id = unique_obj_ids(i);
        % compute manifold
        switch method
            case 'pca'
                [mapped_data, mapping] = compute_mapping(feats(feat_ids, :), 'PCA', 0.9);
            case 'autoencoder'
                [mapped_data, mapping] = compute_mapping(feats(feat_ids, :), 'Autoencoder');
        end
        % comp_pca_manifold(db_feats(feat_ids, :), db_fns(feat_ids));
        db_manifolds{i}.data = mapping;
        disp([num2str(i) '/' num2str(length(unique_obj_ids)) ' manifold learned.']);
    end
    
    save(saved_fn, 'db_manifolds', '-v7.3');
    disp('manifold computed and saved.');
    
else
    disp('loading manifold...');
    tmp = load(saved_fn);
    db_manifolds = tmp.db_manifolds;
    disp('manifold loaded.');
end

disp('manifold ready.');

