%% distance measure
function score = match_obj_manifold(query_feat, k, obj_manifold)
    % compute reconstruction error
    costs = zeros(1, length(obj_manifold));
    for i=1:length(obj_manifold)
        centered = query_feat - obj_manifold{i}.mean;   % 1xD
        use_comp = min(k, size(obj_manifold{i}.eigs, 1));
        pcs = obj_manifold{i}.eigs(1:use_comp, :);
        proj = pcs * centered';         % Cx1
        recon = proj' * pcs;            % 1xD
        costs(i) = norm(recon - centered);
    end
    
    score = min(costs);
end