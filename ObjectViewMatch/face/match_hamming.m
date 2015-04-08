function [ sim_scores ] = match_hamming( probe_codes, gal_codes )
%MATCH_HAMMING Summary of this function goes here
%   
sim_scores = zeros(size(probe_codes,1), size(gal_codes,1));
sim_scores = single(sim_scores);

for i=1:length(probe_codes)
    for j=1:length(gal_codes)
        sim_scores(i,j) = compare_sets(probe_codes{i}, gal_codes{j});
    end
    
    disp(['probe: ' num2str(i)]);
end

% sim_scores = 1 - sim_scores ./ max(sim_scores(:));
disp('similarity computed.');

end


function score = compare_sets(probe_set, gal_set)

dists = pdist2(probe_set, gal_set, 'hamming');
dists = floor(dists.*size(gal_set, 2));
score = mean(dists(:));

end