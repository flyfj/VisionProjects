function [ db_feats, db_ids, query_feats, query_ids ] = prepare_face_data( db_name )
%PREPARE_FACE_DB Summary of this function goes here
%   load face dataset
%   each set is a cell

if strcmp(db_name, 'honda')
    data_fn = 'F:\Recognition\ih_HONDA_video_data_label.mat';
    data = load(data_fn);
    labels = data.video_label;
    db_feats = cell(0);
    db_ids = [];
    % get train data
    for i=1:length(data.nTrainSetIndices)
        cur_id = data.nTrainSetIndices(i);
        db_feats = [db_feats; data.ImgData_HE{cur_id}'];
        db_ids = [db_ids; labels(cur_id)];
    end
    % get query data
    query_feats = cell(0);
    query_ids = [];
    for i=1:length(data.nTestSetIndices)
        cur_id = data.nTestSetIndices(i);
%         tmp = find(data.nTrainSetIndices > cur_id);
%         db_id = tmp(1);
        query_feats = [query_feats; data.ImgData_HE{cur_id}'];
        query_ids = [query_ids; labels(cur_id)];
    end
    
    disp('honda data ready.');
    
end
if strcmp(db_name, 'youtube')
    tmp = load('');
    db_feats = cell(0);
    db_ids = [];
    query_feats = cell(0);
    query_ids = [];
end


end

