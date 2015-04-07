
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
sim_imgs = cell(10,1);
for iter=1:10
    
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
        diff = mindist1 - mean(dists(i,diffsetids));
        if diff > 0
            diff = diff;
        end
        obj_vals(iter) = obj_vals(iter) + diff;
    end
    sim_imgs{iter} = sim_mat;
    obj_vals(iter) = obj_vals(iter) ./ size(dists,1);

    for i=1:iter
        subplot(1, iter, i)
        imshow(sim_mats{i}, [])
        hold on
        colormap jet
        hold on
    end
    pause
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
