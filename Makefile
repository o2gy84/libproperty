all: world

world:
	@ make -C src
	@ make -C t
	@ find src/ -name "*.hpp" -exec ./cpplint.py --quiet {} \;
	@ find src/ -name "*.cpp" -exec ./cpplint.py --quiet {} \;
