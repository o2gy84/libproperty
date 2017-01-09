all:
	@ python ./waf configure ; python ./waf -v --alltests

clean:
	@ rm -f *~ *.o
	python ./waf configure ; python ./waf clean

.PHONY: all clean
