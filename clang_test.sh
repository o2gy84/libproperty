#!/bin/bash

cd src/BUILD &&                                                                  \
for i in $(jq --raw-output '.[] | .file' compile_commands.json);                 \
     do echo $i; clang-tidy-3.9 "$i" -p BUILD/ -header-filter='dbms|libs'        \
     -checks='boost-*,bugprone-*,clang-analyzer-*,performance-*,                 \
     #cppcoreguidelines-*,google-*,llvm-*,misc-*,modernize-*,readability-*       \
     '                                                                           \
     -- -std=c++14;                                                              \
done
