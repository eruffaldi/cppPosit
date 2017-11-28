function r = loadpositdump(name)

% iIv
fid =fopen(name,'rb');
r =fread(fid,Inf,'double');
r =reshape(r,3,[])';
% TODO
r(:,4) =meandiff(r(:,3));

function r = meandiff(x)
x=x(:);

r =nan(size(x));

% ((next-cur)+(cur-prev))/2 == (next-prev)/2
r(2:end-1) = (x(3:end)-x(1:end-2))/2;

