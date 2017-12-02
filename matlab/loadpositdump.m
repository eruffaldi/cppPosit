function q = loadpositdump(name,bits,es)

if nargin < 2
    bits=0;
    es=0;
end
% iIv
fid =fopen(name,'rb');
r =fread(fid,Inf,'double');
r =reshape(r,3,[])';
q = dump2table(r);
q.bits=bits;
q.es=es;
q.what=categorical({'posit'},{'posit','float','valid'});

