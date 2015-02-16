function accu = comp_rec_accu( query_gt_ids, ranked_ids, topK )
%COMP_PR Summary of this function goes here
%   recognition accuracy in topK results

accu = 0;
for i=1:length(query_gt_ids)
    corr_num = sum(ranked_ids(i,1:topK) == query_gt_ids(i));
    accu = accu + (corr_num > 0);
end

accu = accu / length(query_gt_ids);

end

