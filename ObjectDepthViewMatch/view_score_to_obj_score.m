function [ obj_scores ] = view_score_to_obj_score( obj_ids, view_scores )
%VIEW_SCORE_TO_OBJ_SCORE Summary of this function goes here
%   convert view matching score to object matching score
%   use smallest distance as distance to object

assert(length(obj_ids) == length(view_scores));

max_score = max(view_scores);

obj_scores = zeros(1, length(unique(obj_ids))) * max_score;
for i=1:length(view_scores)
    cur_obj_id = obj_ids(i);
    obj_scores(cur_obj_id) = min(obj_scores(cur_obj_id), view_scores(i));
end

end

