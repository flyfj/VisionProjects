
% apply bruteforce depth image match

%% load data

% load db
db_dir = 'F:\3D\ModelNet\test_db_depth2\';
[db_feats, db_fns, db_cate_names, db_fn_cate_ids, db_obj_names, db_fn_obj_ids] = prepare_depth_db(0, db_dir);

unique_db_obj_ids = unique(db_fn_obj_ids);
unique_db_cate_ids = unique(db_fn_cate_ids);

%%
%load query
query_dir = 'F:\RGBD_Dataset\UW\rgbd-obj-dataset\rgbd-dataset\';
[query_feats, query_fns, query_fn_cate_ids] = prepare_depth_query(0, query_dir);

%% L2

% distance to each depth view, object and category
view_match_scores = zeros(length(query_fns), length(db_fns));
obj_match_scores = zeros(length(query_fns), length(db_obj_names));
cate_match_scores = zeros(length(query_fns), length(db_cate_names));

% profile on
parfor i=1:size(query_feats, 1)
    qfeat = query_feats(i,:);
    dists = bsxfun(@minus, qfeat, db_feats);
    view_match_scores(i, :) = sqrt(sum(dists.^2, 2));
    obj_match_scores(i,:) = merge_view_scores(db_fn_obj_ids, view_match_scores(i,:));
    cate_match_scores(i,:) = merge_view_scores(db_fn_cate_ids, view_match_scores(i,:));
%     profile viewer
    disp(num2str(i));
end

%% evaluation
ranked_db_fns = cell(length(query_fns), length(db_fns));
ranked_view_scores = zeros(size(ranked_db_fns));
ranked_obj_ids = zeros(length(query_fns), length(unique_db_obj_ids));
ranked_obj_scores = zeros(size(ranked_obj_ids));
ranked_cate_ids = zeros(length(query_fns), length(unique_db_cate_ids));
ranked_cate_scores = zeros(size(ranked_cate_ids));

for i=1:length(query_fns)
    [Y,I] = sort(view_match_scores(i,:), 2);
    ranked_view_scores(i,:) = Y;
    ranked_db_fns(i,:) = db_fns(I);
    
    [Y,I] = sort(obj_match_scores(i,:), 2);
    ranked_obj_scores(i,:) = Y;
    ranked_obj_ids(i,:) = unique_db_obj_ids(I);
    
    [Y,I] = sort(cate_match_scores(i,:), 2);
    ranked_cate_scores(i,:) = Y;
    ranked_cate_ids(i,:) = unique_db_cate_ids(I);
end

disp('single view accuracy: ');
comp_rec_accu(query_fn_cate_ids, ranked_cate_ids, 34)

%% multiview
unique_query = unique(query_fn_cate_ids);
cate_match_scores_comb = zeros(length(unique_query), length(unique_db_cate_ids));
for i=1:length(unique_query)
    comb_ids = find(query_fn_cate_ids == unique_query(i));
    cate_match_scores_comb(i,:) = min(cate_match_scores(comb_ids, :), [], 1);
end

ranked_cate_ids = zeros(length(unique_query), length(unique_db_cate_ids));
ranked_cate_scores = zeros(length(unique_query), length(unique_db_cate_ids));
for i=1:length(unique_query)
    [Y,I] = sort(cate_match_scores_comb(i,:), 2);
    ranked_cate_ids(i,:) = unique_db_cate_ids(I);
    ranked_cate_scores(i,:) = Y;
end

disp('multiview accuracy: ');
comp_rec_accu(unique_query, ranked_cate_ids, 1)

%% qualitative results



visualize_search_res('res_depth_bf.html', query_fns, ranked_res_fns, 50);

