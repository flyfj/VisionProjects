function returned_set=HashLookup_SSH(Dh,options_lookup)

knn_set=options_lookup.knn_set;
maxsize_bucket=100000;

switch options_lookup.method
    case 'ranking'
        for jj=1:size(Dh,1)
            returned_ind=[];
            returned_hd=[];
            search_radius=0;
            while length(returned_ind)<knn_set(end)
                if search_radius==0
                    temp_ind=find(Dh(jj,:)<=search_radius); 
                    returned_ind=[returned_ind temp_ind];
                    returned_hd=[returned_hd search_radius*ones(1,length(temp_ind))];
                else
                    temp_ind=find((Dh(jj,:)<=search_radius)&(Dh(jj,:)>search_radius-1)); 
                    returned_ind=[returned_ind temp_ind];
                    returned_hd=[returned_hd search_radius*ones(1,length(temp_ind))];
                end
                search_radius=search_radius+1;
            end
            for kk=1:length(knn_set)            
                returned_set(jj,kk).retset=returned_ind(1:knn_set(kk));     
                returned_set(jj,kk).hd=returned_hd(1:knn_set(kk));
            end
        end
        
        
        
    case 'Dhdis'   
        for jj=1:size(Dh,1)    
            ind_0=find(Dh(jj,:)<=0); 
            len_0=length(ind_0);        
            ind_1=find((Dh(jj,:)<=1)&(Dh(jj,:)>0)); 
            len_1=length(ind_1);        
            ind_2=find((Dh(jj,:)<=2)&(Dh(jj,:)>1)); 
            len_2=length(ind_2);              
            for kk=1:length(knn_set)             
                returned_set(jj,kk).retset=[];
                if len_0>=knn_set(kk) 
                    returned_set(jj,kk).retset=ind_0;  
                end
                if len_0<knn_set(kk)&length(ind_1)<hs_bound
                    returned_set(jj,kk).retset=[ind_0 ind_1];
                end
                if (len_0+len_1)<knn_set(kk)&length(ind_2)<hs_bound
                    returned_set(jj,kk).retset=[ind_0 ind_1 ind_2];
                end          
            end
        end
        
    case 'FixDh'   
        for jj=1:size(Dh,1)    
            ind_0=find(Dh(jj,:)<=0); 
            len_0=length(ind_0);     
%             if len_0>maxsize_bucket
%                 returned_set(jj).retset=[];
%                 returned_set(jj).hd=-1;
%                 continue;
%             end                
               
            ind_1=find((Dh(jj,:)<=1)&(Dh(jj,:)>0)); 
            len_1=length(ind_1);   
%             if len_1>maxsize_bucket
%                 returned_set(jj).retset=ind_0;
%                 returned_set(jj).hd=zeros(1,len_0);
%                 continue;
%             end              
            
            ind_2=find((Dh(jj,:)<=2)&(Dh(jj,:)>1)); 
            len_2=length(ind_2);     
%             if len_2>maxsize_bucket
%                 returned_set(jj).retset=[ind_0 ind_1];
%                 returned_set(jj).hd=[zeros(1,len_0) ones(1, len_1)];
%                 continue;
%             end              
            
            returned_set(jj).retset=[ind_0 ind_1 ind_2];
            returned_set(jj).hd=[zeros(1, len_0) ones(1, len_1) 2*ones(1, len_2)];
        end        
        
end