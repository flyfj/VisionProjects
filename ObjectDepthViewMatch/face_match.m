
%% load face data
[db_feats, db_ids, query_feats, query_ids] = prepare_face_data('honda');

%% brute force pixel l2

view_match_scores = zeros(length(query_ids), length(db_ids));
obj_match_scores = zeros(length(query_fns), length(unique(db_ids)));

parfor i=1:size(query_feats, 1)
    qfeat = query_feats(i,:);
    dists = bsxfun(@minus, qfeat, db_feats);
    view_match_scores(i, :) = sqrt(sum(dists.^2, 2));
    
    disp(num2str(i));
end

%% evaluate

ranked_res_ids = zeros(length(query_ids), length(db_ids));
ranked_view_scores = zeros(size(ranked_res_ids));
for i=1:length(query_ids)
    [Y,I] = sort(view_match_scores(i,:), 2);
    ranked_view_scores(i,:) = Y;
    ranked_res_ids(i,:) = db_ids(I);

end

comp_rec_accu(query_ids, ranked_res_ids, 1)