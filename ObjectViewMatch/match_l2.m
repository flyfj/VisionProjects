function [ sim_scores ] = match_l2( db_feats, query_feats )
%FACE_MATCH_L2 Summary of this function goes here
%   Detailed explanation goes here

sim_scores = zeros(size(query_feats,1), size(db_feats,1));
%sim_scores = single(sim_scores);

for i=1:length(query_feats)
    parfor j=1:length(db_feats)
        sim_scores(i,j) = compare_sets(query_feats{i}, db_feats{j});
    end
    
    disp(['probe: ' num2str(i)]);
end

sim_scores = 1 - sim_scores ./ max(sim_scores(:));

end


function score = compare_sets(probe_set, gal_set)

scores = ones(1, size(gal_set,1)) * inf;
for i=1:size(probe_set,1)
    probe_repmat = repmat(probe_set(i,:), size(gal_set, 1), 1);
    dists = sqrt(sum((probe_repmat-gal_set).^2, 2));
    scores(i) = min(dists);
end

score = min(scores);

end