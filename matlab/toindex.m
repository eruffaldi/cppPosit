
function ii = toindex(zp,mode,a)
if isempty(a)
    ii=a;
    return;
end

switch mode
    case 'index'
        ii =a;
    case 'signed'
        ii = find(zp.signed{1}==a,1,'first');
    case 'unsigned'
        ii = find(zp.unsigned{1}==a,1,'first');
    case 'float'
        %ii = find(zp.float{1}==a,1,'first');
        [~,ii] = min((zp.float{1}-a).^2);
    otherwise
        ii = 0;
        
end
