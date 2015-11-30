#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXTOKENLEN 100
#define MAXTOKENS 100

#define ungetchar(x) ungetc((x), stdin)

extern void exit(int);

void done();

const char IDENTIFIER = 'I',
		   QUALIFIER = 'Q',
		   TYPE = 'T',
		   END = 'E';

struct token {
	char type;
	char string[MAXTOKENLEN];
};

struct token token_stack[MAXTOKENS];
int stack_top = -1;

struct token curr_token;

/*******************************************
 * Utility functions                       *
 ******************************************/

/*
	Look at the current token and return a value of "type",
	"qualifier" or "identifier" 
*/
char classify_string() {
	if(!strcmp(curr_token.string, "const")) return QUALIFIER;
	if(!strcmp(curr_token.string, "volatile")) return QUALIFIER;
	if(!strcmp(curr_token.string, "void")) return TYPE;
	if(!strcmp(curr_token.string, "char")) return TYPE;
	if(!strcmp(curr_token.string, "int")) return TYPE;
	if(!strcmp(curr_token.string, "signed")) return TYPE;
	if(!strcmp(curr_token.string, "unsigned")) return TYPE;
	if(!strcmp(curr_token.string, "short")) return TYPE;
	if(!strcmp(curr_token.string, "long")) return TYPE;
	if(!strcmp(curr_token.string, "float")) return TYPE;
	if(!strcmp(curr_token.string, "double")) return TYPE;

	return IDENTIFIER;
}

/*
	Read the next token into this.string
	if it is alphanumeric, classify_string
	else it must be a single character token
	this.type = the token itself; terminate this.string
	with a nul.
 */
void get_token() {
	char c = 'a';
	while((c = getchar()) != EOF) {
		if(isspace(c))  continue;
		else 			break;
	}

	if(c == EOF) {
		curr_token.type = END;
		return;
	}
	
	if(isalnum(c) || c == '_') {
		int i = 0;
		while(isalnum(c) || c == '_') {
			curr_token.string[i++] = c;
			c = getchar();
		}
		curr_token.string[i] = 0;
		ungetchar(c);

		curr_token.type = classify_string();
		return;
	}
	
	curr_token.type = c;
	if(c == '*') {
		strcpy(curr_token.string, "pointer to ");
	}
	else {
		curr_token.string[0] = c;
		curr_token.string[1] = 0;
	}
}

/*
	get_token and push it onto the stack until the first
	identifier is read.
	Print "identifier is", this.string
	get_token
 */
void read_to_first_identifier() {
	get_token();
	while(curr_token.type != END && curr_token.type != IDENTIFIER) {
		token_stack[++stack_top] = curr_token;
		get_token();
	}
	if(curr_token.type == END) {
		printf("Bad syntax.\n");
		done();
	}
	printf("%s is ", curr_token.string);
	get_token();
}

/*******************************************
 * Parsing functions                       *
 ******************************************/
void deal_with_function_args() {
	char c;
	while((c = getchar()) != ')') 
		putchar(c);
	ungetchar(c);
	printf("is a function returning ");
}

void deal_with_arrays() {
	char c;
	printf("an array[");
	while((c = getchar()) != ']') 
		putchar(c);
	ungetchar(c);
	printf("] of ");
}

void deal_with_any_pointers() {
	while(stack_top > -1 && token_stack[stack_top].type == '*') {
		printf("%s ", token_stack[stack_top].string);
		stack_top--;
	}
}

void deal_with_declarator() {
	if(curr_token.type == '[')
		deal_with_arrays();
	if(curr_token.type == '(')
		deal_with_function_args();

	deal_with_any_pointers();

	while(stack_top > -1) {
		if(token_stack[stack_top].type == '(') {
			if(curr_token.type != ')') {
				printf("Bad Syntax! Expected ')', got %s\n", curr_token.string);
				done();
			}
			get_token();
			stack_top--;
			deal_with_declarator();
		}
		else {
			printf("%s ", token_stack[stack_top].string);
			stack_top--;
		}
	}
}

void done() {
	printf("\n");
	exit(0);
}

int main() {
	read_to_first_identifier();
	deal_with_declarator();
	done();
}