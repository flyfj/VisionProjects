% pipeline of running janus data from loading to evaluation

%% load janus data
[ gal_feats, gal_ids, probe_feats, probe_ids ] = prepare_janus_data( 1, 'A', 1 );


%% compute matching score

sim_scores = face_match_l2(probe_feats, gal_feats);


%% evaluation

% convert format
score_fn = 'test_1A.mtx';
mask_fn = 'test_1A.mask';
eval_fn = 'eval_1A.csv';
to_janus_mat_format(sim_scores, score_fn, 0);

mask = create_janus_mask(probe_ids, gal_ids);
to_janus_mat_format(mask, mask_fn, 1);

% call openbr to produce eval
br_cmd = sprintf('C:\OpenBR-0.5.0-win64\bin\br -eval %s %s %s', score_fn, mask_fn, eval_fn);
system(br_cmd, '-echo')