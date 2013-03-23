function [gist, param] = LMgist_simple(D, HOMEIMAGES, param, HOMEGIST)
%
% [gist, param] = LMgist(D, HOMEIMAGES, param);
% [gist, param] = LMgist(filename, HOMEIMAGES, param);
% [gist, param] = LMgist(filename, HOMEIMAGES, param, HOMEGIST);
%
% For a set of images:
% gist = LMgist(img, [], param);
%
% When calling LMgist with a fourth argument it will store the gists in a
% new folder structure mirroring the folder structure of the images. Then,
% when called again, if the gist files already exist, it will just read
% them without recomputing them:
%
%   [gist, param] = LMgist(filename, HOMEIMAGES, param, HOMEGIST);
%   [gist, param] = LMgist(D, HOMEIMAGES, param, HOMEGIST);
% 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Modeling the shape of the scene: a holistic representation of the spatial envelope
% Aude Oliva, Antonio Torralba
% International Journal of Computer Vision, Vol. 42(3): 145-175, 2001.

if isnumeric(D)
    % [gist, param] = LMgist(img, HOMEIMAGES, param);
    Nscenes = size(D,4);
    typeD = 3;
    if ~isfield(param, 'imageSize')
        param.imageSize = [size(D,1) size(D,2)];
    end
end
    
param.boundaryExtension = 32; % number of pixels to pad

if nargin<3
    % Default parameters
    param.imageSize = 128;
    param.orientationsPerScale = [8 8 8 8]; % 32 total
    param.numberBlocks = 4;
    param.fc_prefilt = 4;
    param.G = createGabor(param.orientationsPerScale, param.imageSize+2*param.boundaryExtension);
else
    % Precompute filter transfert functions (only need to do this once, unless
    % image size is changes):
    if ~isfield(param, 'G')
        param.G = createGabor(param.orientationsPerScale, param.imageSize+2*param.boundaryExtension);
    end
end


Nfeatures = size(param.G, 3) * param.numberBlocks^2;

% Loop: Compute gist features for all scenes
gist = zeros([Nscenes Nfeatures], 'single');
for n = 1:Nscenes
    g = [];
    todo = 1;
    
    % otherwise compute gist
    if todo==1
        if Nscenes>1 disp([n Nscenes]); end
        
        img = D(:,:,:,n);
        
        % convert to gray scale
        img = single(mean(img,3));

        % resize and crop image to make it square
        img = imresizecrop(img, param.imageSize, 'bilinear');
        %img = imresize(img, param.imageSize, 'bilinear'); %jhhays

        % scale intensities to be in the range [0 255]. min->0, max->255
        img = img-min(img(:));
        img = 255*img/max(img(:));

        % prefiltering: local contrast scaling
        output = prefilt(img, param.fc_prefilt);

        % get gist:
        g = gistGabor(output, param);
        
    end
    
    gist(n,:) = g;
    drawnow
end




function g = gistGabor(img, param)
% 
% Input:
%   img = input image (it can be a block: [nrows, ncols, c, Nimages])
%   param.w = number of windows (w*w)
%   param.G = precomputed transfer functions
%
% Output:
%   g: are the global features = [Nfeatures Nimages], 
%                    Nfeatures = w*w*Nfilters*c

img = single(img);

w = param.numberBlocks;
G = param.G;
be = param.boundaryExtension;

if ndims(img)==2
    c = 1; 
    N = 1;
    [nrows ncols c] = size(img);
end
if ndims(img)==3
    [nrows ncols c] = size(img);
    N = c;
end
if ndims(img)==4
    [nrows ncols c N] = size(img);
    img = reshape(img, [nrows ncols c*N]);
    N = c*N;
end

[ny nx Nfilters] = size(G);
W = w*w;
g = zeros([W*Nfilters N]);

% pad image
img = padarray(img, [be be], 'symmetric');

img = single(fft2(img)); 
k=0;
for n = 1:Nfilters % do each filtering for all blocks 16
    ig = abs(ifft2(img.*repmat(G(:,:,n), [1 1 N]))); % use nth filter to convolute with image
    ig = ig(be+1:ny-be, be+1:nx-be, :); % crop original region
    
    v = downN(ig, w);   % compute average value for each block; average response for current orientation feature
    g(k+1:k+W,:) = reshape(v, [W N]);   % concatenate filtered blocks
    k = k + W;
    drawnow
end


function y=downN(x, N)
% 
% averaging over non-overlapping square image blocks
%
% Input
%   x = [nrows ncols nchanels]
% Output
%   y = [N N nchanels]

nx = fix(linspace(0,size(x,1),N+1));
ny = fix(linspace(0,size(x,2),N+1));
y  = zeros(N, N, size(x,3));
for xx=1:N
  for yy=1:N
    v=mean(mean(x(nx(xx)+1:nx(xx+1), ny(yy)+1:ny(yy+1),:),1),2);
    y(xx,yy,:)=v(:);
  end
end


function G = createGabor(or, n)
%
% G = createGabor(numberOfOrientationsPerScale, n);
%
% Precomputes filter transfer functions. All computations are done on the
% Fourier domain. 
%
% If you call this function without output arguments it will show the
% tiling of the Fourier domain.
%
% Input
%     numberOfOrientationsPerScale = vector that contains the number of
%                                orientations at each scale (from HF to BF)
%     n = imagesize = [nrows ncols] 
%
% output
%     G = transfer functions for a jet of gabor filters


Nscales = length(or);
Nfilters = sum(or);

if length(n) == 1
    n = [n(1) n(1)];
end

l=0;
for i=1:Nscales
    for j=1:or(i)
        l=l+1;
        param(l,:)=[.35 .3/(1.85^(i-1)) 16*or(i)^2/32^2 pi/(or(i))*(j-1)];
    end
end

% Frequencies:
%[fx, fy] = meshgrid(-n/2:n/2-1);
[fx, fy] = meshgrid(-n(2)/2:n(2)/2-1, -n(1)/2:n(1)/2-1);
fr = fftshift(sqrt(fx.^2+fy.^2));
t = fftshift(angle(fx+sqrt(-1)*fy));

% Transfer functions:
G=zeros([n(1) n(2) Nfilters]);
for i=1:Nfilters
    tr=t+param(i,4); 
    tr=tr+2*pi*(tr<-pi)-2*pi*(tr>pi);

    G(:,:,i)=exp(-10*param(i,1)*(fr/n(2)/param(i,2)-1).^2-2*param(i,3)*pi*tr.^2);
end


if nargout == 0
    figure
    for i=1:Nfilters
        contour(fx, fy, fftshift(G(:,:,i)),[1 .7 .6],'r');
        hold on
    end
    axis('on')
    axis('equal')
    axis([-n(2)/2 n(2)/2 -n(1)/2 n(1)/2])
    axis('ij')
    xlabel('f_x (cycles per image)')
    ylabel('f_y (cycles per image)')
    grid on
end


