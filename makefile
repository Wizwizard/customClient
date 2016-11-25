customClient: customClient.o unp.o
	gcc -o customClient -ggdb -lpthread customClient.o unp.o

customClient.o: customClient.c
	gcc -c -ggdb -lpthread customClient.c

unp.o: unp.c
	gcc -c unp.c

clean:
	rm -rf *.o customClient
