default: optimize

optimize: optimize.h optimize.c
		gcc optimize.c -o optimize

clean:
		rm -f optimize optimize.o
