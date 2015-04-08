function evaluation_info=evaluate_SH_SIFT(SIFT_trndata,SIFT_tstdata,KNN_info,SHparam)

tmp_T=cputime;
SHparam = trainSH(double(SIFT_trndata), SHparam);
%%% Training Time
traintime=cputime-tmp_T;
evaluation_info.trainT=traintime;

tmp_T=cputime;
[npoint,ndim,nbat] = size(SIFT_trndata);  
%%% Compression Time
[B_trn, U] = compressSH(double(SIFT_trndata), SHparam);
[B_tst, U] = compressSH(double(SIFT_tstdata), SHparam);
compressiontime=cputime-tmp_T;
evaluation_info.compressT=compressiontime;

test_num=size(SIFT_tstdata,1);
block_size=100;
block_num=SHparam.blocknum;

%block_num=test_num/block_size;
truth_num=20000;
for i_block=1:block_num    
    D_code = hammingDist(B_tst((i_block-1)*block_size+1:i_block*block_size,:),B_trn);
    [foo, j_code] = sort(D_code, 2,'ascend'); 
    % get groundtruth sorting
    for jj=1:block_size
        j_truth(jj,:) = KNN_info.knn_p2((i_block-1)*block_size+jj,:);
        h2_ind=find(D_code(jj,:)<=2);
        if isempty(h2_ind)
            APH2((i_block-1)*block_size+jj)=0;
        else
            APH2((i_block-1)*block_size+jj)=length(intersect(h2_ind,j_truth(jj,1:truth_num)))/length(h2_ind);
        end    
    end
    [R_block,P_block,AP_block] = neighborsRecall_sift(j_truth, j_code);    
    R(i_block,:)=R_block;
    P(i_block,:)=P_block;
    AP(i_block,:)=AP_block;     
end

if block_num>1
    R=mean(R,1);
    P=mean(P,1);
    AP=mean(AP,1);
else
    R=R_block;
    P=P_block;
    AP=AP_block;
end

evaluation_info.recall=R;
evaluation_info.precision=P;
evaluation_info.AP=AP;
evaluation_info.APH2=mean(APH2);


evaluation_info.SHparam=SHparam;
