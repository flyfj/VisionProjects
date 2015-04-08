function SPLHparam = trainSPLH(X, SPLHparam,vad_Data,SR_M)

% Input
%  Sequential Projection Learning Based Hashing
%  USPLHparam.nbits = number of bits (nbits do not need to be a multiple of 8)
%
%  By Jun Wang (jwang@ee.columbia.edu)
%  Initial version July, 2009
%  Following the code style from the following paper
% "spectral hashing", nips 2008
% Last update Jan. 20, 2010

%%% number and dim of data
[Nsamples Ndim] = size(X);

% %% Remove sample mean
sampleMean = mean(X,1);

%%% Store original data
ori_X=X;
nbits = SPLHparam.nbits;

%%% size of the semantic relevance matrix
srm_num=size(SR_M,1);
pca_options.ReducedDim=1;

for i_bit=1:SPLHparam.nbits
    %%% Assume Nsamples>>Ndim  for a realistic large scale problem
    covdata = X'*X;
    covdata=covdata/(Nsamples);     
    C_M=vad_Data*SR_M*vad_Data'/srm_num;
	M=C_M;% +covdata; SPLHparam.eta*
    [eigvector, eigvalue, elapse] = PCA2(M, pca_options);         
    w(:,i_bit)=eigvector;
    deltaS=vad_Data'*w(:,i_bit)*w(:,i_bit)'*vad_Data;
    maskS=(SR_M.*deltaS<0);
    T_deltaS=maskS.*deltaS;
    SR_M=SR_M-T_deltaS;
    projectX(:,i_bit) = X * eigvector;
    X=X-projectX(:,i_bit)*eigvector';      
    %vad_Data=(vad_Data'-vad_Data'*eigvector*eigvector')';   
end

b=zeros(1,nbits);

% 4) store paramaterswwwSPLHparam.mean=sampleMean;
SPLHparam.w = w;
SPLHparam.b = zeros(1,nbits);

%SPLHparam.projected=projectX;
%save(['SPLHparam_' num2str(nbits) '.mat'],'SPLHparam');
