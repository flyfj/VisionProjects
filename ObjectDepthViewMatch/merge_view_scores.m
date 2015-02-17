function [ merge_scores ] = merge_view_scores( merge_ids, view_scores )
%VIEW_SCORE_TO_OBJ_SCORE Summary of this function goes here
%   convert view matching score to object matching score
%   use smallest distance as distance to object

assert(length(merge_ids) == length(view_scores));

max_score = max(view_scores);

merge_scores = zeros(1, length(unique(merge_ids))) + max_score;
for i=1:length(view_scores)
    cur_id = merge_ids(i);
    merge_scores(cur_id) = min(merge_scores(cur_id), view_scores(i));
end

end

