
function a = fromindex(zp,mode,ii)

if isempty(ii)
    a=ii;
    return;
end
switch mode
    case 'index'
        a=ii;
    case 'signed'
        a = (zp.signed{1}(ii));
    case 'unsigned'
        a = (zp.unsigned{1}(ii));
    case 'float'
        a = zp.float{1}(ii);
    otherwise
        a = 0;
end