function [avg_pre, avg_recall] = search_eval( query_ids, db_ids, scores )
%SEARCH_EVALUATOR Summary of this function goes here
%   evaluate search accuracy
%   scores is a matrix, each row is for each query, each col is for each db
%   item
%   method could be: 1) precision; 2) recall; 3) pr

assert(size(scores,1) == length(query_ids));

pts_num = 20;
sel_ranks = 1: length(db_ids)/pts_num: length(db_ids);
sel_ranks = int32(sel_ranks);

avg_pre = zeros(1, length(sel_ranks));
avg_recall = zeros(1, length(sel_ranks));

for i=1:length(query_ids)
    [~,I] = sort(scores(i,:), 2);
    ranked_ids = db_ids(I) == query_ids(i);
    ranked_corr = cumsum(ranked_ids);
    cnt = 0;
    for j=1:length(sel_ranks)
        cnt = cnt + 1;
        % save results
        precision = ranked_corr(sel_ranks(j)) / sel_ranks(j);
        recall = ranked_corr(sel_ranks(j)) / ranked_corr(end);
        avg_pre(cnt) = avg_pre(cnt) + precision;
        avg_recall(cnt) = avg_recall(cnt) + recall;
    end
end

for i=1:length(avg_pre)
    avg_pre(i) = avg_pre(i) / length(query_ids);
    avg_recall(i) = avg_recall(i) / length(query_ids);
end

% draw curve
xlabel('recall'); hold on
ylabel('precision'); hold on
grid on; hold on
plot(avg_recall, avg_pre, 'r-')



end

