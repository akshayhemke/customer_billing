/*****************************************************************************
 * Copyright (C) Akshay Hemke akshayhemke7@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/



#include <ncurses.h>
#include <form.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define TAX 5.5
#define SIZE 100
struct additem {
	int item_id;
	char item_name[50];
	float price;
	unsigned int qt;
};
struct bill {
	int bill_id, quant[10], n;
	char item_name[10][50], date[15], cust_name[50];
	float total, price[10];
};
static int q=0;
struct BillPro {
	struct additem ad[SIZE];
	struct bill bi[SIZE];
	int ic, bc;
};
static struct BillPro bp;
static FORM *form;
static FIELD *fields[8];
static int op, res;
static int WriteD() {
	FILE *p;
	p = fopen("db.dat", "w");
	fwrite(&bp, sizeof(bp), 1, p);
	fclose(p);
	return 0;
}
static void ReadD() {
	FILE *p;
	p = fopen("db.dat", "r");
	fread(&bp, sizeof(bp), 1, p);
	fclose(p);
}
void SetColor(WINDOW *w, WINDOW *w1, WINDOW *w2, WINDOW *w3) {
	start_color();
	init_color(COLOR_BLUE, 0, 0, 800);
	init_color(COLOR_MAGENTA, 1000, 1000,900);
	init_color(COLOR_WHITE, 1000,1000,1000);
	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	init_pair(2, COLOR_BLACK, COLOR_MAGENTA);
	init_pair(3, COLOR_BLUE, COLOR_WHITE);
	init_pair(4, COLOR_WHITE, COLOR_CYAN);
	init_pair(5, COLOR_WHITE, COLOR_RED);
	init_pair(6, COLOR_RED, COLOR_RED);
	wbkgd(w, COLOR_PAIR(3));           	 
	wbkgd(w1, COLOR_PAIR(4));
	wbkgd(w2, COLOR_PAIR(1));
	wbkgd(w3, COLOR_PAIR(2));
}

void Setting(WINDOW *w1) {
	int row,col;
	getmaxyx(w1 ,row ,col);
	mvwprintw(w1, row - 4, 3, "Press ESC Key To Goto Main Menu");
	mvwprintw(w1, row- 3, 3, "Go To Help For More Help");
	mvwprintw(w1, row- 2, 3, "Copyright(C)Akshay Hemke ");
}
void Print(int sid) {
	WINDOW *w, *w1, *w2, *w3;
	int j, bc = 0, ch, i = 0, width = 7, row, col;
        float sumT = 0, x;
	char it[15][10], item[20], title[25], list[6][20] = { "Help","ADD PRODUCT", "GENERATE BILL", "SEARCH BILL", "SEARCH ITEM", "Quit" };
	initscr(); // initialize Ncurses
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr,row,col);
	w = newwin( row - 20, 23, 10, 10 ); // create a new window
	box( w, 0, 0 ); // sets default borders for the window
	w1 = newwin( row -5, col - 5, 1, 1);
	box( w1, 0, 0 );
	w2 = newwin( 5, col - 20, 3, 10);
	box( w2, 0, 0 );  
	w3 = newwin( row -20, col - 45, 10, 35);
	box( w3, 0, 0);
	SetColor(w, w1, w2, w3);
 	getmaxyx(w2, row, col);		/* get the number of rows and columns */	
	sprintf(title, "%-25s",  "CUSTOMER BILLING SYSTEM");
	mvwprintw( w2, 2, (col-strlen(title))/2, "%s", title );/* print the message at the center of the screen */
        sprintf(item, "%-20s",  "Select Options");
	mvwprintw(w, 0, 0, "%s", item);
	Setting(w1);
	for(j = 0; j <= bp.bc; j++) {
		if(bp.bi[j].bill_id == sid) {
			bc = j;
			break;
		}
	}
	if(bc != 0) {
		getmaxyx(w3,row,col);
		mvwprintw(w3, 2, 42, "Customer Bill ");
		mvwprintw(w3, 1, 40, "***************** ");
		mvwprintw(w3, 3, 40, "***************** ");
		mvwprintw(w3, 5, 1, "---------------------------------------------------------------------------------------------");
		mvwprintw(w3, 7, 1, "---------------------------------------------------------------------------------------------");
		mvwprintw(w3, 6, 5, "Customer Name : %s ",bp.bi[bc].cust_name);
		mvwprintw(w3, 6, 45, "Bill Id : %d ",bp.bi[bc].bill_id);
		mvwprintw(w3, 6, 70, "Bill Date : %s ",bp.bi[bc].date);
		mvwprintw(w3, 8, 1, "Sr.");
		mvwprintw(w3, 8, 10, "Item Name");
		mvwprintw(w3, 8, 40, "Price");
		mvwprintw(w3, 8, 55, "Quantity");
        	mvwprintw(w3, 8, 67, "Tax");
		mvwprintw(w3, 8, 80, "Total");
		
		for(i=0;i<bp.bi[bp.bc].n;i++) {
			mvwprintw(w3, 8 + (i + 1), 1, "%d", (i + 1));
			mvwprintw(w3, 8 + (i + 1), 10, "%s", bp.bi[bc].item_name[i]);
			mvwprintw(w3, 8 + (i + 1), 40, "%f", bp.bi[bc].price[i]);
			mvwprintw(w3, 8 + (i + 1), 55, "%d", bp.bi[bc].quant[i]);
			x = (bp.bi[bc].price[i] * bp.bi[bc].quant[i]);
			bp.bi[bc].total = (x + (x * TAX)/100 );
			sumT += bp.bi[bc].total;
        		mvwprintw(w3, 8 + (i + 1), 67, "%f", TAX);
			mvwprintw(w3, 8 + (i + 1), 80, "%f", bp.bi[bc].total);
		}
		mvwprintw(w3, 23, 1, "---------------------------------------------------------------------------------------------");
		mvwprintw(w3, 24, 60, "Total Bill  : Rs. %f ",sumT);
		mvwprintw(w3, 25, 1, "---------------------------------------------------------------------------------------------");
		}
       
// now print all the menu items and highlight the first one
		for(i = 0; i < 6; i++) {
			if( i == op )
				wattron( w, A_STANDOUT ); // highlights the first item.
        		else
				wattroff( w, A_STANDOUT );
				sprintf(item, "%-7s",  list[i]);
				mvwprintw( w, i+1, 2, "%s", item );
	    	} 
		wrefresh(w1);
		wrefresh(w2);
		wrefresh(w3);
		wrefresh(w); // update the terminal screen
		i = 0;
		noecho(); // disable echoing of characters on the screen
		keypad(w, TRUE); // enable keyboard input for the window.
	// get the input
		while((ch = wgetch(w)) == KEY_UP || ch== KEY_DOWN) {
// right pad with spaces to make the items appear with even width.
			sprintf(item, "%-20s",  list[i]);
			mvwprintw(w, i+1, 2, "%s", item);
// use a variable to increment or decrement the value based on the input.
			switch(ch) {
				case KEY_UP:
					i--;
					i = ( i < 0 ) ? 5 : i;
					break;
				case KEY_DOWN:
					i++;
					i = ( i > 5 ) ? 0 : i;
					break;
			}
			op = i;
        	    // now highlight the next item in the list.
			wattron(w, A_STANDOUT);
			sprintf(item, "%-7s", list[i]);
			mvwprintw(w, i + 1, 2, "%s", item);
			wattroff(w, A_STANDOUT);
		}
		wrefresh(w3);
		delwin(w);
		delwin(w1);
		delwin(w2);
		delwin(w3);
		endwin();
}

// This is useful because ncurses fill fields blanks with spaces.//
static char* trim_whitespaces(char *str) {
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
static int driver1(int ch,WINDOW * w) {
	int i, x, y;
	char proname[25], username[10], password[10];
	res = 0;
	switch(ch) {
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

		case 27:
			break;
		case 10 :
			getyx(w, y, x);	
			if(current_field(form) == fields[6] ) {
				bp.ic++;
				bp.ad[bp.ic].item_id = bp.ad[bp.ic-1].item_id + 1;
				sscanf(trim_whitespaces(field_buffer(fields[1], 0)),"%s", proname);				
				for(i=0; i <= bp.ic; i++) {	
					if(strcmp(proname ,bp.ad[i].item_name) == 0) {
						res = 1	;				
						mvwprintw(w, y - 8, 8,  "Product Name Already Exits");			
					}
				}
				if(res == 0) {	
					strcpy(bp.ad[bp.ic].item_name, trim_whitespaces(field_buffer(fields[1], 0)));
					sscanf(trim_whitespaces(field_buffer(fields[3], 0)), "%f", &bp.ad[bp.ic].price);	
					sscanf(trim_whitespaces(field_buffer(fields[5], 0)),"%d",&bp.ad[bp.ic].qt);
					mvwprintw(w, 2, 25, "Product Added : ID = %d",bp.ad[bp.ic].item_id); 
					wmove(w, y, x);			
					wrefresh( w );
				}
			}	
			if(current_field(form) == fields[7] ) {
				return 27;
			}
			if(current_field(form) == fields[4] ) {
				sscanf(trim_whitespaces(field_buffer(fields[1], 0)), "%s", username);
				sscanf(trim_whitespaces(field_buffer(fields[3], 0)), "%s", password);
				if(strcmp(username, "akshay") == 0){
					if(strcmp(password, "akshay") == 0){
						return 2;
					} else
						mvwprintw(w, 4, y/2, "Incorrect password!! Try Again");
				} else
					mvwprintw(w, 4, y/2, "Incorrect Username Or Password!! Try Again");
					
			}
			break;
		default:
			form_driver(form, ch);
			break;
	}
}
int ns=0;
int found = 0 ,sid;
static void driver2(int ch, WINDOW *w) {
	int i, a = 3, q, id, row, col, l = 0;
	char qn[25], proname[25];
	float x, y;
	time_t now;
	struct tm *d;
	res = 0;
	switch(ch) {
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
		case 27 :
			break;
		case 10:
			if(current_field(form) == fields[6]){
				id=0;
				sscanf(trim_whitespaces(field_buffer(fields[1], 0)),"%s",proname);
				getmaxyx( w , row ,col);		
				for(i=0; i <= bp.ic; i++){	
					if(strcmp(proname ,bp.ad[i].item_name) == 0){
					l = 1;
					strcpy(bp.bi[bp.bc].item_name[ns],bp.ad[i].item_name);
					sscanf(trim_whitespaces(field_buffer(fields[3], 0)),"%d",&q);
						if(bp.ad[i].qt >= q){
							bp.bi[bp.bc].quant[ns] = q;
							bp.bi[bp.bc].price[ns] = bp.ad[i].price;
							bp.ad[i].qt = bp.ad[i].qt - q;
							ns++;
						} else {
							if(bp.ad[i].qt == 0)
								mvwprintw(w, row -12, 8,  "Product Out Of Stock", bp.ad[i].qt);			
							else
							mvwprintw(w, row - 12, 8,  "only %d product left", bp.ad[i].qt);		
						}
	
				bp.bi[bp.bc].n = ns;	
					}
				}
				if(l != 1)
					mvwprintw(w, row - 13,8,  "Product doesnt exits",bp.ad[i].qt);
	
                
				mvwprintw(w, a,10,  "Item Name");
				mvwprintw(w, a,25,  "Quantity");
				mvwprintw(w, a,40,  "Price");
				for(i=0; i < ns; i++) {
					mvwprintw(w, a + (i + 1),10,  "%s",bp.bi[bp.bc].item_name[i]);
					mvwprintw(w, a + (i + 1),25,  "%d",bp.bi[bp.bc].quant[i]);
					mvwprintw(w, a + (i + 1),40,  "%f",bp.bi[bp.bc].price[i]);
				}
		 
				wrefresh( w );
			}
			res=11;
			if(current_field(form) == fields[7] ){
				bp.bi[bp.bc].bill_id = bp.bc + 100;
				strcpy(bp.bi[bp.bc].cust_name,trim_whitespaces(field_buffer(fields[5], 0)));
				time(&now);
				d = localtime(&now);
				strftime(bp.bi[bp.bc].date, 15, "%d/%m/%Y", d);
				getyx(w, y, x);
				mvwprintw(w, 2, 25, "Bill Saved with bill id=%d ", bp.bi[bp.bc].bill_id);
				wmove(w, y, x);
				res=0;	
				if(current_field(form) == fields[7] ){
					int sid = bp.bi[bp.bc].bill_id;
					Print(sid);
				}	
				wrefresh(w);
			}		
		break;
		default:
			form_driver(form, ch);
			break;
	}

	
}
//---------------DRIVER3------------------------//
static void driver3(int ch, WINDOW *w){
	int i, row ,col, l = 0;
		char title[]="Found !!! Press ESC to See The Bill ";
		char title1[]="Not Found !!! Bill Not Exits ";
		char proname[25];
	switch(ch) {
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
			
		case 27:
			break;
		case 10 :
			getmaxyx(w, row, col);
				if(current_field(form) == fields[3] ){
					sscanf(trim_whitespaces(field_buffer(fields[1], 0)),"%s", proname);
					for(i=0; i <= bp.ic; i++){	
						if(strcmp(proname, bp.ad[i].item_name) == 0){
							mvwprintw(w, row - 7, 10, "%s", "Product Found!!!" );		
							mvwprintw(w, row - 6, 10, "%s%s", "Name :", proname );
							mvwprintw(w, row - 5, 10, "%s%f", "Price :", bp.ad[i].price );
							if(bp.ad[i].qt != 0)					
								mvwprintw(w, row - 4, 10, "%s%d", "Stock Present :", bp.ad[i].qt);
							else
								mvwprintw(w, row - 4, 10, "%s", "PRODUCT OUT OF STOCK !!!!!!!!");
							mvwprintw(w, row - 3, 10, "%s%d", "Product ID :",bp.ad[i].item_id);
							l = 1;
						}
					}
					if(l == 0)
						mvwprintw(w, row-5, 10, "%s", "NOT FOUND !! Product Not Exists" );
				}	
				if(current_field(form) == fields[2]) {
					sscanf(trim_whitespaces(field_buffer(fields[1], 0)),"%d", &sid);
					for(i=0; i <= bp.bc; i++) {
						if(bp.bi[i].bill_id == sid) {
							l = found = 1;
							mvwprintw(w, 4, (col-strlen(title))/2, "%s", title );	
						}
					}
					if(l == 0)
						mvwprintw(w, 3, (col-strlen(title1))/2, "%s", title1);
				}
				break;
		default:
			form_driver(form, ch);
			break;
	}

	
}



//Item Search//
static void itemsearch(WINDOW *w) {
	int i, j = 0, a = 3, row ,col;
	getmaxyx(w, row, col);
	
	mvwprintw(w, 1, 25, "Product Information");
		mvwprintw(w, a, 10, "Item Id");
		mvwprintw(w, a, 20, "Item Name");
                mvwprintw(w, a, 40, "Item Price");
		mvwprintw(w, a, 55, "Quantity");
	for(i = 1; i <= 15; i++){
		mvwprintw(w, a + (i + 1), 10,  "%d", bp.ad[i].item_id);
		mvwprintw(w, a + (i + 1), 20,  "%s", bp.ad[i].item_name);
                mvwprintw(w, a + (i + 1), 40,  "%f", bp.ad[i].price);
		mvwprintw(w, a + (i + 1), 55,  "%d", bp.ad[i].qt);
        }
	
	fields[0] = new_field(1, 15, 0, 0, 0, 0);
	fields[1] = new_field(1, 30, 0, 20, 0, 0);
	fields[2] = new_field(1, 15, 2, 0, 0, 0);
	fields[3] = new_field(1, 10, 2, 25, 0, 0);
	fields[4]= NULL;
	//assert(fields[0] != NULL && fields[1] != NULL && fields[2] != NULL && fields[3] != NULL);

	set_field_buffer(fields[0], 0, "Product Name : ");
	set_field_buffer(fields[1], 0, "");
	set_field_buffer(fields[2], 0, "");
	set_field_buffer(fields[3], 0, "  Search ");

	set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[3], O_VISIBLE | O_PUBLIC | O_ACTIVE);

	set_field_back(fields[0], COLOR_PAIR(2));
	set_field_back(fields[1], COLOR_PAIR(1));
	set_field_back(fields[2], COLOR_PAIR(2));
	set_field_back(fields[3], COLOR_PAIR(5));

	form = new_form(fields);
	assert(form != NULL);
	set_form_win(form, w);
	set_form_sub(form, derwin(w, 8, 60, row-10, 7));
	post_form(form);

	refresh();
}

//Add Item Function//
static void AddP1(WINDOW *w) {
	int i, j = 0, a = 3, row, col;
	getmaxyx(w, row, col);
	mvwprintw(w, 1, 25, "Enter Product Id to Generate Bill");
		mvwprintw(w, a, 10,  "Item Id");
		mvwprintw(w, a, 20,  "Item Name");
		mvwprintw(w, a, 40,  "Item Price");
		mvwprintw(w, a, 55,  "Quantity");
	for(i = 1; i <= 15; i++) {
		mvwprintw(w, a + (i + 1), 10,  "%d", bp.ad[i].item_id);
		mvwprintw(w, a + (i + 1), 20,  "%s", bp.ad[i].item_name);
                mvwprintw(w, a + (i + 1), 40,  "%f", bp.ad[i].price);
		mvwprintw(w, a + (i + 1), 55,  "%d", bp.ad[i].qt);
        }
	
	fields[0] = new_field(1, 15, 0, 0, 0, 0);
	fields[1] = new_field(1, 30, 0, 20, 0, 0);
	fields[2] = new_field(1, 15, 2, 0, 0, 0);
	fields[3] = new_field(1, 30, 2, 20, 0, 0);
	fields[4] = new_field(1, 15, 4, 0, 0, 0);
	fields[5] = new_field(1, 30, 4, 20, 0, 0);
        fields[6] = new_field(1, 10, 6, 20, 0, 0);
	fields[7] = new_field(1, 10, 6, 35, 0, 0);
	fields[8] = NULL;
	assert(fields[0] != NULL && fields[1] != NULL && fields[2] != NULL && fields[3] != NULL);

	set_field_buffer(fields[0], 0, "Product Name : ");
	set_field_buffer(fields[1], 0, "");
	set_field_buffer(fields[2], 0, "Product Price : ");
	set_field_buffer(fields[3], 0, "");
	set_field_buffer(fields[4], 0, "Quantity : ");
	set_field_buffer(fields[5], 0, "");
	set_field_buffer(fields[6], 0, " Add Item");
	set_field_buffer(fields[7], 0, " Main Menu");
		
	set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[4], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[5], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[6], O_VISIBLE | O_PUBLIC | O_ACTIVE);
	set_field_back(fields[0], COLOR_PAIR(2));
	set_field_back(fields[1], COLOR_PAIR(1));
	set_field_back(fields[2], COLOR_PAIR(2));
	set_field_back(fields[3], COLOR_PAIR(1));
	set_field_back(fields[4], COLOR_PAIR(2));
	set_field_back(fields[5], COLOR_PAIR(1));
	set_field_back(fields[6], COLOR_PAIR(5));
	set_field_back(fields[7], COLOR_PAIR(5));

	form = new_form(fields);
	assert(form != NULL);
	set_form_win(form, w);
	set_form_sub(form, derwin(w, 8, 60, row-10, 7));
	post_form(form);

	refresh();
}

//Add Item To Bill Function//
static void AddItemToBill(WINDOW *w){
	int i, a = 3, n = 0, row, col;
	FIELD *field[9];
	mvwprintw(w, 1, 25, "Enter Product Id to Generate Bill %d",bp.ic);
	getmaxyx(w, row, col);	
	
	fields[0] = new_field(1, 20, 0, 0, 0, 0);
	fields[1] = new_field(1, 30, 0, 25, 0, 0);
	fields[2] = new_field(1, 20, 2, 0, 0, 0);
	fields[3] = new_field(1, 30, 2, 25, 0, 0);
	fields[4] = new_field(1, 20, 4, 0, 0, 0);
        fields[5] = new_field(1, 30, 4, 25, 0, 0);
	fields[6] = new_field(1, 10, 6, 25, 0, 0);
	fields[7] = new_field(1, 20, 6, 36, 0, 0);
	fields[8] = NULL;

	assert(fields[0] != NULL && fields[1] != NULL && fields[2] != NULL && fields[3] != NULL);

	set_field_buffer(fields[0], 0, "Product Name : ");
	set_field_buffer(fields[1], 0, "");
	set_field_buffer(fields[2], 0, "Product Quantity : ");
	set_field_buffer(fields[3], 0, "");
	set_field_buffer(fields[4], 0, "Customer Name : ");
	set_field_buffer(fields[5], 0, "");
	set_field_buffer(fields[6], 0, " Add Item");
	set_field_buffer(fields[7], 0, "   Generate Bill");
	

	set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[4], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[5], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[6], O_VISIBLE | O_PUBLIC | O_ACTIVE);
	set_field_opts(fields[7], O_VISIBLE | O_PUBLIC | O_ACTIVE);
	

	set_field_back(fields[0], COLOR_PAIR(2));
	set_field_back(fields[1], COLOR_PAIR(1));
	set_field_back(fields[2], COLOR_PAIR(2));
	set_field_back(fields[3], COLOR_PAIR(1));
	set_field_back(fields[4], COLOR_PAIR(2));
	set_field_back(fields[5], COLOR_PAIR(1));
	set_field_back(fields[6], COLOR_PAIR(5));
	set_field_back(fields[7], COLOR_PAIR(5));

	form = new_form(fields);
	assert(form != NULL);
	set_form_win(form, w);
	set_form_sub(form, derwin(w, 8, 60, row - 10, 7));
	post_form(form);

	refresh();
}
static void logfun(WINDOW *w) {
	int i, a = 3, n = 0, row, col;
	FIELD *field[6];
        
	getmaxyx(w, row, col);
	char msg[] = "Welcome !!! Please Login To Continue ";	
	mvwprintw(w, 2, (col-strlen(msg))/2, "%s", msg);
	fields[0] = new_field(1, 20, 0, 0, 0, 0);
	fields[1] = new_field(1, 30, 0, 10, 0, 0);
	fields[2] = new_field(1, 20, 2, 0, 0, 0);
	fields[3] = new_field(1, 30, 2, 10, 0, 0);
	fields[4] = new_field(1, 10, 4, 20, 0, 0);
	fields[5] = NULL;

	assert(fields[0] != NULL && fields[1] != NULL && fields[2] != NULL && fields[3] != NULL);

	set_field_buffer(fields[0], 0, "UserName  ");
	set_field_buffer(fields[1], 0, "");
	set_field_buffer(fields[2], 0, "Password  ");
	set_field_buffer(fields[3], 0, "");
	set_field_buffer(fields[4], 0, "  Login ");
	
	

	set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[4], O_VISIBLE | O_PUBLIC | O_ACTIVE);
	

	set_field_back(fields[0], COLOR_PAIR(2));
	set_field_back(fields[1], COLOR_PAIR(1));
	set_field_back(fields[2], COLOR_PAIR(2));
	set_field_back(fields[3], COLOR_PAIR(1));
	set_field_back(fields[4], COLOR_PAIR(5));
	

	form = new_form(fields);
	assert(form != NULL);
	set_form_win(form, w);
	set_form_sub(form, derwin(w, 8, 60, row/4, col/4 ));
	post_form(form);

	refresh();
}


static void Help(WINDOW *w) {
	int row, col;
	char title[] = " Welcome ";
	char title1[] = " After Pressing Enter On Options Press Left Arrow Key To Enter The Window";
	char title2[] = " Use Up Arrow And Down Arrow Keys To Move Through Fields ";
	char title3[] = " After Adding Product Press Any Arrow Key And Then ESC To Move To Main Menu ";
	char title4[] = " Only Some Items Are Listed In Serch Item Window. To search The Product Use Search Iteam ";
	getmaxyx(w, row, col);	
	attron(A_BOLD);
	
	mvwprintw( w, 2, (col-strlen(title))/2, "%s", title );
	mvwprintw( w, 5, (col-strlen(title1))/2, "%s", title1 );
	mvwprintw( w, 7, (col-strlen(title2))/2, "%s", title2 );
	mvwprintw( w, 9, (col-strlen(title3))/2, "%s", title3 );
	mvwprintw( w, 11, (col-strlen(title4))/2, "%s", title4 );

	attroff(A_BOLD);
			
}
static void BillGen(WINDOW * w)
{
	int i, a, b, c, d, e, f, bc=0, dc;
        float sumT = 0, x;
	a = 1, b = 10, c = 40, d = 55, e = 67, f = 80;
	char it[15][10];
	for(i = 0; i <= bp.bc; i++){
		if(bp.bi[i].bill_id == sid){
			bc = i;
			break;
		}
	}
        if(bc != 0) {
	mvwprintw(w, 2, 42, "Customer Bill ");
	mvwprintw(w, 1, 40, "***************** ");
	mvwprintw(w, 3, 40, "***************** ");
	mvwprintw(w, 5, 1, "---------------------------------------------------------------------------------------------");
	mvwprintw(w, 7, 1, "---------------------------------------------------------------------------------------------");
	mvwprintw(w, 6, 5, "Customer Name : %s ", bp.bi[bc].cust_name);
	mvwprintw(w, 6, 45, "Bill Id : %d ", bp.bi[bc].bill_id);
	mvwprintw(w, 6, 70, "Bill Date : %s ", bp.bi[bc].date);
	mvwprintw(w, 8, a, "Sr.");
	mvwprintw(w, 8, b, "Item Name");
	mvwprintw(w, 8, c, "Price");
	mvwprintw(w, 8, d, "Quantity");
        mvwprintw(w, 8, e, "Tax");
	mvwprintw(w, 8, f, "Total");
	dc = sid - 100;
	for(i=0; i < bp.bi[dc].n; i++){
		mvwprintw(w, 8 + (i + 1), 1, "%d", (i+1));
			mvwprintw(w, 8 + (i + 1), 10, "%s", bp.bi[bc].item_name[i]);
			mvwprintw(w, 8 + (i + 1), 40, "%f", bp.bi[bc].price[i]);
			mvwprintw(w, 8 + (i + 1), 55, "%d", bp.bi[bc].quant[i]);
			x = (bp.bi[bc].price[i] * bp.bi[bc].quant[i]);
			bp.bi[bc].total = (x + (x * TAX)/100 );
			sumT += bp.bi[bc].total;
        		mvwprintw(w, 8 + (i + 1), 67, "%f", TAX);
			mvwprintw(w, 8 + (i + 1), 80, "%f", bp.bi[bc].total);
	}
	mvwprintw(w, 23, 1, "---------------------------------------------------------------------------------------------");
	mvwprintw(w, 24, 60, "Total Bill  : Rs. %f ",sumT);
	mvwprintw(w, 25, 1, "---------------------------------------------------------------------------------------------");
	}
}

// search bill//
static void AddP3(WINDOW *w) {	
	int row, col;
	char msg[] = "Enter The Bill Id To Be Searched";	
	getmaxyx(w ,row ,col);        
	
	mvwprintw(w, 2, (col-strlen(msg))/2, "%s", msg);
	fields[0] = new_field(1, 15, 0, 0, 0, 0);
	fields[1] = new_field(1, 30, 0, 20, 0, 0);
	fields[2] = new_field(1, 15, 2, 20, 0, 0);
	fields[3] = NULL;
	assert(fields[0] != NULL && fields[1] != NULL && fields[2] != NULL);

	set_field_buffer(fields[0], 0, "Bill Id : ");
	set_field_buffer(fields[1], 0, "");
	set_field_buffer(fields[2], 0, "  Search Bill");
	
	set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[2], O_VISIBLE | O_PUBLIC | O_ACTIVE );

	set_field_back(fields[1], A_UNDERLINE);
	set_field_back(fields[2], A_UNDERLINE);
	
	set_field_back(fields[0], COLOR_PAIR(2));
	set_field_back(fields[1], COLOR_PAIR(1));
	set_field_back(fields[2], COLOR_PAIR(5));
	form = new_form(fields);
	assert(form != NULL);
	set_form_win(form, w);
	set_form_sub(form, derwin(w, 18, 60, 7, (col-60)/2));
	post_form(form);

	refresh();
}

int ch2;

void f1() {
	WINDOW *w,*w1,*w2,*w3;
	char item[20], title[25], list[6][20] = { "Help","ADD PRODUCT", "GENERATE BILL", "SEARCH BILL", "SEARCH ITEM", "Quit" };
	int ch, i = 0, width = 7, row ,col;
	initscr(); // initialize Ncurses
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, row, col);
	w = newwin(row - 20, 23, 10, 10); // create a new window
	box(w, 0, 0); // sets default borders for the window
	w1 = newwin(row -5, col - 5, 1, 1);
	box(w1, 0, 0);
	w2 = newwin(5, col - 20, 3, 10);
	box(w2, 0, 0);  
	w3 = newwin(row -20, col - 45, 10, 35);
	box(w3, 0, 0);
	SetColor(w, w1, w2, w3);
 	getmaxyx(w2, row, col);	/* get the number of rows and columns */
	sprintf(title, "%-25s",  "CUSTOMER BILLING SYSTEM");
	mvwprintw( w2, 2, (col-strlen(title))/2, "%s", title);                    	/* print the message at the center of the screen */
	sprintf(item, "%-20s",  "Select Options");
	mvwprintw(w, 0, 0, "%s", item);
	Setting(w1);
	Help(w3);
       
// now print all the menu items and highlight the first one
	for(i = 0; i < 6; i++) {
	        if(i == op)
	            wattron(w, A_STANDOUT); // highlights the first item.
	        else
	            wattroff(w, A_STANDOUT);
	        sprintf(item, "%-7s",  list[i]);
	        mvwprintw(w, i + 1, 2, "%s", item);
	}
	wrefresh(w1);
	wrefresh(w2);
	wrefresh(w3);
	wrefresh(w); // update the terminal screen
  	i = 0;
	noecho(); // disable echoing of characters on the screen
	keypad(w, TRUE); // enable keyboard input for the window.
   
       // get the input
	while((ch = wgetch(w)) == KEY_UP || ch== KEY_DOWN) {
         // right pad with spaces to make the items appear with even width.
		sprintf(item, "%-20s", list[i]);
		mvwprintw(w, i + 1, 2, "%s", item);
         // use a variable to increment or decrement the value based on the input.
		switch(ch) {
			case KEY_UP:
				i--;
				i = ( i < 0 ) ? 5 : i;
				break;
			case KEY_DOWN:
				i++;
				i = ( i > 5 ) ? 0 : i;
				break;
		}
		op = i;
            // now highlight the next item in the list.
		wattron( w, A_STANDOUT );
		sprintf(item, "%-7s",  list[i]);
		mvwprintw( w, i+1, 2, "%s", item);
		wattroff( w, A_STANDOUT );
	}
	wrefresh(w3);
	delwin(w);
	delwin(w1);
	delwin(w2);
	delwin(w3);
	endwin();
}
//add items
void f2(){
	WINDOW *w,*w1,*w2,*w3;
	char item[20], title[25], list[6][20] = { "Help","ADD PRODUCT", "GENERATE BILL", "SEARCH BILL", "SEARCH ITEM", "Quit" };
	int ch, i = 0, width = 7, row ,col;
	initscr(); // initialize Ncurses
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, row, col);
	w = newwin(row - 20, 23, 10, 10); // create a new window
	box(w, 0, 0); // sets default borders for the window
	w1 = newwin(row -5, col - 5, 1, 1);
	box(w1, 0, 0);
	w2 = newwin(5, col - 20, 3, 10);
	box(w2, 0, 0);  
	w3 = newwin(row -20, col - 45, 10, 35);
	box(w3, 0, 0);
	SetColor(w, w1, w2, w3);
 	getmaxyx(w2, row, col);	/* get the number of rows and columns */
	sprintf(title, "%-25s",  "CUSTOMER BILLING SYSTEM");
	mvwprintw( w2, 2, (col-strlen(title))/2, "%s", title);                    	/* print the message at the center of the screen */
	sprintf(item, "%-20s",  "Select Options");
	mvwprintw(w, 0, 0, "%s", item);
	Setting(w1);
	AddP1(w3);
       
// now print all the menu items and highlight the first one
	for(i = 0; i < 6; i++) {
	        if(i == op)
	            wattron(w, A_STANDOUT); // highlights the first item.
	        else
	            wattroff(w, A_STANDOUT);
	        sprintf(item, "%-7s",  list[i]);
	        mvwprintw(w, i + 1, 2, "%s", item);
	}
	wrefresh(w1);
	wrefresh(w2);
	wrefresh(w3);
	wrefresh(w); // update the terminal screen
	i = 0;
	noecho(); // disable echoing of characters on the screen
	keypad(w, TRUE); // enable keyboard input for the window.
   
       // get the input
	while((ch = wgetch(w)) == KEY_UP || ch== KEY_DOWN) {
         // right pad with spaces to make the items appear with even width.
		sprintf(item, "%-20s", list[i]);
		mvwprintw(w, i + 1, 2, "%s", item);
         // use a variable to increment or decrement the value based on the input.
		switch(ch) {
			case KEY_UP:
				i--;
				i = ( i < 0 ) ? 5 : i;
				break;
			case KEY_DOWN:
				i++;
				i = ( i > 5 ) ? 0 : i;
				break;
		}
		op = i;
            // now highlight the next item in the list.
		wattron( w, A_STANDOUT );
		sprintf(item, "%-7s",  list[i]);
		mvwprintw( w, i+1, 2, "%s", item);
		wattroff( w, A_STANDOUT );
	}
	wmove(w3,0,20);
	while((ch2 = wgetch(w)) != 27) {		
		int hp;
		hp = driver1(ch2, w3);
		wrefresh(w3);
	 	op=0;
		if(hp == 27)
			break;
                if(ch2 == 27)
			break;
	}
	mvwprintw(w3, 30, 25, "Record Saved");	
	wrefresh(w3);
	unpost_form(form);
	free_form(form);
	free_field(fields[0]);
	free_field(fields[1]);
	free_field(fields[2]);
	free_field(fields[3]);
	delwin(w);
	delwin(w1);
	delwin(w2);
	delwin(w3);
	endwin();
}

//new bill
void f3(){
	WINDOW *w,*w1,*w2,*w3;
	char item[20], title[25], list[6][20] = { "Help","ADD PRODUCT", "GENERATE BILL", "SEARCH BILL", "SEARCH ITEM", "Quit" };
	int ch, i = 0, width = 7, row ,col;
	initscr(); // initialize Ncurses
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, row, col);
	w = newwin(row - 20, 23, 10, 10); // create a new window
	box(w, 0, 0); // sets default borders for the window
	w1 = newwin(row -5, col - 5, 1, 1);
	box(w1, 0, 0);
	w2 = newwin(5, col - 20, 3, 10);
	box(w2, 0, 0);  
	w3 = newwin(row -20, col - 45, 10, 35);
	box(w3, 0, 0);
	SetColor(w, w1, w2, w3);
 	getmaxyx(w2, row, col);	/* get the number of rows and columns */
	sprintf(title, "%-25s",  "CUSTOMER BILLING SYSTEM");
	mvwprintw( w2, 2, (col-strlen(title))/2, "%s", title);                    	/* print the message at the center of the screen */
	sprintf(item, "%-20s",  "Select Options");
	mvwprintw(w, 0, 0, "%s", item);
	Setting(w1);	
	AddItemToBill(w3);
       
// now print all the menu items and highlight the first one
	for(i = 0; i < 6; i++) {
	        if(i == op)
	            wattron(w, A_STANDOUT); // highlights the first item.
	        else
	            wattroff(w, A_STANDOUT);
	        sprintf(item, "%-7s",  list[i]);
	        mvwprintw(w, i + 1, 2, "%s", item);
	}
	wrefresh(w1);
	wrefresh(w2);
	wrefresh(w3);
	wrefresh(w); // update the terminal screen
	i = 0;
	noecho(); // disable echoing of characters on the screen
	keypad(w, TRUE); // enable keyboard input for the window.
   
       // get the input
	while((ch = wgetch(w)) == KEY_UP || ch== KEY_DOWN) {
         // right pad with spaces to make the items appear with even width.
		sprintf(item, "%-20s", list[i]);
		mvwprintw(w, i + 1, 2, "%s", item);
         // use a variable to increment or decrement the value based on the input.
		switch(ch) {
			case KEY_UP:
				i--;
				i = ( i < 0 ) ? 5 : i;
				break;
			case KEY_DOWN:
				i++;
				i = ( i > 5 ) ? 0 : i;
				break;
		}
		op = i;
            // now highlight the next item in the list.
		wattron( w, A_STANDOUT );
		sprintf(item, "%-7s",  list[i]);
		mvwprintw( w, i+1, 2, "%s", item);
		wattroff( w, A_STANDOUT );
	}
	bp.bc++;
	ns = 0;
	while((ch2 = wgetch(w)) != 27) {
		driver2(ch2, w3);
		wrefresh(w3);
		op = 0;
		if(ch2 == 27)
		break;
	}
	wrefresh(w3);
	unpost_form(form);
	free_form(form);
	free_field(fields[0]);
	free_field(fields[1]);
	free_field(fields[2]);
	free_field(fields[3]);
	delwin(w);
	delwin(w1);
	delwin(w2);
	delwin(w3);
	endwin();
}
// search bill
void f4(){
	WINDOW *w,*w1,*w2,*w3;
	char item[20], title[25], list[6][20] = { "Help","ADD PRODUCT", "GENERATE BILL", "SEARCH BILL", "SEARCH ITEM", "Quit" };
	int ch, i = 0, width = 7, row ,col;
	initscr(); // initialize Ncurses
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, row, col);
	w = newwin(row - 20, 23, 10, 10); // create a new window
	box(w, 0, 0); // sets default borders for the window
	w1 = newwin(row -5, col - 5, 1, 1);
	box(w1, 0, 0);
	w2 = newwin(5, col - 20, 3, 10);
	box(w2, 0, 0);  
	w3 = newwin(row -20, col - 45, 10, 35);
	box(w3, 0, 0);
	SetColor(w, w1, w2, w3);
 	getmaxyx(w2, row, col);	/* get the number of rows and columns */
	sprintf(title, "%-25s",  "CUSTOMER BILLING SYSTEM");
	mvwprintw( w2, 2, (col-strlen(title))/2, "%s", title);                    	/* print the message at the center of the screen */
	sprintf(item, "%-20s",  "Select Options");
	mvwprintw(w, 0, 0, "%s", item);
	Setting(w1);
	AddP3(w3);
       
// now print all the menu items and highlight the first one
	for(i = 0; i < 6; i++) {
	        if(i == op)
	            wattron(w, A_STANDOUT); // highlights the first item.
	        else
	            wattroff(w, A_STANDOUT);
	        sprintf(item, "%-7s",  list[i]);
	        mvwprintw(w, i + 1, 2, "%s", item);
	}
	wrefresh(w1);
	wrefresh(w2);
	wrefresh(w3);
	wrefresh(w); // update the terminal screen
	i = 0;
	noecho(); // disable echoing of characters on the screen
	keypad(w, TRUE); // enable keyboard input for the window.
   
       // get the input
	while((ch = wgetch(w)) == KEY_UP || ch== KEY_DOWN) {
         // right pad with spaces to make the items appear with even width.
		sprintf(item, "%-20s", list[i]);
		mvwprintw(w, i + 1, 2, "%s", item);
         // use a variable to increment or decrement the value based on the input.
		switch(ch) {
			case KEY_UP:
				i--;
				i = ( i < 0 ) ? 5 : i;
				break;
			case KEY_DOWN:
				i++;
				i = ( i > 5 ) ? 0 : i;
				break;
		}
		op = i;
            // now highlight the next item in the list.
		wattron( w, A_STANDOUT );
		sprintf(item, "%-7s",  list[i]);
		mvwprintw( w, i+1, 2, "%s", item);
		wattroff( w, A_STANDOUT );
	}
	while((ch2 = wgetch(w)) != 27) {
		driver3(ch2, w3);
		wrefresh(w3);
		op=0;
                if(ch2 == 27)
		break;
	}
	wrefresh(w3);
	unpost_form(form);
	free_form(form);
	free_field(fields[0]);
	free_field(fields[1]);
	free_field(fields[2]);
	free_field(fields[3]);
	delwin(w);
	delwin(w1);
	delwin(w2);
	delwin(w3);
	endwin();
}
void f5(){
	WINDOW *w,*w1,*w2,*w3;
	char item[20], title[25], list[6][20] = { "Help","ADD PRODUCT", "GENERATE BILL", "SEARCH BILL", "SEARCH ITEM", "Quit" };
	int ch, i = 0, width = 7, row ,col;
	initscr(); // initialize Ncurses
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, row, col);
	w = newwin(row - 20, 23, 10, 10); // create a new window
	box(w, 0, 0); // sets default borders for the window
	w1 = newwin(row -5, col - 5, 1, 1);
	box(w1, 0, 0);
	w2 = newwin(5, col - 20, 3, 10);
	box(w2, 0, 0);  
	w3 = newwin(row -20, col - 45, 10, 35);
	box(w3, 0, 0);
	SetColor(w, w1, w2, w3);
 	getmaxyx(w2, row, col);	/* get the number of rows and columns */
	sprintf(title, "%-25s",  "CUSTOMER BILLING SYSTEM");
	mvwprintw( w2, 2, (col-strlen(title))/2, "%s", title);                    	/* print the message at the center of the screen */
	sprintf(item, "%-20s",  "Select Options");
	mvwprintw(w, 0, 0, "%s", item);
	Setting(w1);
	BillGen(w3);
       
// now print all the menu items and highlight the first one
	for(i = 0; i < 6; i++) {
	        if(i == op)
	            wattron(w, A_STANDOUT); // highlights the first item.
	        else
	            wattroff(w, A_STANDOUT);
	        sprintf(item, "%-7s",  list[i]);
	        mvwprintw(w, i + 1, 2, "%s", item);
	}
	wrefresh(w1);
	wrefresh(w2);
	wrefresh(w3);
	wrefresh(w); // update the terminal screen
	i = 0;
	noecho(); // disable echoing of characters on the screen
	keypad(w, TRUE); // enable keyboard input for the window.
   
       // get the input
	while((ch = wgetch(w)) == KEY_UP || ch== KEY_DOWN) {
         // right pad with spaces to make the items appear with even width.
		sprintf(item, "%-20s", list[i]);
		mvwprintw(w, i + 1, 2, "%s", item);
         // use a variable to increment or decrement the value based on the input.
		switch(ch) {
			case KEY_UP:
				i--;
				i = ( i < 0 ) ? 5 : i;
				break;
			case KEY_DOWN:
				i++;
				i = ( i > 5 ) ? 0 : i;
				break;
		}
		op = i;
            // now highlight the next item in the list.
		wattron( w, A_STANDOUT );
		sprintf(item, "%-7s",  list[i]);
		mvwprintw( w, i+1, 2, "%s", item);
		wattroff( w, A_STANDOUT );
	}
	wrefresh(w3);
	delwin(w);
	delwin(w1);
	delwin(w2);
	delwin(w3);
	endwin();
}
void f6(){
	WINDOW *w,*w1,*w2,*w3;
	char item[20], title[25], list[6][20] = { "Help","ADD PRODUCT", "GENERATE BILL", "SEARCH BILL", "SEARCH ITEM", "Quit" };
	int ch, i = 0, width = 7, row ,col;
	initscr(); // initialize Ncurses
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, row, col);
	w = newwin(row - 20, 23, 10, 10); // create a new window
	box(w, 0, 0); // sets default borders for the window
	w1 = newwin(row -5, col - 5, 1, 1);
	box(w1, 0, 0);
	w2 = newwin(5, col - 20, 3, 10);
	box(w2, 0, 0);  
	w3 = newwin(row -20, col - 45, 10, 35);
	box(w3, 0, 0);
	SetColor(w, w1, w2, w3);
 	getmaxyx(w2, row, col);	/* get the number of rows and columns */
	sprintf(title, "%-25s",  "CUSTOMER BILLING SYSTEM");
	mvwprintw( w2, 2, (col-strlen(title))/2, "%s", title);                    	/* print the message at the center of the screen */
	sprintf(item, "%-20s",  "Select Options");
	mvwprintw(w, 0, 0, "%s", item);
	Setting(w1);
	itemsearch(w3);
      
       
// now print all the menu items and highlight the first one
	for(i = 0; i < 6; i++) {
	        if(i == op)
	            wattron(w, A_STANDOUT); // highlights the first item.
	        else
	            wattroff(w, A_STANDOUT);
	        sprintf(item, "%-7s",  list[i]);
	        mvwprintw(w, i + 1, 2, "%s", item);
	}
	wrefresh(w1);
	wrefresh(w2);
	wrefresh(w3);
	wrefresh(w); // update the terminal screen
	i = 0;
	noecho(); // disable echoing of characters on the screen
	keypad(w, TRUE); // enable keyboard input for the window.
   
       // get the input
	while((ch = wgetch(w)) == KEY_UP || ch== KEY_DOWN) {
         // right pad with spaces to make the items appear with even width.
		sprintf(item, "%-20s", list[i]);
		mvwprintw(w, i + 1, 2, "%s", item);
         // use a variable to increment or decrement the value based on the input.
		switch(ch) {
			case KEY_UP:
				i--;
				i = ( i < 0 ) ? 5 : i;
				break;
			case KEY_DOWN:
				i++;
				i = ( i > 5 ) ? 0 : i;
				break;
		}
		op = i;
            // now highlight the next item in the list.
		wattron( w, A_STANDOUT );
		sprintf(item, "%-7s",  list[i]);
		mvwprintw( w, i+1, 2, "%s", item);
		wattroff( w, A_STANDOUT );
	}
	while((ch2 = wgetch(w)) != 27) {
		driver3(ch2, w3);
		wrefresh(w3);
		op = 0;
		if(ch2 == 27)
		break;
	}
	wrefresh(w3);
	unpost_form(form);
	free_form(form);
	free_field(fields[0]);
	free_field(fields[1]);
	free_field(fields[2]);
	free_field(fields[3]);
	delwin(w);
	delwin(w1);
	delwin(w2);
	delwin(w3);
	endwin();
}
void login(){
	WINDOW *w,*w1,*w2,*w3;
	char item[20], title[25];
	int ch, i = 0, width = 7, row ,col;
	initscr(); // initialize Ncurses
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, row, col);
	w = newwin(row - 20, 23, 10, 10); // create a new window
	box(w, 0, 0); // sets default borders for the window
	w1 = newwin(row -5, col - 5, 1, 1);
	box(w1, 0, 0);
	w2 = newwin(5, col - 20, 3, 10);
	box(w2, 0, 0);  
	w3 = newwin(row -20, col - 45, 10, 35);
	box(w3, 0, 0);
	SetColor(w, w1, w2, w3);
 	getmaxyx(w2, row, col);	/* get the number of rows and columns */
	sprintf(title, "%-25s",  "CUSTOMER BILLING SYSTEM");
	mvwprintw( w2, 2, (col-strlen(title))/2, "%s", title);                    	/* print the message at the center of the screen */
	getmaxyx(w,row,col);
	mvwprintw( w, 0, 0, "%s", "Select Options" );
	mvwprintw( w, 1, 1, "%s", "Please " );
	mvwprintw( w, 2, 1, "%s", "Login" );
	mvwprintw( w, 3, 1, "%s", "To" );
	mvwprintw( w, 4, 1, "%s", "See" );
	mvwprintw( w, 5, 1, "%s", "Options" );	
	Setting(w1);
	logfun(w3);
       
// now print all the menu items and highlight the first one
    
	wrefresh(w1);
	wrefresh(w2);
	wrefresh(w3);
	wrefresh(w); // update the terminal screen
	i = 0;
	noecho(); // disable echoing of characters on the screen
	keypad(w, TRUE); // enable keyboard input for the window.
   	while((ch2 = wgetch(w))) {		
		int hp;
		hp = driver1(ch2, w3);
		wrefresh(w3);
	 	op = 0;
		if(hp == 2){
			f1();
			break;
		}
	}
	wrefresh(w3);
	unpost_form(form);
	free_form(form);
	free_field(fields[0]);
	free_field(fields[1]);
	free_field(fields[2]);
	free_field(fields[3]);
	delwin(w);
	delwin(w1);
	delwin(w2);
	delwin(w3);
	endwin();
	return;
}
/* to create a file */
void fcreate(){
	FILE *p;
	p = fopen("db.dat", "w");
	fclose(p);
	return;
}

int main(int argc, char *argv[]) {
	
	int fd;
	if(argc == 2) {
		if(strcmp(argv[1], "-h") == 0){
			printf("1)Run ./project\n2)Login to see the menu options \n3)After Pressing Enter On Options Press Left Arrow Key To Enter The Window \n4)Use Up Arrow And Down Arrow Keys To Move Through Fields \n5)After Adding Product Press Any Arrow Key And Then ESC To Move To Main Menu \n6)Only Some Items Are Listed In Serch Item Window. To search The Product Use Search Item \n");
			return 0;	
		}
		if(strcmp(argv[1], "-help") == 0){
			printf("1)Run ./project\n2)Login to see the menu options \n3)After Pressing Enter On Options Press Left Arrow Key To Enter The Window \n4)Use Up Arrow And Down Arrow Keys To Move Through Fields \n5)After Adding Product Press Any Arrow Key And Then ESC To Move To Main Menu \n6)Only Some Items Are Listed In Serch Item Window. To search The Product Use Search Item \n");
			return 0;	
		}
	}
	if(fd = open( "db.dat" ,O_RDONLY))
		close(fd);
	if( fd == -1){
		fcreate();			
	}
  	ReadD();
	login();
	while(op >= 0){
		sid = 0;
		f1();
		switch(op){
			case 0:
				f1();
				break;
			case 1:
				f2();
				break;
			case 2:
				f3();
				break;
			case 3:
				f4();
   		      		if(found == 1)
   		      			f5();
				break;
			case 4:
				f6();
				break;
			case 5:	
		          	op = -1;
			break;
			default:
				op = -1;
				break;
		}     
	}
	WriteD();
	return 0;
}
