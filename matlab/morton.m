 function ind=morton(n);
 % MORTON(N) return the morton permutation order for array of size 2^N
 % e.g.
 % matrix size is 2^n
 % n=2;
 % ind=morton(n);
 % d=fix(rand(2^n,2^n)*10);
 % disp(d)
 % disp(d(ind))
 linind4=(1:4^n)-1; %start index count for array at zero
 ind4str=dec2base(linind4,4); %convert indices to base-4
 b1=dec2bin(str2num(ind4str(:,1))); %split each base-4 into two base-2numbers
 b2=dec2bin(str2num(ind4str(:,2)));
 rb=[b1(:,1) b2(:,1)]; %the rows are given by the first bits of b1 and
 b2
 cb=[b1(:,2) b2(:,2)]; %the columns are given by the second bits of b1 and b2
 r=bin2dec(rb)+1; %convert the row from bit to decimal
 c=bin2dec(cb)+1; %convert column
 ind=[2^n*(c-1)+r]'; %make a linear row index into array for easyaddressing
 %morton.m ends