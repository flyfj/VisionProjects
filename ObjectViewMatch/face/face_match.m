
%% load face data
[db_feats, db_ids, query_feats, query_ids] = prepare_face_data('honda');

% distance between each query view and db view
view_match_scores = zeros(length(query_ids), length(db_ids));
% distance between each query view and db object
obj_match_scores = zeros(length(query_ids), length(unique(db_ids))) * inf;

unique_query = unique(query_ids);
% distance between query object and db object
obj_match_scores_comb = zeros(length(unique_query), length(unique(db_ids)));

%% brute force pixel L2

for i=1:size(query_feats, 1)
    qfeat = query_feats(i,:);
    dists = bsxfun(@minus, qfeat, db_feats);
    view_match_scores(i, :) = sqrt(sum(dists.^2, 2));
    for j=1:size(view_match_scores,2)
        obj_match_scores(i, db_ids(j)) = min(obj_match_scores(i, db_ids(j)), view_match_scores(i,j));
    end
    
    disp(num2str(i));
end


%% manifold matching
db_manifolds = comp_manifolds(0, 'honda_manifolds.mat', 'pca', db_feats, db_ids);

for i=1:size(query_feats, 1)
    qfeat = query_feats(i,:);
    for j=1:length(db_manifolds)
        obj_match_scores(i,j) = match_obj_manifold(qfeat, db_manifolds{j}.data, 'pca');
    end
end

%% single query evaluate

[p, r] = search_eval(query_ids, db_ids, view_match_scores);

%%

ranked_res_ids = zeros(length(query_ids), length(db_ids));
ranked_view_scores = zeros(size(ranked_res_ids));
ranked_obj_ids = zeros(length(query_ids), length(unique(db_ids)));
ranked_obj_scores = zeros(length(query_ids), length(unique(db_ids)));
obj_ids = unique(db_ids);

for i=1:length(query_ids)
%     [Y,I] = sort(view_match_scores(i,:), 2);
%     ranked_view_scores(i,:) = Y;
%     ranked_res_ids(i,:) = db_ids(I);
    
    [Y,I] = sort(obj_match_scores(i,:), 2);
    ranked_obj_ids(i,:) = obj_ids(I);
    ranked_obj_scores(i,:) = Y;
end

comp_rec_accu(query_ids, ranked_obj_ids, 1)

%% multiview voting

for i=1:length(unique_query)
    comb_ids = find(query_ids == unique_query(i));
    obj_match_scores_comb(i,:) = min(obj_match_scores(comb_ids, :), [], 1);
end

%% query manifold
query_manifolds = comp_manifolds(0, 'honda_query_manidolds.mat', 'pca', query_feats, query_ids);

for i=1:size(query_manifolds, 1)
    for j=1:length(db_manifolds)
        obj_match_scores_comb(i,j) = 1- comp_principle_dist(query_manifolds{i}.data.M, db_manifolds{j}.data.M);
    end
end


%% multiview evaluate
ranked_obj_ids = zeros(length(unique_query), length(unique(db_ids)));
ranked_obj_scores = zeros(length(unique_query), length(unique(db_ids)));
obj_ids = unique(db_ids);
for i=1:length(unique_query)
    [Y,I] = sort(obj_match_scores_comb(i,:), 2);
    ranked_obj_ids(i,:) = obj_ids(I);
    ranked_obj_scores(i,:) = Y;
end

comp_rec_accu(unique_query, ranked_obj_ids, 1)



