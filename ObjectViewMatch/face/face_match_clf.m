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
    train_data = [train_data; gal_feats{i}./255];
    train_labels = [train_labels; repmat(gal_ids(i), size(gal_feats{i},1), 1)];
end
for i=1:length(probe_feats)
    test_data = [test_data; probe_feats{i}./255];
    test_labels = [test_labels; repmat(probe_ids(i), size(probe_feats{i},1), 1)];
end

train_labels = double(train_labels);
test_labels = double(test_labels);

model = svmtrain(train_labels, train_data, '-v 5');
[predicted_label, accuracy, prob_estimates] = svmpredict(test_labels, test_data, model);




end

