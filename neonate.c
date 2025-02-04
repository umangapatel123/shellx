#include "headers.h"

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

/**
 * Enable row mode for the terminal
 * The ECHO feature causes each key you type to be printed to the terminal, so you can see what you’re typing.
 * Terminal attributes can be read into a termios struct by tcgetattr().
 * After modifying them, you can then apply them to the terminal using tcsetattr().
 * The TCSAFLUSH argument specifies when to apply the change: in this case, it waits for all pending output to be written to the terminal, and also discards any input that hasn’t been read.
 * The c_lflag field is for “local flags”
 */
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

/**
 * stdout and stdin are buffered we disable buffering on that
 * After entering in raw mode we read characters one by one
 * Up arrow keys and down arrow keys are represented by 3 byte escape codes
 * starting with ascii number 27 i.e. ESC key
 * This way we interpret arrow keys
 * Tabs are usually handled by the term, but here we are simulating tabs for the sake of simplicity
 * Backspace move the cursor one control character to the left
 * @return
 */

int get_recent_pid()
{
    FILE *fp = fopen("/proc/loadavg", "r");
    if (fp == NULL)
    {
        printf("Error: Could not open file /proc/loadavg\n");
        return -1;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int count = 0;
    while ((read = getline(&line, &len, fp)) != -1)
    {
        char *token = strtok(line, " \t");
        while (token != NULL)
        {
            if (count == 4)
            {
                int pid = atoi(token);
                return pid;
            }
            count++;
            token = strtok(NULL, " \t");
        }
    }
    fclose(fp);
    return -1;
}

bool key_detect(char *inp, char c)
{
    int pt = 0;
    memset(inp, '\0', 100);
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (iscntrl(c))
        {
            if (c == 10)
                break;
            else if (c == 27)
            {
                char buf[3];
                buf[2] = 0;
                if (read(STDIN_FILENO, buf, 2) == 2)
                { // length of escape code
                    printf("\rarrow key: %s", buf);
                }
            }
            else if (c == 127)
            { // backspace
                if (pt > 0)
                {
                    if (inp[pt - 1] == 9)
                    {
                        for (int i = 0; i < 7; i++)
                        {
                            printf("\b");
                        }
                    }
                    inp[--pt] = '\0';
                    printf("\b \b");
                }
            }
            else if (c == 9)
            { // TAB character
                inp[pt++] = c;
                for (int i = 0; i < 8; i++)
                { // TABS should be 8 spaces
                    printf(" ");
                }
            }
            else if (c == 4)
            {
                exit(0);
            }
            else
            {
                printf("%d\n", c);
            }
        }
        else
        {
            inp[pt++] = c;
            // printf("%c", c);
        }
        if (c ==120)
        {
            printf("Exiting\n");
            disableRawMode();
            return true;
        }
    }
}

void neonate(char **args, char *permenant_home)
{
    if (args[0] == NULL || strcmp(args[0], "-n") != 0)
    {
        printf("neonate: Usage: neonate -n [time_arg] \n");
        return;
    }
    if (args[1] == NULL)
    {
        printf("neonate: Usage: neonate -n [time_arg] \n");
        return;
    }
    if (args[2] != NULL)
    {
        printf("neonate: Usage: neonate -n [time_arg] \n");
        return;
    }
    int time_arg = atoi(args[1]);
    if (time_arg < 0)
    {
        printf("neonate: time_arg should be positive\n");
        return;
    }
    char *inp = malloc(sizeof(char) * 100);
    char c;
    setbuf(stdout, NULL);
    enableRawMode();
    // printf("%d\n", output);
    int pid = fork();
    if (pid < 0)
    {
        printf("neonate: Error: Could not fork\n");
        return;
    }
    else if (pid == 0)
    {
        while (1)
        {
            int output = get_recent_pid();
            if (output == -1)
            {
                printf("neonate: Error: Could not get recent pid\n");
                return;
            }
            printf("%d\n", output);
            sleep(time_arg);
        }
    }
    else
    {
        while (1)
        {
            bool output = key_detect(inp, c);
            if (output)
            {
                kill(pid, SIGKILL);
                break;
            }
        }
    }
    disableRawMode();
}

// int main()
// {
//     char *inp = malloc(sizeof(char) * 100);
//     char c;
//     while (1)
//     {
//         setbuf(stdout, NULL);
//         enableRawMode();
//         printf("Prompt>");
//         memset(inp, '\0', 100);
//         int pt = 0;
//         while (read(STDIN_FILENO, &c, 1) == 1)
//         {
//             if (iscntrl(c))
//             {
//                 if (c == 10)
//                     break;
//                 else if (c == 27)
//                 {
//                     char buf[3];
//                     buf[2] = 0;
//                     if (read(STDIN_FILENO, buf, 2) == 2)
//                     { // length of escape code
//                         printf("\rarrow key: %s", buf);
//                     }
//                 }
//                 else if (c == 127)
//                 { // backspace
//                     if (pt > 0)
//                     {
//                         if (inp[pt - 1] == 9)
//                         {
//                             for (int i = 0; i < 7; i++)
//                             {
//                                 printf("\b");
//                             }
//                         }
//                         inp[--pt] = '\0';
//                         printf("\b \b");
//                     }
//                 }
//                 else if (c == 9)
//                 { // TAB character
//                     inp[pt++] = c;
//                     for (int i = 0; i < 8; i++)
//                     { // TABS should be 8 spaces
//                         printf(" ");
//                     }
//                 }
//                 else if (c == 4)
//                 {
//                     exit(0);
//                 }
//                 else
//                 {
//                     printf("%d\n", c);
//                 }
//             }
//             else
//             {
//                 inp[pt++] = c;
//                 printf("%c", c);
//             }
//         }
//         disableRawMode();

//         printf("\nInput Read: [%s]\n", inp);
//     }
//     return 0;
// }
