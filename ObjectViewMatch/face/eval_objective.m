function [ val ] = eval_objective( train_codes, train_ids )
%EVAL_OBJ Summary of this function goes here
%   evaluate binary code quality via objective function
%   objective: hamming distance to point in the same set as small as
%   possible; different sets as big as possible

% compute distance
sim_dists = [];
dis_dists = [];
dists = pdist(train_codes, 'hamming');
dists = int32(dists .* size(train_codes,2));
pdists = squareform(dists);
% pdists(1:size(pdists,2)+1:end) = 100;

ids = unique(train_ids);
for i=1:length(ids)
    samps = find(ids(i)==train_ids);
    d = pdists(samps, samps);
    sim_dists = [sim_dists; d(:)];
    samps = find(ids(i)~=train_ids);
    d = pdists(samps, samps);
    dis_dists = [dis_dists; d(:)];
end
% compute objective
val = mean(sim_dists) - mean(dis_dists);

end

