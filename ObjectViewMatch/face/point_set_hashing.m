
%% load data
[db_feats, db_ids, query_feats, query_ids] = prepare_face_data('honda');

% stack all
db_feats_all = [];
db_ids_all = [];
for i=1:length(db_feats)
    db_feats_all = [db_feats_all; db_feats{i}];
    db_ids_all = [db_ids_all; ones(size(db_feats{i},1), 1)*db_ids(i)];
end


%% iteration
obj_vals = zeros(10, 1);
db_codes_all = [];
for iter=1:10
    
    % create similarity matrix
    sim_mat = 0 * ones(size(db_feats_all,1), size(db_feats_all,1));
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
        diffsetids = find(db_ids_all~=db_ids_all(i));
%         sim_mat(i, cursetids) = 0;
        mindist = min(dists(i, cursetids));
        sameminids = find(dists(i,cursetids)==mindist);
        objval = mindist;
        sim_mat(i, sameminids) = 1;
        mindist = min(dists(i, diffsetids));
        sameminids = find(dists(i,diffsetids)==mindist);
        sim_mat(i, sameminids) = -1;
        obj_vals(iter) = objval - mindist;
    end

    % learn hash functions/codes
    SPLHparam.nbits = 12;
    SPLHparam.eta = 0.2;
    SPLHparam = trainSPLH(db_feats_all, SPLHparam, db_feats_all', sim_mat);
    [B, U] = compressSPLH(db_feats_all, SPLHparam);
    
    % evalute objective
    db_codes_all = double(U);
%     obj_val = trace(db_codes_all'*sim_mat*db_codes_all) ./ size(db_codes_all,1);
%     obj_vals(iter) = obj_val;
    obj_vals
end

%% evaluate search performance





%% helper functions
