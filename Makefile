all: world

world:
	@ make -C src
	@ make -C t
