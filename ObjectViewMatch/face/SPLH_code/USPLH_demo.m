%%%% Demo code for unsupervised sequential projection learning for hashing
%%%% Jun Wang, Sanjiv Kumar, Shih-Fu Chang. Sequential Projection Learning
%%%% for Hashing with Compact Codes. In ICML, 2010
%%%% This code runs the hashing test on 1 M SIFT data. It requires
%%%% considerable computation time and computer memory
%%%% by Jun Wang (jwang@ee.columbia.edu)
%%%% Sep. 2010


clear;clc;
path(path,'USPLH');
path(path,'SH');

%%%% load sift data 
load SIFT_gnd_release.mat;

nbits=48;
blocknum=2;

USPLHparam.nbits=nbits; %%% number of hash bits
%%% sequential learning parameter, please see algorithm 2
%%% this is the one you might spend some time to tune a little bit. 
USPLHparam.eta=0.125;
%%% number of block data used for testing
USPLHparam.blocknum=blocknum;  %%% means test on 2*100 queries
USPLH_eva_info =evaluate_USPLH_SIFT(SIFT_trndata,SIFT_tstdata,KNN_info,USPLHparam);

SHparam.nbits=nbits;
SHparam.blocknum=blocknum;
SH_eva_info =evaluate_SH_SIFT(SIFT_trndata,SIFT_tstdata,KNN_info,SHparam);
%%% see how the hash projections corrlelated. 
A=USPLH_eva_info.USPLHparam.w'*USPLH_eva_info.USPLHparam.w; 
imshow(uint8(255*abs(A)));
xlabel('Correlation of USPLH projections');

figure; hold on;
plot(SH_eva_info.recall,'r-');
plot(USPLH_eva_info.recall,'k-');
xlabel('M');
ylabel('Recall');
legend('SH','USPLH');

M_set=[10, 50 100, 200, 300, 400, 500, 1000,2000,10000];
figure; hold on;
plot(M_set,SH_eva_info.precision,'r-d');
plot(M_set,USPLH_eva_info.precision,'k-o');
xlabel('M');
ylabel('Precision');
legend('SH','USPLH');

