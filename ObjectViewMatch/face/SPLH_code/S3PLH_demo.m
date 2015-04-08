%%%% Demo code for semi-supervised sequential projection learning for
%%%% hashing (S3PLH)
%%%% Jun Wang, Sanjiv Kumar, Shih-Fu Chang. Sequential Projection Learning
%%%% for Hashing with Compact Codes. In ICML, 2010
%%%% This code runs the hashing test on MNIST data. It requires
%%%% considerable computation time and computer memory
%%%% The result should generate similar results presented in Figure 3, and
%%%% Figure 5 (a) (b) -- SH curve and S3PLH curves
%%%% by Jun Wang (jwang@ee.columbia.edu)
%%%% Oct. 2010


clear;clc;
path(path,'S3PLH');
path(path,'SH');

%%%% load MNIST data 
load MNIST_gnd_release.mat;

nbits_set=[12 16 24 32 48 64];


for ii=1:length(nbits_set)    
    nbits=nbits_set(ii);
    ct=cputime;
    fprintf('### The number of bits is %d ### \n',nbits);    
       
    SHparam.nbits=nbits;
    SH_eva_info{ii} =evaluate_SH_MNIST(MNIST_trndata,MNIST_trnlabel,MNIST_tstdata,MNIST_tstlabel, SHparam);
    AR_SH(ii)=SH_eva_info{ii}.AR(end-1);
    AHD_SH(ii)=SH_eva_info{ii}.AHD(end-1);
    AP_SH(ii)=SH_eva_info{ii}.AP;
    PH2_SH(ii)=SH_eva_info{ii}.PH2;
    fprintf('SH %d bits -- Train T: %f, Compress T: %f AR: %f AP: %f PH2: %f \n',nbits,SH_eva_info{ii}.trainT,SH_eva_info{ii}.compressT,AR_SH(ii),AP_SH(ii),PH2_SH(ii));      
    
    ct=cputime;
    SPLHparam.nbits=nbits;
    SPLHparam.eta=0.2;
    SPLH_eva_info{ii} =evaluate_SPLH_MNIST(MNIST_trndata,MNIST_trnlabel,MNIST_tstdata,MNIST_tstlabel,MNIST_vaddata,SR_M,SPLHparam);
    AR_SPLH(ii)=SPLH_eva_info{ii}.AR(end-1);
    AHD_SPLH(ii)=SPLH_eva_info{ii}.AHD(end-1);
    AP_SPLH(ii)=SPLH_eva_info{ii}.AP;
    PH2_SPLH(ii)=SPLH_eva_info{ii}.PH2;
    fprintf('SPLH %d bits -- Train T: %f, Compress T: %f, AR: %f AP: %f PH2: %f \n',nbits,SPLH_eva_info{ii}.trainT,SPLH_eva_info{ii}.compressT,AR_SPLH(ii),AP_SPLH(ii),PH2_SPLH(ii));      
end

%%% Figure 3 (a) SH & SPLH curve
figure;hold on;
plot(nbits_set,AP_SH,'-ro');
plot(nbits_set,AP_SPLH,'-ks');
legend('SH','SPLH');
ylabel('Mean Average Precision');
xlabel('Number of bits');
grid on;
box on;

%%% Figure 3 (b) SH & SPLH curve
figure;hold on;
plot(nbits_set,PH2_SH,'-ro');
plot(nbits_set,PH2_SPLH,'-ks');
legend('SH','SPLH');
ylabel('Precision whithin Hamming radius 2');
xlabel('Number of bits');
grid on;
box on;

%%% Figure 5 (a) SH & SPLH curve
%%% 24 bits Recall Curve
figure;hold on;
plot(SH_eva_info{3}.recall,'r-');
plot(SPLH_eva_info{3}.recall,'k-');
legend('SH','SPLH');
ylabel('Recall');
xlabel('The number of samples');
grid on;

box on;
%%% Figure 5 (b) SH & SPLH curve
%%% 48 bits Recall Curve
figure;hold on;
plot(SH_eva_info{5}.recall,'r-');
plot(SPLH_eva_info{5}.recall,'k-');
legend('SH','SPLH');
ylabel('Recall');
xlabel('The number of samples');
grid on;
box on;