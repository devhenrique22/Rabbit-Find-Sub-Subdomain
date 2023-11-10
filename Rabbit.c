/*Estudando Scoket em C 
Use gcc Rabbit.c -o Rabbit 

https://github.com/devhenrique22

Um find subdomain simples apenas para fins didaticos 
*/


#define MAX 256
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int save_host(char *output, char *host, int port)
{
    int result = 0;
    char *content = NULL;

    if (!host || !port || !output)
        return 0;

    if (!(content = malloc(sizeof(char) * MAX)))
    {
        printf("Error allocating memory.\n");
        return result;
    }
    memset(content, '\0', sizeof(char) * MAX);
    sprintf(content, "%s:%d\n", host, port);

    FILE *fp = NULL;
    if ((fp = fopen(output, "a+")) != NULL)
    {
        fwrite(content, strlen(content), 1, fp);
        fclose(fp);
        free(content);
        result = 1;
    }
    else
        printf("Error opening file: %s\n", output);

    return result;
}

int check_sub_domain(char *subdomain, char *host, int port, char *output)
{
    if (!subdomain || !host || !port || !output)
        return 0;

    struct sockaddr_in address;
    struct hostent *info;
    char *new_host = NULL;

    for (int a = 0; subdomain[a] != '\0'; a++)
    {
        if (subdomain[a] == '\n')
        {
            subdomain[a] = '\0';
            break;
        }
    }

    if (!(new_host = malloc(sizeof(char) * MAX)))
    {
        printf("Error allocating memory.\n");
        exit(0);
    }

    memset(new_host, '\0', sizeof(char) * MAX);
    sprintf(new_host, "%s.%s", subdomain, host);

    if ((info = gethostbyname(new_host)) == NULL)
    {
        printf("Not found: %s\n", new_host);
        free(new_host);
        return 0;
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = *(u_long *)info->h_addr_list[0];

    printf("-> Connected to %s - IP: %s:%d\n",
           new_host, inet_ntoa(address.sin_addr), port);

    save_host(output, new_host, port);

    free(new_host);
    return 0;
}

int main(int argc, char *argv[])
{
    char *host = NULL;
    char *list = NULL;
    char *output = NULL;
    int port = 0;

    if (!(host = malloc(sizeof(char) * MAX)) ||
        !(list = malloc(sizeof(char) * MAX)) ||
        !(output = malloc(sizeof(char) * MAX)))
    {
        printf("Error allocating memory.\n");
        exit(0);
    }
    memset(host, '\0', sizeof(char) * MAX);
    memset(list, '\0', sizeof(char) * MAX);
    memset(output, '\0', sizeof(char) * MAX);

    if (argc != 9)
    {
        printf("Rabbit PortScan\n");
        printf("Usage: ./%s -h host.com -l list-of-words.txt -p port -o output\n", argv[0]);
        exit(0);
    }

    for (int a = 0; a < argc; a++)
    {
        if (strcmp(argv[a], "-h") == 0)
            memcpy(host, argv[a + 1], strlen(argv[a + 1]));
        else if (strcmp(argv[a], "-l") == 0)
            memcpy(list, argv[a + 1], strlen(argv[a + 1]));
        else if (strcmp(argv[a], "-p") == 0)
            port = atoi(argv[a + 1]);
        else if (strcmp(argv[a], "-o") == 0)
            memcpy(output, argv[a + 1], strlen(argv[a + 1]));
    }

    if (!host || !list || !port || !output)
    {
        printf("Invalid parameters.\n");
        printf("Usage: ./%s -h host.com -l list-of-words.txt -p port -o output\n", argv[0]);
        exit(0);
    }

    FILE *fp = NULL;
    char *line = NULL;

    if (!(line = malloc(sizeof(char) * MAX)))
    {
        printf("Error allocating memory.\n");
        exit(0);
    }
    memset(line, '\0', sizeof(char) * MAX);

    if ((fp = fopen(list, "r")) != NULL)
    {
        while (fgets(line, MAX - 1, fp))
        {
            check_sub_domain(line, host, port, output);
        }
    }
    else
        printf("File not found: %s\n", list);

    free(output);
    free(line);
    free(host);
    free(list);

    return 0;
}
