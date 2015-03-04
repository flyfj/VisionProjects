function visualize_imgs(type, fns, imgmat, imgsz)
    % each row in imgmat is an image
    
    if type == 0
        figure
        for i=1:length(fns)
            img = imread(fns{i});
            img = imresize(img, imgsz);
            subplot(1, length(fns), i)
            imshow(img, [])
            colormap jet
            hold off
        end
    else
        figure
        for i=1:size(imgmat,1)
            img = imgmat(i,:);
            img = reshape(img, imgsz);
            subplot(1, size(imgmat,1), i)
            imshow(img, [])
            hold off
        end
    end

end