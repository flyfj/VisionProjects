function visualize_imgs(fns)

    figure
    for i=1:length(fns)
        img = imread(fns{i});
        img = imresize(img, [64 64]);
        subplot(1, length(fns), i)
        imshow(img, [])
        colormap jet
        hold off
    end

end