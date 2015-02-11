function [ obj_new_names, obj_scores ] = view_score_to_obj_score( obj_names, view_scores )
%VIEW_SCORE_TO_OBJ_SCORE Summary of this function goes here
%   convert view matching score to object matching score
%   use smallest distance as distance to object

assert(length(obj_names) == length(view_scores));

obj_map = containers.Map();
for i=1:length(obj_names)
    obj_id = obj_names{i};
    if isKey(obj_map, obj_id) == 0
        obj_map(obj_id) = inf;
    end
    obj_map(obj_id) = min(obj_map(obj_id), view_scores(i));
end

obj_new_names = keys(obj_map);
obj_scores = zeros(1, length(obj_new_names));
for i=1:length(obj_new_names)
    obj_scores(i) = obj_map(obj_new_names{i});
end

end

