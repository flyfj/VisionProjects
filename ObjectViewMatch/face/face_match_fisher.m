function [ sim_mat ] = face_match_fisher( totrain, probe_imgs, gal_imgs )
%FACE_MATCH_FISHER Summary of this function goes here
%   Detailed explanation goes here

save_fn = 'janus_fv.mat';

% prepare fv data
if totrain

    dict_sz = 128;

    % sample descriptors
    gal_descs = cell(length(gal_imgs), 1);
    for i=1:length(gal_imgs)
        for j=1:1
            imgsz = size(gal_imgs{i}(j,:), 2);
            img = reshape(gal_imgs{i}(j,:), [sqrt(imgsz) sqrt(imgsz)]);
            feats = getDenseSIFT(img, 'step', 4, 'scales', 2.^(1:-.5:-3));
            gal_descs{i} = feats.descr';
        end
    end
    all_descs = cat(1, gal_descs{:});
    clear gal_descs

    % compute dictionary
    [means, covariances, priors] = vl_gmm(all_descs', dict_sz);
    clear all_descs
    fv_data.means = means;
    fv_data.covariances = covariances;
    fv_data.priors = priors;

    % compute fisher vectors
    gal_fvs = cell(length(gal_imgs), 1);
    for i=1:length(gal_imgs)
        for j=1:size(gal_imgs{i},1)
            imgsz = size(gal_imgs{i}(j,:), 2);
            img = reshape(gal_imgs{i}(j,:), [sqrt(imgsz) sqrt(imgsz)]);
            feats = getDenseSIFT(img, 'step', 4, 'scales', 2.^(1:-.5:-3));
            encoding = vl_fisher(feats.descr, means, covariances, priors, 'Improved');
            gal_fvs{i} = [gal_fvs{i}; encoding(:)'];
        end
    end
    fv_data.gal_fvs = gal_fvs;
    clear gal_fvs
    
    probe_fvs = cell(length(probe_imgs), 1);
    for i=1:length(probe_imgs)
        for j=1:size(probe_imgs{i},1)
            imgsz = size(probe_imgs{i}(j,:), 2);
            img = reshape(probe_imgs{i}(j,:), [sqrt(imgsz) sqrt(imgsz)]);
            feats = getDenseSIFT(img, 'step', 4, 'scales', 2.^(1:-.5:-3));
            encoding = vl_fisher(feats.descr, means, covariances, priors, 'Improved');
            probe_fvs{i} = [probe_fvs{i}; encoding(:)'];
        end
    end
    fv_data.probe_fvs = probe_fvs;
    clear probe_fvs

    % save
    save(save_fn, 'fv_data', '-v7.3');

else
    
    % load
    tmp = load(save_fn);
    fv_data = tmp.fv_data;
    
end


% apply pca (optional)
[gal_mapped, mapping] = compute_mapping(cat(1, fv_data.gal_fvs{:}), 'PCA', 0.9);
gal_fv_pca = cell(size(fv_data.gal_fvs));
probe_fv_pca = cell(size(fv_data.probe_fvs));

cnt = 1;
for i=1:length(fv_data.gal_fvs)
    gal_fv_pca{i} = gal_mapped(cnt: cnt+size(fv_data.gal_fvs{i},1)-1, :);
    cnt = cnt + size(fv_data.gal_fvs{i},1);
end
for i=1:length(fv_data.probe_fvs)
    % mapping
    sub_mean = bsxfun(@minus, fv_data.probe_fvs{i}, mapping.mean);
    probe_fv_pca{i} = sub_mean * mapping.M;
end

% compute distance
sim_mat = face_match_l2(probe_fv_pca, gal_fv_pca);


end

