function [ dist_scores ] = face_match_clf( totrain, probe_feats, probe_ids, gal_feats, gal_ids )
%FACE_MATCH_CLF Summary of this function goes here
%   use classifiers to measure similarity

bitnum = 12;
pool_size = 30;
svm_fn = sprintf('youtube_svms_%db.mat', bitnum);

if totrain == 1
    %% prepare data
    train_data = vertcat(gal_feats{:});
    train_ids = [];     % corresponding id
    train_samp_num = zeros(length(gal_feats), 1);
    for i=1:length(gal_feats)
        train_ids = [train_ids; gal_ids(i)*ones(size(gal_feats{i},1),1)];
        train_samp_num(i) = size(gal_feats{i},1);
    end
    train_samp_num = [1; cumsum(train_samp_num)];

    test_data = vertcat(probe_feats{:});
    test_ids = [];
    test_samp_num = zeros(length(probe_feats), 1);
    for i=1:length(probe_feats)
        test_ids = [test_ids; probe_ids(i)*ones(size(probe_feats{i},1),1)];
        test_samp_num(i) = size(probe_feats{i},1);
    end
    test_samp_num = [1; cumsum(test_samp_num)];
    
    fprintf('sample created.\n');
    %% train binary svms
    % randomly select pool sets
    train_cls_ids = randsample(1:length(gal_feats), pool_size);
    svms = cell(length(train_cls_ids), 1);
    % sample_num x bitnum
    code_pool = zeros(size(train_data, 1), length(pool_size));
    % train binary svm for selected sets
    for i=1:length(train_cls_ids)
        cls_id = train_cls_ids(i);
        train_labels = zeros(size(train_data,1),1);
        train_labels(find(train_ids==cls_id)) = 1;
        % samples in test data with the same id
        ids = find(test_ids==cls_id);
        test_labels = zeros(size(test_data,1),1);
        test_labels(ids) = 1;
        
        fprintf('training %dth svm\n', i);
        svm = fitcsvm(train_data, train_labels, 'KernelFunction', 'linear', 'Standardize', true, 'Verbose', 1);
        % test prediction
        [labels, ~] = predict(svm, test_data);
        % compute precision
        accuracy = sum(labels == test_labels) / length(test_labels);
        disp([num2str(i) 'th prediction accuracy: ' num2str(accuracy)]);
        svms{i}.model = svm;
        % generate hash bits using svm
        [labels, ~] = predict(svm, train_data);
        for j=1:length(labels)
            code_pool(j,i) = labels(j);
        end
    end

    %% greedily select hash bits
    optimal_codes = zeros(size(train_data, 1), bitnum);
    optimal_svms = zeros(bitnum, 1);
    for k=1:bitnum
        bestval = inf;
        bestbit = 0;
        for i=1:size(code_pool, 2)
            tmp_codes = optimal_codes;
            for j=1:size(code_pool, 1)
                tmp_codes(j,k) = code_pool(j,i);
            end
            cur_val = eval_objective(tmp_codes, train_ids);
            if cur_val < bestval
                bestval = cur_val;
                bestbit = i;
            end
            disp(i);
        end
        % add optimal codes
        for j=1:size(code_pool, 1)
            optimal_codes(j,k) = code_pool(j,bestbit);
        end
        optimal_svms(k) = bestbit;
        code_pool(:,bestbit) = [];
    end

    clear code_pool
    clear optimal_codes

    save(svm_fn, 'svms', 'optimal_svms',  '-v7.3');

else
    tmp = load(svm_fn);
    svms = tmp.svms;
    optimal_svms = tmp.optimal_svms;
end

%% compute similarity matrix

% compute codes for db
db_codes = cell(length(gal_feats), 1);
for i=1:length(gal_feats)
    db_codes{i} = zeros(size(gal_feats{i},1), bitnum);
    for j=1:bitnum
        [db_codes{i}(:,j), ~] = predict(svms{optimal_svms(j)}.model, gal_feats{i});
    end
end

% compute codes for query
query_codes = cell(length(probe_feats), 1);
for i=1:length(probe_feats)
    query_codes{i} = zeros(size(probe_feats{i},1), bitnum);
    for j=1:bitnum
        [query_codes{i}(:,j), ~] = predict(svms{optimal_svms(j)}.model, probe_feats{i});
    end
end

% compute hamming distance
dist_scores = zeros(size(probe_feats,1), size(gal_feats,1));
for i=1:length(query_codes)
    for j=1:length(db_codes)
        dist_scores(i,j) = set_hamming_dist(query_codes{i}, db_codes{j});
    end
end

end


%%
function dist = set_hamming_dist(set_codes1, set_codes2)
    dist = zeros(size(set_codes2,1),1);
    for i=1:size(set_codes1,1)
        repcode = repmat(set_codes1(i,:), size(set_codes2,1), 1);
        diff = sum(abs(repcode-set_codes2),2);
        dist(i) = min(diff) + 1./(sum(diff==min(diff))+0.001);
    end
    dist = mean(dist);
end
