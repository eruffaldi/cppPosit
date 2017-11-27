x_i = cast(-128:127,'int8');
x_if = p8Tofloat(x_i,double(0));
x_ifi = floatTop8(x_if);
assert(all(x_i==x_ifi),'conversion');
%%
scatter(x_i,x_if)
xlabel('Posit (uint8)');
ylabel('float');
%%
d_i = p8binop(x_i,x_i,'+');
ds_i = p8binop(x_i,x_i,'*');
di_i = p8unop(x_i,'inv');
d_if = p8Tofloat(d_i,double(0));
di_if = p8Tofloat(di_i,double(0));
ds_if = p8Tofloat(ds_i,double(0));
scatter(x_i,x_if,'r')
hold on
scatter(x_i,d_if,'b')
scatter(x_i,ds_if,'g')
scatter(x_i,di_if,'m')
hold off
xlabel('Posit (uint8)');
ylabel('float');
legend({'x','2x','x**2','inv(x)'});
