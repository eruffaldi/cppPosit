function r = cellvcat(x)

if isempty(x)
    r=[];
else
r =x{1};
for I=2:length(x)
    r=[r;x{I}];
end

end