function [ db_feats, db_ids, query_feats, query_ids ] = prepare_face_data( db_name )
%PREPARE_FACE_DB Summary of this function goes here
%   load face dataset
%   each set is a cell

switch db_name
    case 'honda'
       [db_feats, db_ids, query_feats, query_ids] = prepare_honda_data();
    case 'youtube'
       [db_feats, db_ids, query_feats, query_ids] = prepare_youtube_data();
end

end

