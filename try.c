#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
// #include <libxml/HTMLparser.h>

// Function to perform DNS resolution and return the IP address
int resolveDNS(const char *hostname, char *ipAddress) {
    struct hostent *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **)he->h_addr_list;
    strcpy(ipAddress, inet_ntoa(*addr_list[0]));

    return 0;
}

// Function to fetch and print specific sections from the man page
void fetchManPage(const char *command_name) {
    char hostname[] = "man.he.net";
    char ipAddress[100];
    int sock;
    struct sockaddr_in server;
    char request[1000];

    // Resolve DNS to get IP address
    if (resolveDNS(hostname, ipAddress) != 0) {
        fprintf(stderr, "DNS resolution failed.\n");
        return;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
    server.sin_addr.s_addr = inet_addr(ipAddress);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("connect");
        return;
    }

    // Prepare the HTTP GET request
    sprintf(request, "GET /?topic=%s&section=all HTTP/1.1\r\nHost: %s\r\n\r\n", command_name, hostname);

    // Send the HTTP GET request
    if (send(sock, request, strlen(request), 0) == -1) {
        perror("send");
        return;
    }

    // Receive and parse the HTML response
    char response[4096];
    char *content_start = NULL;
    int content_length = 0;
    while (recv(sock, response, sizeof(response), 0) > 0) {
        // printf("%s", response);
        if (!content_start) {
            // Find the start of the HTML content
            content_start = strstr(response, "<pre>");
            if (content_start) {
                // Skip the opening <pre> tag
                content_start += 5;
            }
        } else {
            // Calculate content length
            content_length += strlen(response);
            printf("%s", response);
        }
    }

    // Close the socket
    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <command_name>\n", argv[0]);
        return 1;
    }

    char *command_name = argv[1];
    printf("%s\n", command_name);
    fetchManPage(command_name);

    return 0;
}
