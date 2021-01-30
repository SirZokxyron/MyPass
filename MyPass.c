#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* TO-DO list
 * - [DONE] Data structures
 * - [DONE] Basic functions to manipulate them
 * - [IN-PROGRESS] Encryption of the structure
 * - Save to a file
 * - Decryption of the structure
 * - [IN-PROGRESS] Main
 */

/* Useful stuff here */

	#define _MAX_PWD_ 100
	#define _MAX_LEN_ 100
	#define true 1
	#define false 0

	typedef char * string;

	void ERROR(string reason, int error_code) {
		fprintf(stderr, "[ERROR] - %s\n", reason);
		exit(error_code);
	}

	void inc_of_string(int * x, string s) {
		if ((*x + 1) >= strlen(s)) *x = 0;
		else *x = *x + 1;
	}

	string add_char_str(string s, char c) {
		int old_len = strlen(s);
		string new_s = (string)malloc(2 + old_len);
		strcpy(new_s, s);
		new_s[old_len] = c;
		new_s[old_len + 1] = '\0';
		return new_s;
	}

/* Data structure */

	//* Structure to hold a single website/username/password combo
	typedef struct combo_ {
		string website;
		string username;
		string password;
	} combo;
	
	//* Structure to hold multiple website/username/password combo
	typedef struct database_ {
		int size;
		combo tab[_MAX_PWD_];
	} database;
	
/* Basic functions to manipulate the data structures */

	//* Setting up the database
	void init_data(database * D) {
		//> Setting the size of the database to 0;
		D->size = 0;
	}

	//* Adding a website/password combo to an existing database
	void add_combo(database * D, string website, string username, string password) {
		//> Verifying if the database exists
		if (!D) ERROR("Trying to add a combo to a NULL database.", -1);

		//> Retrieving the combo we have to modify
		int current_i = D->size;

		//> Modifying the website string
		D->tab[current_i].website = (string)malloc(1 + strlen(website));
		strcpy(D->tab[current_i].website, website);

		//> Modifying the username string
		D->tab[current_i].username = (string)malloc(1 + strlen(username));
		strcpy(D->tab[current_i].username, username);

		//> Modifying the password string
		D->tab[current_i].password = (string)malloc(1 + strlen(password));
		strcpy(D->tab[current_i].password, password);

		//> Increasing the size of the database;
		D->size++;
	}

	//* Clean display of the database content
	void display_database(database D) {
		for (int combo_i = 0; combo_i < D.size; combo_i++) {
			printf("[%d]\t%s - %s - %s\n", combo_i, D.tab[combo_i].website, D.tab[combo_i].username, D.tab[combo_i].password);
		}
	}

	//* Encrypt the content of the database D in file_name using the given master_password
	void encrypt(string file_name, database D, string master_password) {
		FILE * fp = fopen(file_name, "w");

		int mp_i = 0;

		for (int combo_i = 0; combo_i < D.size; combo_i++) {
			for (int website_i = 0; website_i < strlen(D.tab[combo_i].website); website_i++) {
				fprintf(fp, "%d ", D.tab[combo_i].website[website_i] * master_password[mp_i]);
				inc_of_string(&mp_i, master_password);
			}
			fprintf(fp, "-1 ");
			for (int username_i = 0; username_i < strlen(D.tab[combo_i].username); username_i++) {
				fprintf(fp, "%d ", D.tab[combo_i].username[username_i] * master_password[mp_i]);
				inc_of_string(&mp_i, master_password);
			}
			fprintf(fp, "-1 ");
			for (int password_i = 0; password_i < strlen(D.tab[combo_i].password); password_i++) {
				fprintf(fp, "%d ", D.tab[combo_i].password[password_i] * master_password[mp_i]);
				inc_of_string(&mp_i, master_password);
			}
			fprintf(fp, "-1 ");
		}	
		fclose(fp);
	}

	//* Decrypt the content of file_name using the given master_password to fill the database D.
	void decrypt(string file_name, database * D, string master_password) {
		FILE * fp = fopen(file_name, "r");

		int mp_i = 0;
		int read_char;

		string website = (string)malloc(_MAX_LEN_);
		string username = (string)malloc(_MAX_LEN_);
		string password = (string)malloc(_MAX_LEN_);

		//> Waiting for var, if 0 then waiting for website, if 1 then waiting for username, if 2 then waiting for password
		int waiting_for = 0;

		while (!feof(fp)) {  
			fscanf(fp, "%d ", &read_char);
			if (read_char == -1) {
				waiting_for++;
				if (waiting_for == 3) {
					waiting_for = 0;
					add_combo(D, website, username, password);
					website = (string)malloc(_MAX_LEN_);
					username = (string)malloc(_MAX_LEN_);
					password = (string)malloc(_MAX_LEN_);
				}
			} else { 
				switch (waiting_for) {
					case 0:
						website = add_char_str(website, read_char/master_password[mp_i]);
						inc_of_string(&mp_i, master_password);
						break;
					case 1:
						username = add_char_str(username, read_char/master_password[mp_i]);
						inc_of_string(&mp_i, master_password);
						break; 
					case 2:
						password = add_char_str(password, read_char/master_password[mp_i]);
						inc_of_string(&mp_i, master_password);
						break; 
				}
			} 
		}

		fclose(fp);
	}

	//* Displays a help menu
	void display_help() {
		printf("MyPass Help Menu:\n");
		printf("\tV - View existing passwords\n");
		printf("\tA - Add new password\n");
		printf("\tD - Delete existing password\n");
		printf("\tH - Help menu\n");
		printf("\tQ - Save and Quit\n");
	}

/* Main */

	int main (int argc, char * argv[]) {
		//> Get the default encrypted file
		string safe_file = "passwords.safe";

		//> Setting up the database
		database D;
		init_data(&D);

		//> Filling up the database with the content of the encrypted file
		if (argc != 2) ERROR("Waiting for exactly one argument, got 1", -3);
		decrypt(safe_file, &D, argv[1]);

		//> Set up main loop
		int active = true;
		char user_input;
		string website = (string)malloc(_MAX_LEN_); 
		string username = (string)malloc(_MAX_LEN_);
		string password = (string)malloc(_MAX_LEN_);
		int password_index;

		//> Printing a help menu for the user
		display_help();

		while (active) {
			//> Getting the user input and switching upon it
			scanf("%c%*c", &user_input);

			switch (user_input) {
				case 'V':
					//> Displaying the database content
					display_database(D);
					break;
				case 'A':
					//> Getting user inputs for the website/username/password combo
					printf("Website : ");
					scanf("%s%*c", website);
					printf("Username : "); 	
					scanf("%s%*c", username); 
					printf("Password : "); 	
					scanf("%s%*c", password);
					add_combo(&D, website, username, password);
					printf("[INFO] - Password successfully added.\n");
					break;
				case 'D':
					//> Deleting an existing password entry
					printf("[INFO] - Please input the index of the password you wish to delete\n");
					scanf("%d%*c", &password_index);
					if (password_index >= D.size) ERROR("Cannot delete unexisting passwords.", -2);
					for (int combo_i = password_index; combo_i < D.size; combo_i++) {
						D.tab[combo_i] = D.tab[combo_i + 1];
					}
					D.size--;
					break;
				case 'H':
					//> Displaying the help menu
					system("clear");
					display_help();
					break;
				case 'Q':
					encrypt(safe_file, D, argv[1]);
					active = false;
					break;
				default:
					printf("Unknown command : %c\n", user_input);
					break;
			}
		}

		return 0;
	}