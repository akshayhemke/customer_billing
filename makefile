project : project.o
	cc project.o -lncurses -lform -lmenu -o project
project.o : project.c
	gcc project.c -c -lncurses -lform -lmenu
