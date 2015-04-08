%%% Unsupervised Sequential Projection Learning for Hashing (USPLH) 
%%% Reference: Jun Wang, Sanjiv Kumar, Shih-Fu Chang. Sequential Projection Learning
%%%% for Hashing with Compact Codes. In ICML, 2010
%%% by Jun Wang EE@Columbia (jwang@ee.columbia.edu);

function evaluation_info=evaluate_USPLH_SIFT(SIFT_trndata,SIFT_tstdata,KNN_info,USPLHparam)

%%% Unsupervised Sequential Projection Learning for Hashing (USPLH) 

%%% Dependency 
%%% USPLHparam = trainUSPLH(trndata), USPLHparam); 
%%%              train hash functions

%%% Input:
%%%       --- SIFT_trndata:  input the training sift data set 1M X 128 matrix
%%%                       n by d matrix (n: number of samples; d: feature dimension)
%%%       --- SIFT_tstdata:  test data 10K X 128 matrix
%%%                       n by d matrix (n: number of samples; d: feature dimension);
%%%       --- KNN_info:   ground truth of nearest neighbor information
%%%                       for each test point, record the nearest 2% points
%%%                       in 1 M database, i.e. 20K
%%%                   knn_p2   sorted index of the ground truth 10K X 20K 
%%%                   dis_p2   sorted distance of the ground truth 10K X 20K 
%%%       --- USPLHparam: parameters of the algorithm
%%%                  nbits: number of hash bits
%%%                  eta:   sequential learning rate
%%%                  blocknum: the real tested queries in number of blocks  
%%% Output:
%%%       --- evaluation_info:    evaulation results;
%%%                trainT: training time
%%%                compressT: compression time
%%%                recall: recall curve
%%%                precision: precision of top [10, 50 100, 200, 300, 400, 500, 1000,2000,10000];
%%%                AP: average precision
%%%                APH2: mean precision in hamming radius 2
%%%                USPLHparam: strucutre to strore learned hash functions
%%% by J. Wang (jwang@ee.columbia.edu)
%%% Last update Sep, 2010

tmp_T=cputime;
USPLHparam = trainUSPLH(double(SIFT_trndata), USPLHparam);  %%% trani hash functions
%%% Training Time
traintime=cputime-tmp_T;
evaluation_info.trainT=traintime;

tmp_T=cputime;
[npoint,ndim,nbat] = size(SIFT_trndata);  
%%% Compression Time
[B_trn, U] = compressUSPLH(double(SIFT_trndata), USPLHparam);  %%% compress hash codes for training data
[B_tst, U] = compressUSPLH(double(SIFT_tstdata), USPLHparam);  %%% compress hash codes for test data
compressiontime=cputime-tmp_T;
evaluation_info.compressT=compressiontime;


test_num=size(SIFT_tstdata,1);
block_size=100;
block_num=USPLHparam.blocknum;
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

evaluation_info.USPLHparam=USPLHparam;