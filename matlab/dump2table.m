function q =dump2table(r)

% TODO
r(:,4) =meandiff(r(:,3));

q = table();
q.signed = {r(:,1)};
q.unsigned = {r(:,2)};
v=r(:,3);
q.float ={v};
q.res = {r(:,4)};
q = describetab(q);


function r = meandiff(x)
x=x(:);

r =nan(size(x));

% ((next-cur)+(cur-prev))/2 == (next-prev)/2
r(2:end-1) = (x(3:end)-x(1:end-2))/2;
