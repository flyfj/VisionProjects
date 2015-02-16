
% match object using manifold model

%% load data

% db manifold
db_manifolds = prepare_manifolds(0, 'db_manifolds.mat');

% query data
query_dir = 'F:\RGBD_Dataset\UW\rgbd-obj-dataset\rgbd-dataset\';
[query_feats, query_fns] = prepare_depth_query(0);

%% matching

% result data
db_obj_names = cell(length(db_manifolds), 1);
obj_match_scores = zeros(length(query_fns), length(db_manifolds));

% match
for i=1:1%size(query_hog, 1)
    qfeat = query_feats(i,:);
    % match each object manifold
    for j=1:length(db_manifolds)
        obj_match_scores(i, j) = match_obj_manifold(qfeat, db_manifolds{j}.data);
        if i>1
            continue;
        end
        db_obj_names{j} = db_manifolds{j}.name;
    end
    
    disp(num2str(i));
end

%% evaluation
% form result files
ranked_scores = zeros(length(query_fns), length(db_obj_names));
ranked_res_names = cell(length(query_fns), length(db_obj_names));
for i=1:length(query_fns)
    [Y,I] = sort(obj_match_scores(i,:), 2);
    ranked_scores(i,:) = Y;
    ranked_res_names(i,:) = db_obj_names(I);
end

comp_accu(query_fns, ranked_res_names, 5, 1)
