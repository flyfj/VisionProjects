function [ output_args ] = visualize_clusters( cls_fns )
%VISUALIZE_CLUSTERS Summary of this function goes here
%   Detailed explanation goes here

% max cluster size
max_cls_num = 0;
for i=1:length(cls_fns)
    max_cls_num = max(length(cls_fns{i}), max_cls_num);
end

figure
for i=1:length(cls_fns)
    for j=1:length(cls_fns{i})
        subplot(length(cls_fns), max_cls_num, (i-1)*max_cls_num+j)
        img = imread(cls_fns{i}{j});
        img = imresize(img, [64 64]);
        imshow(img, [])
    end
end

colormap jet

end

