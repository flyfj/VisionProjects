
%% process synthesized depth map

root_dir = 'F:\3D\DepthMaps\';
save_dir = 'F:\3D\DepthMaps_new\';
norm_sz = [64 64];    % [256, 256]

% classify images based on filename
fns = dir([root_dir '*.png']);
parfor i=1:size(fns,1)
    cur_fn = fns(i).name;
    cur_fn_path = [root_dir cur_fn];
    img = imread(cur_fn_path);
    grayimg = rgb2gray(img);
    [imgh, imgw] = size(grayimg);
    binimg = grayimg > 0;
    % get object bounding box
    stats = regionprops(binimg, 'Area', 'BoundingBox');
    cc_sz = zeros(size(stats,1), 1);
    for j=1:size(cc_sz,1)
        cc_sz(j) = stats(j).Area;
    end
    [~, maxid] = max(cc_sz);
    % tighten object, normalize size
    newimg = imcrop(grayimg, stats(maxid).BoundingBox);
    newimg = imresize(newimg, norm_sz);
    % save
    imwrite(newimg, [save_dir cur_fn]);
    disp([i '/' size(fns,1)]);
end
