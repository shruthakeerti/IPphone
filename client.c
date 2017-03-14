/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<signal.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <fcntl.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/gccmacro.h>
#define BUFSIZE 32
 int sockfd; 
void my_handler_for_sigint(int );

/* A simple routine calling UNIX write() in a loop */
static ssize_t loop_write(int fd, const void*data, size_t size) {
    ssize_t ret = 0;

    while (size > 0) {
        ssize_t r;

        if ((r = write(fd, data, size)) < 0)
            return r;

        if (r == 0)
            break;

        ret += r;
        data = (const uint8_t*) data + r;
        size -= (size_t) r;
    }

    return ret;
}
 // get sockaddr, IPv4or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int numbytes;  
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

     static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };
    pa_simple *sa = NULL;
    int ret = 1;
    int error;   
  
if (argc != 3) {
        fprintf(stderr,"Ip and Port number is missing\n");
        exit(1);
    }

     /* Registering the Signal handler */
  if (signal(SIGINT, my_handler_for_sigint) == SIG_ERR)
      printf("\ncan't catch SIGINT\n");
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    //printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
//printf("Please enter the data\n");

 if (!(sa = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }


for (;;) {//infinte loop to record voice and send to server
        uint8_t buf[BUFSIZE];
	
        /* Record from listener*/
        if (pa_simple_read(sa, buf, sizeof(buf), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish;
        }
/*send to the server*/
if (loop_write(sockfd, buf, sizeof(buf)) != sizeof(buf)) {
            fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
            goto finish;
        }
}
 
    ret = 0;

finish:

    if (sa)
        pa_simple_free(sa);

    return ret;
}
void my_handler_for_sigint(int signumber)//signal handler
{
  char ans[2];
  if (signumber == SIGINT)
  {
    printf("received SIGINT\n");
    printf("Program received a CTRL-C\n");
    printf("Terminate Y/N : "); 
    scanf("%s", ans);
    if (strcmp(ans,"Y") == 0)
    {
       printf("Exiting ....\n");
       close(sockfd);       
       exit(0); 
    }
    else
    {
       printf("Continung ..\n");
    }
  }
}














