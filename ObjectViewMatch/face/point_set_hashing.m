

%% load data
addpath(genpath('SPLH_code'))
[db_feats, db_ids, query_feats, query_ids] = prepare_face_data('honda');

% stack all
db_feats_all = [];
db_ids_all = [];
for i=1:length(db_feats)
    db_feats_all = [db_feats_all; db_feats{i}];
    db_ids_all = [db_ids_all; ones(size(db_feats{i},1), 1)*db_ids(i)];
end

% clear db_feats
% clear query_feats

%% iteration
num_iter = 5;
obj_vals = zeros(num_iter, 1);
db_codes_all = [];
sim_imgs = cell(num_iter,1);
iter_codes = cell(num_iter,1);
eval_res = [];
for iter=1:num_iter
    fprintf('iter: %d\n', iter);
    % create similarity matrix
    sim_mat = -1 * ones(size(db_feats_all,1), size(db_feats_all,1));
    if iter==1
        % use L2
        dists = pdist(db_feats_all, 'euclidean');
        dists = squareform(dists);
    else
        % use hamming distance
        dists = pdist(db_codes_all, 'hamming');
        dists = squareform(dists);
        dists = floor(dists.*size(db_codes_all, 2));
    end
    
    % find most similar points for each point
    disp('computing similarity matrix')
    for i=1:size(dists,1)
        % ignore self distance
        dists(i,i) = inf;
        % points in the same set
        cursetids = find(db_ids_all==db_ids_all(i));
        sim_mat(i, cursetids) = 0;
        mindist1 = min(dists(i, cursetids));
        sameminids1 = find( dists(i,cursetids)==mindist1 );
        sim_mat(i, cursetids(sameminids1)) = 1;% ./ length(cursetids);
        % different sets
        diffsetids = find(db_ids_all~=db_ids_all(i)); 
        %mindist2 = min(dists(i, diffsetids));
        %sameminids2 = find( dists(i,diffsetids)==mindist2 );
        sim_mat(i, diffsetids) = -1; % ./ length(diffsetids);
        % compute objective
        diff = mindist1 - sum(dists(i,diffsetids));
        if diff > 0
            diff = diff;
        end
        obj_vals(iter) = obj_vals(iter) + diff;
    end
    sim_imgs{iter} = sim_mat;
    obj_vals(iter) = obj_vals(iter) ./ size(dists,1);
    obj_vals

    % learn hash functions/codes
    SPLHparam.nbits = 12;
    SPLHparam.eta = 0.2;
    SPLHparam = trainSPLH(db_feats_all, SPLHparam, db_feats_all', sim_mat);
    [B, U] = compressSPLH(db_feats_all, SPLHparam);
    db_codes_all = double(U);
    iter_codes{iter} = db_codes_all;
    
    % evaluate search performance
    % db codes
    db_codes = cell(size(db_feats));
    for k=1:length(db_feats)
        [B, U] = compressSPLH(db_feats{k}, SPLHparam);
        db_codes{k} = double(U);
    end
    query_codes = cell(size(query_feats));
    for k=1:length(query_feats)
        [B, U] = compressSPLH(query_feats{k}, SPLHparam);
        query_codes{k} = double(U);
    end
    dists = match_hamming(query_codes, db_codes);
    [apr, ar, ~] = search_eval(query_ids, db_ids, dists, 0);
    fprintf('top1 accuracy: %f\n', apr(1));
    eval_res = [eval_res; apr];
   
end

%% diagnose

% similarity matrix
sim_diffs = [];
for i=2:length(sim_imgs)
    f = imagesc(sim_imgs{i});
    saveas(f, sprintf('res/youtube_simmat_%d.png', i));
    diff_val = abs(sim_imgs{i}-sim_imgs{i-1});
    sim_diffs = [sim_diffs; sum(diff_val(:))];
end

% code matrix
code_diffs = [];
for i=2:length(iter_codes)
    f = imagesc(iter_codes{i});
    saveas(f, sprintf('res/youtube_codemat_%d.png', i));
    diff_val = abs(iter_codes{i}-iter_codes{i-1});
    code_diffs = [code_diffs; sum(diff_val(:))];
end

figure
subplot(1,2,1)
plot(1:length(sim_diffs), sim_diffs, 'r-')
hold on
title('similarity matrix')
subplot(1,2,2)
plot(1:length(code_diffs), code_diffs, 'k-')
hold on
title('code matrix')


%% evaluate search performance

figure
plot(1:length(eval_res), eval_res, 'r-')
xlabel('iteration')
title('top-1 recognition results')
hold on



