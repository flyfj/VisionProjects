function [ db_feats, db_ids, query_feats, query_ids ] = load_youtube_data(  )
%LOAD_YOUTUBE_DATA Summary of this function goes here
%   Detailed explanation goes here

data_fn = 'n:\YouTube_Faces\youtube_face_subset20.mat';
data = load(data_fn);
db_ids = unique(data.train_ids);
db_feats = cell(length(db_ids), 1);
query_ids = unique(data.test_ids);
query_feats = cell(length(query_ids), 1);
for i=1:length(db_ids)
    db_feats{i} = data.train_data(find(db_ids(i)==data.train_ids), :);
end
for i=1:length(query_ids)
    query_feats{i} = data.test_data(find(query_ids(i)==data.test_ids), :);
end

clear data;

end

