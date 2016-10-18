#include <ncurses.h>
#include <form.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static FORM *form;
static FIELD *fields[5];

static void Help(WINDOW * w) {
	mvwprintw(w, 2, 37, "Customer Bill Program Help ");
	mvwprintw(w, 1, 37, "*************************** ");
	mvwprintw(w, 3, 37, "*************************** ");
	mvwprintw(w, 5, 1, "---------------------------------------------------------------------------------------------");
	mvwprintw(w, 12, 40, "USE THE KEYBOARD ONLY \n ");
	mvwprintw(w, 24, 1, "---------------------------------------------------------------------------------------------");
}


/*
 * This is useful because ncurses fill fields blanks with spaces.
 */
static char* trim_whitespaces(char *str){
	char *end;

	// trim leading space
	while(isspace(*str))
		str++;

	if(*str == 0) // all spaces?
		return str;

	// trim trailing space
	end = str + strnlen(str, 128) - 1;

	while(end > str && isspace(*end))
		end--;

	// write new null terminator
	*(end+1) = '\0';

	return str;
}

static void driver(int ch){
	int i;
	switch (ch) {
		case KEY_F(2):
			// Or the current field buffer won't be sync with what is displayed
			form_driver(form, REQ_NEXT_FIELD);
			form_driver(form, REQ_PREV_FIELD);
			move(LINES-3, 2);

			for (i = 0; fields[i]; i++) {
				printw("%s", trim_whitespaces(field_buffer(fields[i], 0)));

				if (field_opts(fields[i]) & O_ACTIVE)
					printw("\"\t");
				else
					printw(": \"");
			}

			refresh();
			pos_form_cursor(form);
			break;

		case KEY_DOWN:
			form_driver(form, REQ_NEXT_FIELD);
			form_driver(form, REQ_END_LINE);
			break;

		case KEY_UP:
			form_driver(form, REQ_PREV_FIELD);
			form_driver(form, REQ_END_LINE);
			break;

		case KEY_LEFT:
			form_driver(form, REQ_PREV_CHAR);
			break;

		case KEY_RIGHT:
			form_driver(form, REQ_NEXT_CHAR);
			break;

		// Delete the char before cursor
		case KEY_BACKSPACE:
		case 127:
			form_driver(form, REQ_DEL_PREV);
			break;

		// Delete the char under the cursor
		case KEY_DC:
			form_driver(form, REQ_DEL_CHAR);
			break;

		default:
			form_driver(form, ch);
			break;
	}

	
}

void AddP(WINDOW * w){
        mvwprintw(w, 3, 25, "Enter Product Name and Price ");
	
	fields[0] = new_field(1, 15, 0, 0, 0, 0);
	fields[1] = new_field(1, 30, 0, 20, 0, 0);
	fields[2] = new_field(1, 15, 2, 0, 0, 0);
	fields[3] = new_field(1, 30, 2, 20, 0, 0);
	fields[4] = NULL;
	assert(fields[0] != NULL && fields[1] != NULL && fields[2] != NULL && fields[3] != NULL);

	set_field_buffer(fields[0], 0, "Product Name : ");
	set_field_buffer(fields[1], 0, "val1");
	set_field_buffer(fields[2], 0, "Product Price : ");
	set_field_buffer(fields[3], 0, "val2");

	set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

	set_field_back(fields[1], A_UNDERLINE);
	set_field_back(fields[3], A_UNDERLINE);

	form = new_form(fields);
	assert(form != NULL);
	set_form_win(form, w);
	set_form_sub(form, derwin(w, 18, 60, 7, 7));
	post_form(form);

	refresh();
}

int main(){
	WINDOW *w,*w1,*w2,*w3;
	char list[5][20] = { "Help","ADD PRODUCT", "GENERATE A BILL", "DISPLAY A BILL", "DELETE A BILL", "Quit" };
	char item[20];
	char title[25];
	int ch, i = 0, width = 7;
 
	initscr(); // initialize Ncurses

	noecho();
	cbreak();
	keypad(stdscr, TRUE);


	w = newwin( 27, 23, 8, 10 ); // create a new window
	box( w, 0, 0 ); // sets default borders for the window
	w1 = newwin( 37, 135,1,1);
	box( w1, 0, 0 );
	w2 = newwin( 5, 120,2,10);
	box( w2, 0, 0 );  
	w3 = newwin( 27, 95,8,35);
	box( w3, 0, 0 ); 
	sprintf(title, "%-25s",  "CUSTOMER BILLING SYSTEM");
	mvwprintw( w2, 2, 50, "%s", title );
	sprintf(item, "%-20s",  "Select Options");
	mvwprintw( w1, 6, 10, "%s", item );
	AddP(w3);
       
	// now print all the menu items and highlight the first one
	for( i=0; i<5; i++ ) {
|		if( i == 0 )
			wattron( w, A_STANDOUT ); // highlights the first item.
		else
			wattroff( w, A_STANDOUT );
			sprintf(item, "%-20s",  list[i]);
			mvwprintw( w, i+1, 2, "%s", item );
	}
	wrefresh( w1 );
	wrefresh( w2 );
	wrefresh( w3 );
	wrefresh( w ); // update the terminal screen
  	i = 0;
	noecho(); // disable echoing of characters on the screen
	keypad( w, TRUE ); // enable keyboard input for the window.
	curs_set( 0 ); // hide the default screen cursor.
     	// get the input
	while(( ch = wgetch(w)) != 'q'){
	// right pad with spaces to make the items appear with even width.
	sprintf(item, "%-20s",  list[i]);
	mvwprintw( w, i+1, 2, "%s", item );
	// use a variable to increment or decrement the value based on the input.
		switch( ch ) {
			case KEY_UP:
					i--;
					i = ( i<0 ) ? 4 : i;
					break;
			case KEY_DOWN:
					i++;
					i = ( i>4 ) ? 0 : i;
					break;
		}
 // now highlight the next item in the list.
		wattron( w, A_STANDOUT );
		sprintf(item, "%-7s",  list[i]);
		mvwprintw( w, i+1, 2, "%s", item);
		wattroff( w, A_STANDOUT );
	}
 
	while ((ch = getch()) != KEY_F(1))
		driver(ch);
	wrefresh(w3);
	unpost_form(form);
	free_form(form);
	free_field(fields[0]);
	free_field(fields[1]);
	free_field(fields[2]);
	free_field(fields[3]);
	delwin( w );
	delwin( w1 );
	delwin( w2 );
	delwin( w3 );
	endwin();
}
