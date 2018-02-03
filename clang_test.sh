#!/bin/bash

#cppcoreguidelines-*,google-*,llvm-*,misc-*,modernize-*,readability-*       \

cd src/BUILD &&                                                                  \
for i in $(jq --raw-output '.[] | .file' compile_commands.json);                 \
     do echo $i; clang-tidy-3.9 "$i" -p BUILD/ -header-filter='src'              \
     -checks='boost-*,bugprone-*,clang-analyzer-*,performance-*,'                \
     -- -std=c++14;                                                              \
done
