FLAGS='--std=c++14 -O3 --march=native -Werror -Wall -Wno-long-long -pedantic';
mex('floatTop8.cpp',['COMPFLAGS="$COMPFLAGS ' FLAGS]);
mex('p8Tofloat.cpp',['COMPFLAGS="$COMPFLAGS ' FLAGS]);
mex('p8binop.cpp',['COMPFLAGS="$COMPFLAGS ' FLAGS]);
mex('p8unop.cpp',['COMPFLAGS="$COMPFLAGS ' FLAGS]);
