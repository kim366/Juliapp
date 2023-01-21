#!/bin/bash
cd "$(dirname "$0")"/"$1"
set -e

lcov --capture --directory CMakeFiles/juliapp.dir/src --include "*/src/*" --output-file coverage.info
genhtml coverage.info --branch-coverage --output-directory coverage-report | tee coverage-meta.txt
