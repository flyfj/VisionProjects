function [avg_pre, avg_recall, sel_ranks] = search_eval( query_ids, db_ids, dists, show )
%SEARCH_EVALUATOR Summary of this function goes here
%   evaluate search accuracy
%   scores is a matrix, each row is for each query, each col is for each db
%   item
%   method could be: 1) precision; 2) recall; 3) pr

% assert(size(dists,1) == length(query_ids));

pts_num = 100;
sel_ranks = 1: length(db_ids)/pts_num: length(db_ids);
sel_ranks = int32(sel_ranks);

avg_pre = zeros(1, length(sel_ranks));
avg_recall = zeros(1, length(sel_ranks));

for i=1:length(query_ids)
    [~,I] = sort(dists(i,:), 2);
    ranked_ids = db_ids(I) == query_ids(i);
    ranked_corr = cumsum(ranked_ids);
    for j=1:length(sel_ranks)
        % save results
        precision = ranked_corr(sel_ranks(j)) / double(sel_ranks(j));
        recall = ranked_corr(sel_ranks(j)) / ranked_corr(end);
        avg_pre(j) = avg_pre(j) + precision;
        avg_recall(j) = avg_recall(j) + recall;
    end
end

for i=1:length(avg_pre)
    avg_pre(i) = avg_pre(i) / length(query_ids);
    avg_recall(i) = avg_recall(i) / length(query_ids);
end

if show==1
    figure
    subplot(1, 2, 1)
    % draw pr curve
    title('pr curve')
    xlabel('recall'); hold on
    ylabel('precision'); hold on
    axis([0 1 0 1]); hold on
    grid on; hold on
    plot(avg_recall, avg_pre, 'r-')

    subplot(1,2,2)
    % draw rank precision curve
    title('rank curve')
    xlabel('rank'); hold on
    ylabel('precision'); hold on
    grid on; hold on
    plot(sel_ranks, avg_pre, 'ro-')
end

end

