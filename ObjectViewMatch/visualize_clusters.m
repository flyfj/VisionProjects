function [ output_args ] = visualize_clusters( cls )
%VISUALIZE_CLUSTERS Summary of this function goes here
%   Detailed explanation goes here

% max cluster size
max_member_num = 0;
for i=1:length(cls)
    max_member_num = max(length(cls{i}.fns), max_member_num);
end

figure
for i=1:length(cls)
    for j=1:length(cls{i}.fns)
        subplot(length(cls), max_member_num, (i-1)*max_member_num+j)
        img = imread(cls{i}.fns{j});
        img = imresize(img, [64 64]);
        imshow(img, [])
    end
end

colormap jet

end

