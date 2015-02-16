function [ db_feats, db_ids, query_feats, query_ids ] = prepare_face_data( db_name )
%PREPARE_FACE_DB Summary of this function goes here
%   load face dataset

if strcmp(db_name, 'honda')
    data_fn = 'F:\Recognition\Honda_processed.mat';
    data = load(data_fn);
    db_feats = [];
    db_ids = [];
    % get train data
    for i=1:length(data.nTrainSetIndices)
        cur_id = data.nTrainSetIndices(i);
        db_feats = [db_feats; data.ImgData{cur_id}'];
        db_ids = [db_ids; ones(size(data.ImgData{cur_id},2), 1) * i];
    end
    % get query data
    query_feats = [];
    query_ids = [];
    for i=1:length(data.nTestSetIndices)
        cur_id = data.nTestSetIndices(i);
        tmp = find(data.nTrainSetIndices > cur_id);
        db_id = tmp(1);
        query_feats = [query_feats; data.ImgData{cur_id}'];
        query_ids = [query_ids; ones(size(data.ImgData{cur_id},2), 1) * db_id];
    end
    
    disp('honda data ready.');
    
end


end

