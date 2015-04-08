function evaluation_info=evaluate_SH_MNIST(MNIST_trndata,MNIST_trnlabel,MNIST_tstdata,MNIST_tstlabel,SHparam)

tmp_T=cputime;
SHparam = trainSH(double(MNIST_trndata), SHparam);
%%% Training Time
traintime=cputime-tmp_T;
evaluation_info.trainT=traintime;

tmp_T=cputime;
[npoint,ndim,nbat] = size(MNIST_trndata);  
%%% Compression Time
[B_trn, U] = compressSH(double(MNIST_trndata), SHparam);
[B_tst, U] = compressSH(double(MNIST_tstdata), SHparam);
compressiontime=cputime-tmp_T;
evaluation_info.compressT=compressiontime;

%for n=1:100
for n = 1:length(MNIST_tstlabel)
    % compute your distance
    D_code = hammingDist(B_tst(n,:),B_trn);
    
    % get groundtruth sorting
    D_truth =MNIST_trnlabel;
    
    % evaluation
    [p, r, apM, ahd,ap, ph2] = prcal_MNIST(D_code,D_truth,MNIST_tstlabel(n));
    precision(n,:)=p;
    recall(n,:)=r;    
    ar_all(n,:)=apM;
    ahd_all(n,:)=ahd;
    ap_all(n)=ap;
    ph2_all(n)=ph2;
end
evaluation_info.recall=mean(recall);
evaluation_info.precision=mean(precision);
evaluation_info.AR=mean(ar_all);
evaluation_info.AHD=mean(ahd_all);
evaluation_info.AP=mean(ap_all);
evaluation_info.PH2=mean(ph2_all);


