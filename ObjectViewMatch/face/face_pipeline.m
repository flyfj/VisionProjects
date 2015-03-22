% pipeline of running face matching

%% load face data

[ db_feats, db_ids, query_feats, query_ids ] = prepare_face_data( 'honda' );

%% compute matching score

% 0: l2; 1: clf; 2:fisher
match_type = 1;

switch match_type
    case 0
        dists = face_match_l2(query_feats, db_feats);
    case 1
        dists = face_match_clf(0, query_feats, query_ids, db_feats, db_ids);
    case 2
        dists = face_match_fisher(0, probe_feats, gal_feats);
end


%% evaluation

[apr, ar, ranks] = search_eval( query_ids, db_ids, dists );

