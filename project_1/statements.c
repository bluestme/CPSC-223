#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    int index = 0; // To indicate if the input is in the comment, normal script, quotes or parantheses
    int count_semi = 0;
    int count_front = 0;
    int count_back = 0;
    int cont_index = 0; // To determine if it is a continuous backslash
    char curr;
    char break_pt;
    while (index < 10)
    {
        if (index == 0)
        {
            break_pt = scanf("%c", &curr);
            if (curr == ';')
            {
                count_semi++;
                continue;
            }
            if (curr == '{')
            {
                count_front++;
                continue;
            }
            if (curr == '}')
            {
                count_back++;
                continue;
            }
            if (curr == '<')
            {
                break_pt = scanf("%c", &curr);
                if (break_pt == EOF) break;
                if (curr == '%')
                {
                    count_front++;
                }
                continue;
            }
            if (curr == '%')
            {
                break_pt = scanf("%c", &curr);
                if (break_pt == EOF) break;
                if (curr == '>')
                {
                    count_back++;
                }
                continue;
            }
            if (curr == '(')
            {
                index = 1;
                continue;
            }
            if (curr == '\'')
            {
                index = 2;
                continue;
            }
            if (curr == '\"')
            {
                index = 3;
                continue;
            }
            if (curr == '/')
            {
                break_pt = scanf("%c", &curr);
                if (curr == '*')
                {
                    index = 4;
                    continue;
                }
                if (curr == '/')
                {
                    index = 5;
                    continue;
                }
                if (break_pt == EOF) break;
                else continue;
            }
            if (break_pt == EOF) break;
        }
        if (index == 1) // The case to be in a paratheses
        {
            break_pt = scanf("%c", &curr);
            if (curr == ')')
            {
                index = 0;
                continue;
            }
            if (break_pt == EOF) break;
            else continue;
        }
        if (index == 2) // The case to be in a single quote
        {
            break_pt = scanf("%c", &curr);
            if (curr == '\'')
            {
                index = 0;
                continue;
            }
            if (break_pt == EOF) break;
            else continue;
        }
        if (index == 3) // The case to be in a double quote
        {
            break_pt = scanf("%c", &curr);
            if (curr == '\"')
            {
                index = 0;
                continue;
            }
            if (break_pt == EOF) break;
            else continue;
        }
        if (index == 4) // The case to be in /*  comments
        {
            break_pt = scanf("%c", &curr);
            if (curr == '*')
            {
                break_pt = scanf("%c", &curr);
                if (curr == '/')
                {
                    index = 0;
                    continue;
                }
                if (break_pt == EOF) break;
                else continue;
            }
            if (break_pt == EOF) break;
            else continue;
        }
        if (index == 5) // The case to be in a // comments
        {
            break_pt = scanf("%c", &curr);
            if (curr == '\\')
            {
                cont_index = 1;
                if (break_pt == EOF) break;
                continue;
            }
            if (curr == '\n')
            {
                if (cont_index == 0)
                    index = 0;
                if (break_pt == EOF) break;
                continue;
            }
            if (curr != '\\')
            {
                cont_index = 0;
                if (break_pt == EOF) break;
                continue;
            }
            if (break_pt == EOF) break;
            else continue;
        }
    }
    printf("%d\n", count_semi + count_back);
    return 0;
}
