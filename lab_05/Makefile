c_p.out: src/cons_prod.c
	gcc -o c_p.out src/cons_prod.c

r_w.out: src/read_write.c
	gcc -o r_w.out src/read_write.c

1: c_p.out
2: r_w.out

clean:
	rm -f *.o *.exe *.out
