CC = gcc
CFLAGS = -W -Wall -O2 -g

.PHONY: all
all: libcurlspy.so

.PHONY: clean
clean:
	rm -f *.so

lib%.so: %.c
	$(CC) $(CFLAGS) -shared -fPIC -ldl $(<) -o $(@)

# vim:ts=4 sts=4 sw=4 noet
