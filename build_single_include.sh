#!/bin/bash

infiles=(Errors Helpers Boxing Julia)
outdir=single_include
outfile=julia
outpath="$outdir/$outfile.hpp"
outcontent=""

mkdir -p $outdir
touch $outpath
> $outpath

for ((i = 0; i < 4; i++)); do
  cat "src/${infiles[i]}.hpp" >> $outpath
done

perl -pi -e 's/(#pragma once\n|#include ".+\.hpp"\n)//g' single_include/julia.hpp
