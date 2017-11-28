function r = loadpositdump(name)

% iIv
fid =fopen(name,'rb');
r =fread(fid,Inf,'double');
r =reshape(r,3,[])';
