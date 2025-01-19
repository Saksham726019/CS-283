#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here

int setup_buff(char *buff, char *user_str, int len)
{
    //TODO: #4:  Implement the setup buff as per the directions
    
    int str_len = 0;    // Variable to hold the length of user_str.
    int i = 0;          // Variable to hold the index for user_str.
    int j = 0;          // Variable to hold the index for buff.

    if (user_str == NULL || buff == NULL)
    {
        return -2;      // Null pointer error.
    }

    // Iterating until we hit the null termination of user_str.
    while (user_str[i] != '\0')
    {
        if (i >= len)
        {
            return -1;      // The user supplied string is too large.
        }
        
        // Checking if it's a white space or tab and adding to the buff as single whitespace.
        if (user_str[i] == ' ' || user_str[i] == '\t')
        {
            if (j > 0 && buff[j - 1] != ' ')
            {
                buff[j] = ' ';
                j++;
                str_len++;
            }
        } else      // Adding the non-whitespace char to buff.
        {
            buff[j] = user_str[i];
            j++;
            str_len++;
        }
        i++;
    }

    // Removing the last white space from buff.
    if (j > 0 && buff[j - 1] == ' ')
    {
        j--;
    }
    
    // Adding '.' until len.
    while (j < len)
    {
        buff[j] = '.';
        j++;
    }
     
    // Returning the user_str length.
    return str_len;
}

void print_buff(char *buff, int len)
{
    printf("Buffer:  ");
    for (int i=0; i<len; i++)
    {
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename)
{
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}

// Function to count the total number of words in the buff.
int count_words(char *buff, int len, int str_len)
{
    //YOU MUST IMPLEMENT
    if (str_len > len)
    {
        return -1;
    }
    
    int word_count = 0;

    // When we hit a space character or tab character, it means we finished processing a word.
    // When the index == str_len, that means we processed the last word.
    for (int i = 0; i < str_len; i++)
    {
        if (i == str_len - 1)
        {
            word_count++;
        } else if (buff[i] != ' ' && buff[i] != '\t')
        {
            continue;
        } else
        {
            word_count++;
        }
    }
    
    return word_count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

// Function to swap two characters.
void swap(char* a, char* b)
{
    char temp = *a;
    *a = *b;
    *b = temp;
}

// Function to reverse the characters in buff (in place). Uses the helper function swap. Two pointer approach.
void reverseString(char* buff, int str_len)
{
    int i = 0;
    int j = str_len - 1;

    while (i < j)
    {
        swap(&buff[i], &buff[j]);
        i++;
        j--;
    }
    
}

// Function to print words in order.
void  wordPrint(char* buff, int str_len)
{
    int words_count = count_words(buff, BUFFER_SZ, str_len);
    int char_count = 0;
    int index = 0;

    printf("Word Print\n");
    printf("----------\n");

    // Here, we will keep printing the character until we hit a space character. 
    // After that, we move on to next for loop iteration, meaning next word.
    // Once index == str_len, that means we have processed all the characters.
    for (int i = 0; i < words_count; i++)
    {
        printf("%d. ", i+1);
        while (buff[index] != ' ' && index != str_len)
        {
            printf("%c", buff[index]);
            index++;
            char_count++;
        }
        printf(" (%d)\n", char_count);
        char_count = 0;
        index++;     
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // This checks the number of arguments passed. The first (argc < 2) makes sure enough arguments are passed.
    // And, the (*argv[1] != '-') checks if the second argument starts with '-'. 
    // These both checks avoids accessing argv[1] if it doesn't exist.
    if ((argc < 2) || (*argv[1] != '-'))
    {
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h')
    {
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // This if statement checks if at least 3 arguments were passed.
    // argv[0] is the executable file.
    // argv[1] is the flag.
    // argv[2] is the user string.
    // If the program doesn't recieve these three arguments, it will not run and prevents unexpected behavior.
    if (argc < 3)
    {
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = malloc(BUFFER_SZ);
    if(buff == NULL)
    {
        printf("Memory allocation failure\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0)
    {
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt)
    {
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0)
            {
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            reverseString(buff, user_str_len);
            break;
        
        case 'w':
            wordPrint(buff, user_str_len);
            break;

        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE
// 1. Makes the code flexible because in case the buffer size is changed, it makes the function reusable for different buffer sizes.
// 2. Passing the pointer to buffer allows to manipulate the data easily.
// 3. By passing the length alongside the buffer, the function can iterate through the buffer as per the size of length.
// If length was not passed, we would have to look for the value of buffer length everytime we need to iterate. In case buffer size changes, the entire code needs to be fixed.