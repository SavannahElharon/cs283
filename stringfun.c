#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BUFFER_SZ 50
//prototypes
void usage(char *);
void print_buff(char *, int);
int setup_buff(char *, char *, int);
//prototypes for functions to handle required functionality
int count_words(char *, int, int);
int reverse_string(char *, int);
int word_print(char *, int);
int string_replace(char *buff, int buffLen, char *search, char *replace);
//add additional prototypes here


int setup_buff(char *buff, char *user_str, int len) {
	  //TODO: #4:  Implement the setup buff as per the directions
	int i = 0, j = 0;
	char *ptr = user_str;
	while (*ptr && j < len) {
		if (*ptr != ' ' && *ptr != '\t') {
			buff[j++] = *ptr;
		} else if (j > 0 && buff[j - 1] != ' ') {
			buff[j++] = ' ';
		}
		ptr++;
	}
	if (j >= len) {
		return -1; 
	}
	while (j < len) {
		buff[j++] = '.';
	}
	return j;
}


void print_buff(char *buff, int len) {
	printf("Buffer:  ");
	for (int i = 0; i < len; i++) {
		putchar(*(buff + i));
	}
	putchar('\n');
}

void usage(char *exename) {
	printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}


int count_words(char *buff, int len, int str_len) {
	//YOU MUST IMPLEMENT
	int count = 0;
	int inWord = 0;
	//loop through each character in the input buffer up to the given string length
	for (int i = 0; i < str_len; i++) {
		if (*(buff + i) != ' ' && *(buff + i) != '.') {
			if (!inWord) {
			//if in word set flag to true
				inWord = 1;
				count++;
			}	
		} else {
			inWord = 0;
		}
	}
	return count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int reverse_string(char *buff, int str_len) {
 // Check for invalid input
	if (buff == NULL || str_len <= 0) {
        return -1;
    }
	char *start = buff;
    char *end = buff + str_len - 1;
    while (end > buff && *end == '.') {
        end--;
    }
    //swap chars between start and end to reverse the string
    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
    return 0;
}


int word_print(char *buff, int str_len) {
	  if (buff == NULL || str_len <= 0) {
        return -1;
    }
	printf("Word Print\n----------\n");
	int start = 0;
	int wordLen = 0;
	int index = 1;
	int wordCount = 0;
	 //iterate through input
	for (int i = 0; i < str_len; i++) {
		if (*(buff + i) != ' ' && *(buff + i) != '.') {
		//iterate through each word until u hit end then restart
			if (wordLen == 0) {
				start = i;
			}
			wordLen++;
		} else if (wordLen > 0) {
			printf("%d. ", index++);
			for (int j = 0; j < wordLen; j++) {
				putchar(*(buff + start + j));
			}
			printf(" (%d)\n", wordLen);
			wordLen = 0;
			wordCount++;
		}
	}
	if (wordLen > 0) {
		printf("%d. ", index++);
		//print each word in the list form
		for (int j = 0; j < wordLen; j++) {
			putchar(*(buff + start + j));
		}
		printf(" (%d)\n", wordLen);
	}
	 printf("\nNumber of words returned: %d\n", wordCount);
	 return 0;
}
int string_replace(char *buff, int buffLen, char *search, char *replace) {
    char *x = buff;
    char *match = NULL;
    //locate first occurance of word to be replaced
    while (*x) {
        char *start = x;
        char *y = search;
        while (*y && *x && *x == *y) {
            x++;
            y++;
        }
        if (!*y) {
            match = start;
            break;
        }
        x = start + 1;
    }
	 //if no match is found, return -1
    if (!match) {
        return -1;
    }
    int searchLen = 0;
    int replaceLen = 0;
    for (char *y = search; *y; y++) searchLen++;
    for (char *y = replace; *y; y++) replaceLen++;
    //adjust the buffer if the replace string length differs from the search string length
    if (replaceLen != searchLen) {
        char *end = buff;
        while (*end) end++;
        int tailLen = end - (match + searchLen);
        //shift tail
        if (replaceLen > searchLen) { 
            for (char *r = end + (replaceLen - searchLen); r >= match + replaceLen; r--) {
                *r = *(r - (replaceLen - searchLen));
            }
        } else {
            for (char *r = match + replaceLen; r < end; r++) {
                *r = *(r + (searchLen - replaceLen));
            }
        }
    }
    for (int i = 0; i < replaceLen; i++) {
        match[i] = replace[i];
    }
    return 0;
}
 
int main(int argc, char *argv[]) {
	char *buff;            //placeholder for the internal buffer
	char *input_string;    //holds the string provided by the user on cmd line
	char opt;              //used to capture user option from cmd line
	int rc;                //used for return codes
	int user_str_len;      //length of user supplied string

   //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
	// This is safe because this conditional ensures that there aree 2 arguments before derefrencing argv so it ensures that argv[1] exists.
	if ((argc < 2) || (*argv[1] != '-')) {
		usage(argv[0]);
		exit(1);
	}
	
	opt = (char) *(argv[1] + 1); //get the option flag
	
	//handle the help flag and exit normally
	if (opt == 'h') {
		usage(argv[0]);
		exit(0);
	}
	
	//WE NOW WILL HANDLE THE REQUIRED OPERATIONS
	//TODO:  #2 Document the purpose of the if statement below
	// This if statement checks to make sure the user provided 3 aarguments.
	if (argc < 3) {
		usage(argv[0]);
		exit(1);
	}
	input_string = argv[2]; //capture the user input string
	
	//TODO:  #3 Allocate space for the buffer using malloc and
	    //          handle error if malloc fails by exiting with a 
	        //          return code of 99
	            // CODE GOES HERE FOR #3
	buff = (char *)malloc(BUFFER_SZ);
	if (!buff) {
		printf("Error: Memory allocation failed.\n");
		exit(99);
	}
	user_str_len = setup_buff(buff, input_string, BUFFER_SZ);
	if (user_str_len < 0) {
		printf("Error setting up buffer, error = %d", user_str_len);
		exit(2);
	}
	switch (opt) {
	case 'c':
		rc = count_words(buff, BUFFER_SZ, user_str_len);
		if (rc < 0) {
		printf("Error counting words, rc = %d", rc);
		exit(2);
		}
		printf("Word Count: %d\n", rc);
		break;
	//TODO:  #5 Implement the other cases for 'r' and 'w' by extending
	        //       the case statement options
	case 'r':
		rc = reverse_string(buff, user_str_len);
		 if (rc < 0) {
        printf("Error reversing string, rc = %d", rc);
        exit(3);
        }
		break;
	case 'w':
		rc = word_print(buff, user_str_len);
		if (rc < 0) {
        printf("Error countingwords, rc = %d", rc);
        exit(3);
        }
		break;
	case 'x': 
    rc = string_replace(buff, BUFFER_SZ, argv[3], argv[4]);
    if (rc < 0) {
        printf("Error replacing string, rc = %d", rc);
        exit(3);
        }
    printf("Modified String:");
     for (int i = 0; i < BUFFER_SZ && buff[i] != '.'; i++) {
        printf("%c", buff[i]);
    }
    printf("\n");
    break;
		default:
		usage(argv[0]);
		free(buff);
		exit(1);
	}
 //TODO:  #6 Dont forget to free your buffer before exiting
	print_buff(buff, BUFFER_SZ);
	free(buff);
	exit(0);
}
//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          It is good practice because by providing both the pointer and length there is an esurance of safety
// 			and flexivility since it will prevent overflow by limiting the number of operations. It also allows the program to work with varying buffer lengths.

