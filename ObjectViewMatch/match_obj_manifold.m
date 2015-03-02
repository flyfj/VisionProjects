%% distance measure
function score = match_obj_manifold(query_feat, obj_manifold, method)
    switch method
        case 'pca'
            % compute reconstruction error
            centered = query_feat - obj_manifold.mean;   % 1xD
            pcs = obj_manifold.M';
            proj = pcs * centered';         % Cx1
            recon = proj' * pcs;            % 1xD
            score = norm(recon - centered);
        case 'autoencoder'
            % compress
            [recon, mapped] = run_data_through_autoenc(obj_manifold.network, query_feat);
            score = norm(recon - query_feat);
    end
end