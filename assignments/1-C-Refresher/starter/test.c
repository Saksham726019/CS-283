#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int count_words(char* buff, int str_len)
{
    //YOU MUST IMPLEMENT
    int word_count = 0;

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

// Function to swap two characters.
void swap(char* a, char* b)
{
    char temp = *a;
    *a = *b;
    *b = temp;
}

void reverseString(char* buff, int str_len)
{
    int i = 0;
    int j = str_len - 1;

    while (i != j)
    {
        swap(&buff[i], &buff[j]);
        i++;
        j--;
    }   
}



int main()
{
    char buff[50] = "Hello world";

    int count = count_words(buff, 11);
    printf("Word count: %d\n", count);

    reverseString(buff, 11);
    printf("Reversed String: %s\n", buff);
}
