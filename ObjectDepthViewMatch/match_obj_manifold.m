%% distance measure
function score = match_obj_manifold(query_feat, obj_manifold)
    % compute reconstruction error
    costs = zeros(1, length(obj_manifold));
%     for i=1:length(obj_manifold)
        centered = query_feat - obj_manifold.mean;   % 1xD
        pcs = obj_manifold.M';
        proj = pcs * centered';         % Cx1
        recon = proj' * pcs;            % 1xD
        score = norm(recon - centered);
%     end
    
%     score = min(costs);
end