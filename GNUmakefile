cJSON-bench:    json-shootout.c
	gcc json-shootout.c -lcjson -o cJSON-bench

sxe-jitson-bench.o:    json-shootout.c ../sxe/libsxe/build-linux-64-release/include/sxe-jitson.h
	gcc -DSXE_JITSON=1 -I../sxe/libsxe/build-linux-64-release/include -g -Og -c json-shootout.c -o sxe-jitson-bench.o

sxe-jitson-bench:    sxe-jitson-bench.o ../sxe/libsxe/build-linux-64-release/libsxe.a
	gcc -g -Og sxe-jitson-bench.o ../sxe/libsxe/build-linux-64-release/libsxe.a -lxxhash -o sxe-jitson-bench
