#!/bin/bash

infiles=(Errors Helpers Boxing Julia)
outdir="single_include/juliapp"
outfile=julia
outpath="$outdir/$outfile.hpp"
outcontent=""

mkdir -p $outdir
touch $outpath
> $outpath

for ((i = 0; i < 4; i++)); do
  cat "src/${infiles[i]}.hpp" >> $outpath
done

perl -pi -e 's/(#pragma once\n|#include ".+\.hpp"\n)//g' $outpath

cp "src/conversion.hpp" "$outdir"

for f in $outpath $outdir/conversion.hpp; do
  printf '/*\n\n%s\n%s\n%s\n%s\n\n%s\n\n%s\n*/\n\n%s\n%s\n' "Juliapp" "=======" \
    "Seamlessly embed Julia code in C++ with an API using modern language features." \
    "https://github.com/kim366/Juliapp" "=======" \
    "$(cat LICENSE)" "#pragma once" "$(cat $f)" >$f
done


