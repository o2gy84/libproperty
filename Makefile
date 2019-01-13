all: world

CLANG_USES_EXTERNAL_STDLIB=0

ifeq ($(CLANG_USES_EXTERNAL_STDLIB), 1)
  clang_uses_ext_stdlib=1
else
  clang_uses_ext_stdlib=0
endif

world:
	@ make clang_uses_ext_stdlib=$(clang_uses_ext_stdlib) -C src
	@ make -C t
	@ find src/ -name "*.hpp" -exec ./cpplint.py --quiet {} \;
	@ find src/ -name "*.cpp" -exec ./cpplint.py --quiet {} \;

clangtest:
	#@ TODO: !!!
	@ $(shell ./clang_test.sh)

.PHONY: clangtest
