
% apply bruteforce depth image match

%% load data

% load db
db_dir = 'F:\3D\ModelNet\test_db_depth2\';
[db_feats, db_fns] = prepare_depth_db(0, db_dir);

disp('extracting db object names...');
db_obj_names = cell(0); % corresponding db obj name
db_fn_obj_ids = zeros(length(db_fns), 1);
obj_map = containers.Map();
for i=1:length(db_fns)
    % get db object name
    db_fns{i} = [db_dir db_fns{i}];
    parts = strsplit(db_fns{i}, '\');
    tmp = strsplit(parts{5}, '__');
    obj_id = [tmp{1} '__' tmp{2}];
    if isKey(obj_map, obj_id) == 0
        % add new object name
        db_obj_names = [db_obj_names; obj_id];
        obj_map(obj_id) = length(db_obj_names);
        disp(['added object: ' obj_id]);
    end
    db_fn_obj_ids(i) = obj_map(obj_id);
end
disp('done.');

% load query
query_dir = 'F:\RGBD_Dataset\UW\rgbd-obj-dataset\rgbd-dataset\';
[query_feats, query_fns] = prepare_depth_query(0);

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

ranked_res_fns = cell(length(query_fns), size(db_fns,2));
ranked_scores = zeros(size(ranked_res_fns));
for i=1:size(query_hog, 1)
    [Y,I] = sort(view_match_scores(i,:), 2);
    ranked_scores(i,:) = Y;
    ranked_res_fns(i,:) = db_fns(1,I);
end

visualize_search_res('res_depth_bf.html', query_fns, ranked_res_fns, 50);


