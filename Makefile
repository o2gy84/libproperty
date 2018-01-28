all: world

world:
	@ make -C src
	@ make -C t
	@ find src/ -name "*.hpp" -o -name "*.cpp" -exec ./cpplint.py --quiet {} \;
