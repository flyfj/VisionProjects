function [ sim_scores ] = face_match_clf( probe_feats, probe_ids, gal_feats, gal_ids )
%FACE_MATCH_CLF Summary of this function goes here
%   use classifiers to measure similarity

sim_scores = zeros(size(probe_feats,1), size(gal_feats,1));
sim_scores = single(sim_scores);

% train a multi-class svm for gallery

% form training data
train_data = [];
train_labels = [];
test_data = [];
test_labels = [];

for i=1:length(gal_feats)
    train_data = [train_data; gal_feats{i}];
    train_labels = [train_labels; repmat(gal_ids(i), size(gal_feats{i},1), 1)];
end
for i=1:length(probe_feats)
    test_data = [test_data; probe_feats{i}];
    test_labels = [test_labels; repmat(probe_ids(i), size(probe_feats{i},1), 1)];
end

train_data = double(train_data);
test_data = double(test_data);
train_labels = double(train_labels);
test_labels = double(test_labels);

model = svmtrain(train_labels, train_data, '-b 1');

% compute similarity
for i=1:length(probe_feats)
    test_labels = repmat(probe_ids(i), size(probe_feats{i},1), 1);
    [predicted_label, accuracy, prob_estimates] = svmpredict(double(test_labels), probe_feats{i}, model, '-b 1');
    sim_scores(i,:) = max(prob_estimates, [], 1);
end



end

