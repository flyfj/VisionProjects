function evaluation_info=evaluate_SPLH_MNIST(MNIST_trndata,MNIST_trnlabel,MNIST_tstdata,MNIST_tstlabel,MNIST_vaddata,SR_M,SPLHparam)


tmp_T=cputime;
SPLHparam = trainSPLH(double(MNIST_trndata), SPLHparam,double(MNIST_vaddata'),SR_M);
%%% Training Time
traintime=cputime-tmp_T;
evaluation_info.trainT=traintime;

tmp_T=cputime;
[npoint,ndim,nbat] = size(MNIST_trndata);  
%%% Compression Time
[B_trn, U] = compressSPLH(double(MNIST_trndata), SPLHparam);
[B_tst, U] = compressSPLH(double(MNIST_tstdata), SPLHparam);
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
evaluation_info.SPLHparam=SPLHparam;