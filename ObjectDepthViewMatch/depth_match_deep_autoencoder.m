
% learn a unified autoencoder using all depth images
db_dir = 'F:\3D\ModelNet\test_db_depth2\';
[db_feats, db_fns, db_cate_names, db_fn_cate_ids, db_obj_names, db_fn_obj_ids] = prepare_depth_db(0, db_dir);

unique_db_obj_ids = unique(db_fn_obj_ids);
unique_db_cate_ids = unique(db_fn_cate_ids);
obj_colors = rand(length(unique_db_obj_ids), 3);
cate_colors = rand(length(unique_db_cate_ids), 3);

% learn deep autoencoder
[mapped_data, mapping] = compute_mapping(db_feats, 'Autoencoder', 2);

%% visualize low-dimensional data
figure
for i=1:length(unique_cate_ids)
    feat_ids = find(unique_cate_ids(i) == db_fn_cate_ids);
    plot(mapped_data(feat_ids,1), mapped_data(feat_ids, 2), 'LineStyle', 'none', 'Marker', 'o', 'MarkerSize', 8, 'color', cate_colors(i,:))
    hold on
end
legend(db_cate_names)
