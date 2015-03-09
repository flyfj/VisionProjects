

%% load data
% database
[db_feats, db_cate_ids] = prepare_depth_db(1, 'hog');

%% query
[query_feats, query_cate_ids] = prepare_depth_query(1, 'hog');

%% match

sim_scores = match_l2(db_feats, query_feats);

%% multiview



%% evaluation

dists = 1 - sim_scores;
[apr, ar, ranks] = search_eval(query_cate_ids, db_cate_ids, dists);

save('point-point-hog-res.mat', 'apr', 'ar', 'ranks');