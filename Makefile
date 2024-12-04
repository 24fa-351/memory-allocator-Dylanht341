all: memtest_sys memtest_dev

memtest_sys: memtest.c
	gcc -DUSE_SYSTEM_MALLOC -o memtest_sys memtest.c

memtest_dev: memtest.c allocator.c allocator.h
	gcc -o memtest_dev memtest.c allocator.c

clean:
	rm -f memtest_sys memtest_dev