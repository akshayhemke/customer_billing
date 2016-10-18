billing : billing.o
	cc billing.o -lncurses -lform -o billing
billing.o : billing.c
	gcc billing.c -c -lncurses -lform 
