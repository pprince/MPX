

/* strtok example by mind@metalshell.com
 *
 * This is an example on string tokenizing
 *
 * 02/19/2002
 *
 * http://www.metalshell.com
 *
 */
 
#include <stdio.h>
#include <string.h>
 
int main(int argc, char *argv[])
{
        int x = 1;
        char str[]="this:is:a:test:of:string:tokenizing";
        char *str1;
 
        /* print what we have so far */
        printf("String: %s\n", str);
 
        /* extract first string from string sequence */
        str1 = strtok(str, ":");
 
        /* print first string after tokenized */
        printf("%i: %s\n", x, str1);
 
        /* loop until finishied */
        while (1)
        {
                /* extract string from string sequence */
                str1 = strtok(NULL, ":");
 
                /* check if there is nothing else to extract */
                if (str1 == NULL)
                {
                        printf("Tokenizing complete\n");
                        exit(0);
                }
 
                /* print string after tokenized */
                printf("%i: %s\n", x, str1);
                x++;
        }
 
        return 0;
 
}


