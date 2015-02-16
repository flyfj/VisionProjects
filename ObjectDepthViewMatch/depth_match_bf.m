
% apply bruteforce depth image match

%% load data

% load db
db_dir = 'F:\3D\ModelNet\test_db_depth2\';
[db_feats, db_fns, db_obj_names, db_fn_obj_ids] = prepare_depth_db(1, db_dir);

%%
%load query
query_dir = 'F:\RGBD_Dataset\UW\rgbd-obj-dataset\rgbd-dataset\';
[query_feats, query_fns] = prepare_depth_query(1, query_dir);

%% perform match

% distance to all depth view for each query view
view_match_scores = zeros(length(query_fns), length(db_fns));
% distance to all database object for each query view
obj_match_scores = zeros(length(query_fns), length(db_obj_names));

% compute distane to each db view and db object
% profile on
parfor i=1:size(query_feats, 1)
    qfeat = query_feats(i,:);
    dists = bsxfun(@minus, qfeat, db_feats);
    view_match_scores(i, :) = sqrt(sum(dists.^2, 2));
    obj_match_scores(i,:) = view_score_to_obj_score(db_fn_obj_ids, view_match_scores(i,:));
%     profile viewer
    disp(num2str(i));
end


%% qualitative results

ranked_res_fns = cell(length(query_fns), length(db_fns));
ranked_view_scores = zeros(size(ranked_res_fns));
ranked_res_names = cell(length(query_fns), length(db_obj_names));
ranked_obj_scores = zeros(size(ranked_res_names));
for i=1:length(query_fns)
    [Y,I] = sort(view_match_scores(i,:), 2);
    ranked_view_scores(i,:) = Y;
    ranked_res_fns(i,:) = db_fns(I);
    
    [Y,I] = sort(obj_match_scores(i,:), 2);
    ranked_obj_scores(i,:) = Y;
    ranked_res_names(i,:) = db_obj_names(I);
end

visualize_search_res('res_depth_bf.html', query_fns, ranked_res_fns, 50);

%%

comp_accu(query_fns, ranked_res_names, 1, 1)

