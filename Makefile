myls: myls.c
	cc -g -Wall -Werror -Wextra  myls.c -o myls

clean:
	rm myls
