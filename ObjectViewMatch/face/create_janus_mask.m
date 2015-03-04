function [ mask ] = create_janus_mask( probe_ids, gal_ids )
%CREATE_JANUS_MASK Summary of this function goes here
%   Detailed explanation goes here

mask = zeros(length(probe_ids), length(gal_ids));

for i=1:length(probe_ids)
    for j=1:length(gal_ids)
        if probe_ids(i) == gal_ids(j)
            mask(i,j) = 255; % 0xff;
        else
            mask(i,j) = 127; % 0x7x
        end
    end
end

mask = uint8(mask);

end

