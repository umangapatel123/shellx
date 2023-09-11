#include "headers.h"

char* RemovePart(char* str, char* word)
{
    char* temp = strstr(str, word);
    if(temp!=NULL)
    {
        size_t len = (size_t)(temp-str);
        char *temp3 = (char *)malloc(len*sizeof(char));
        strncpy(temp3, str, len);
        temp3[len] = '\0';
        return temp3;
    }
    else
    {
        return str;
    }
}

char *SpecialFormat(char *str)
{
    //replace &lt; with < and &gt; with >
    char *temp = (char *)malloc(100000*sizeof(char));
    strcpy(temp, str);
    char *search = "&lt;";
    char *replace = "<";
    char *pos=temp;
    while(pos=strstr(pos, search))
    {
        memmove(pos+strlen(replace), pos+strlen(search), strlen(pos+strlen(search))+1);
        memcpy(pos, replace, strlen(replace));
        pos+=strlen(replace);
    }
    char *search2 = "&gt;";
    char *replace2 = ">";
    char *pos2=temp;
    while(pos2=strstr(pos2, search2))
    {
        memmove(pos2+strlen(replace2), pos2+strlen(search2), strlen(pos2+strlen(search2))+1);
        memcpy(pos2, replace2, strlen(replace2));
        pos2+=strlen(replace2);
    }
    return temp;
}

char * RemoveHTMLTags(char *str)
{
    char *temp = (char *)malloc(100000*sizeof(char));
    strcpy(temp, str);
    char *output= (char *)malloc(100000*sizeof(char));
    int i=0, j=0;
    bool tag = false;
    while(temp[i]!='\0')
    {
        if(temp[i]=='<')
        {
            tag = true;
        }
        else if(temp[i]=='>')
        {
            tag = false;
        }
        else if(!tag)
        {
            output[j] = temp[i];
            j++;
        }
        i++;
    }
    output[j] = '\0';
    return output;
}

void iMan(char **args, char * permenant_home)
{
    if(args[0] == NULL)
    {
        printf("iMan: Expected argument to \"iMan\"\n");
        return;
    }
    if(args[1]!=NULL)
    {
        printf("iMan: Too many arguments\n");
        return;
    }
    char *command = args[0];

    struct addrinfo hints, *res;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if(getaddrinfo(HOST_NAME, HOST_PORT, &hints, &res) != 0)
    {
        printf("iMan: Error in DNS resolution\n");
        return;
    }

    if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
    {
        printf("iMan: Error in socket creation\n");
        return;
    }

    if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        printf("iMan: Error in connecting to server\n");
        return;
    }

    char *request = (char *)malloc(100000*sizeof(char));
    // printf("iMan: Requesting manual for %s\n", args[0]);
    // snprintf(request, sizeof(request), "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", args[0], HOST_NAME);
    // alternative to snprintf
    sprintf(request, "GET /?topic=%s&section=all HTTP/1.1\r\nHost: %s\r\n\r\n", args[0], HOST_NAME);
    if(send(sockfd, request, strlen(request), 0) == -1)
    {
        printf("iMan: Error in sending request\n");
        close(sockfd);
        return;
    }

    char *response = (char *)malloc(100000*sizeof(char));
    int bytes;
    bool start_content = false;
    while((bytes = recv(sockfd, response, 100000, 0)) > 0)
    {
        response[bytes] = '\0';
        if ( strstr(response, "No matches for ") != NULL )
        {
            printf("ERROR\n");
            printf("        No such command found\n");
            close(sockfd);
            return;
        }
        if(strstr(response, "NAME\n") != NULL)
        {
            char *temp= strstr(response, "NAME\n");
            char *toPrint= (char *)malloc(100000*sizeof(char));
            if(temp!=NULL)
            {
                start_content = true;
                strcpy(toPrint, temp);
            }
            else
            {
                strcpy(toPrint, response);
            }
            char *temp3= (char *)malloc(100000*sizeof(char));
            strcpy(temp3, toPrint);
            if(strstr(temp3, "<center>")!=NULL)
            {
                char *temp2 = RemovePart(temp3, "<center>");
                temp2 = RemoveHTMLTags(temp2);
                temp2 = SpecialFormat(temp2);
                printf("%s", temp2);
            }
            else
            {
                toPrint = RemoveHTMLTags(toPrint);
                toPrint = SpecialFormat(toPrint);
                printf("%s", toPrint);
            }
        }
        else if (start_content)
        {
            char *temp3= (char *)malloc(100000*sizeof(char));
            strcpy(temp3, response);
            if(strstr(temp3, "<center>")!=NULL)
            {
                char *temp2 = RemovePart(temp3, "<center>");
                temp2 = RemoveHTMLTags(temp2);
                temp2 = SpecialFormat(temp2);
                printf("%s", temp2);
            }
            else
            {
                char *temp2 = RemoveHTMLTags(response);
                temp2 = SpecialFormat(temp2);
                printf("%s", temp2);
            }
        }
        // else
        // {
        //     printf("%s", response);
        // }
    }

    if(bytes == -1)
    {
        printf("iMan: Error in receiving response\n");
        close(sockfd);
        return;
    }

    close(sockfd);
    return;
}