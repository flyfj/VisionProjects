
%% process synthesized depth map

root_dir = 'f:\';
save_dir = '';
norm_sz = [128 128];    % [256, 256]

% classify images based on filename
fns = dir([root_dir '*.png']);
for i=1:size(fns,1)
    cur_fn = fns(i).name;
    cur_fn_path = [root_dir cur_fn];
    img = imread(cur_fn_path);
    grayimg = rgb2gray(img);
    [imgw, imgh] = size(grayimg);
    % get object bounding box
    % minx, miny, maxx, maxy
    bbox = [intmax, intmax, 0, 0];
    for r=0:imgh
        for c=0:imgh
            if(grayimg(r,c) ~= 0)
                bbox(1) = min(bbox(1), c);
                bbox(2) = min(bbox(2), r);
                bbox(3) = max(bbox(3), c);
                bbox(4) = max(bbox(4), r);
            end
        end
    end
    bbox(3) = bbox(3)-bbox(1)+1;
    bbox(4) = bbox(4)-bbox(2)+1;
    
    % tighten object, normalize size
    newimg = imcrop(grayimg, bbox);
    newimg = imresize(newimg, norm_sz);
    
    imwrite(newimg, [save_dir cur_fn]);
    
end
