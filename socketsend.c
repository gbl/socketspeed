/* 
 * File:   socketsend.c
 * Author: gbl
 *
 * Created on 28. November 2017, 17:21
 */

#define	_POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef  __MINGW64_VERSION_MAJOR
#include <winsock.h>
#else
#include <netdb.h>
#endif
#include <sys/time.h>

const char *hstrerror(int err);

int main(int argc, char** argv) {

    struct hostent *he;
    struct servent *se;
    int port;
    long size;
    char *endsize;
    int count=1;
    int sock, newsock;
    char *buffer;
    struct sockaddr_in server, client;
    
    if (argc<4) {
        fprintf(stderr, "Usage: %s hostname port bufsize [count]\n", argv[0]);
        exit(1);
    }
    
#ifdef  __MINGW64_VERSION_MAJOR
        WSADATA blah;
        if ((sock=WSAStartup(MAKEWORD(2,2), &blah))!=0) {
                fprintf(stderr, "WSAStartup: Error %d (%x)\n", sock, sock);
                exit(1);
        }
#endif

    if ((he=gethostbyname(argv[1]))==NULL) {
        fprintf(stderr, "gethostbyname %s: %s\n", argv[1], hstrerror(h_errno));
        exit(2);
    }
    
    if ((port=atoi(argv[2]))>0) {
        
    } else if ((se=getservbyname(argv[2], "tcp"))==NULL) {
        fprintf(stderr, "getservbyname %s: %s\n", argv[2], hstrerror(h_errno));
        exit(3);
        port=se->s_port;
    }
    
    size=atol(argv[3]);
    endsize=argv[3]+strlen(argv[3])-1;
    switch (*endsize) {
        case 'k': case 'K': size*=1024; break;
        case 'm': case 'M': size*=1024*1024; break;
        case 'g': case 'G': size*=1024*1024*1024; break;
    }
    
    if (argc>4)
        count=atoi(argv[4]);
    
    printf("listening on IP %d.%d.%d.%d port %d to send %d packets of size %ld\n",
            (unsigned char) he->h_addr_list[0][0],
            (unsigned char) he->h_addr_list[0][1],
            (unsigned char) he->h_addr_list[0][2],
            (unsigned char) he->h_addr_list[0][3],
            port, count, size);
    if ((buffer=malloc(size))==NULL) {
        perror("malloc: no space");
        exit(4);
    }
    memset(buffer, '\0', size);
    
    if ((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))==-1) {
        perror("socket");
        exit(4);
    }
    int opt=1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR)");
    }
    memset(&server, '\0', sizeof server);
    server.sin_family=AF_INET;
    server.sin_port=ntohs(port);
    memcpy(&server.sin_addr.s_addr, he->h_addr_list[0], sizeof server.sin_addr.s_addr);
    if (bind(sock, (struct sockaddr *) &server, sizeof server)<0) {
        perror("bind");
        exit(4);
    }
    listen(sock, 50);
    
    while ((newsock=accept(sock, (struct sockaddr *)&client, &(int){sizeof client}))>=0) {
        int i;
        long rest, n, sent=0;
        struct timeval before, after, between;
        gettimeofday(&before, NULL);
        for (i=0; i<count; i++) {
            rest=size;
            while ((n=send(newsock, buffer+size-rest, rest, 0))>0) {
                sent+=n;
                rest-=n;
            }
            if (n!=0) {
                perror("write");
                goto endconn;
            }
        }
endconn:
        gettimeofday(&after, NULL);
        between.tv_sec=after.tv_sec-before.tv_sec;
        between.tv_usec=after.tv_usec-before.tv_usec;
        while (between.tv_usec<0) {
            between.tv_usec+=1000000;
            between.tv_sec--;
        }
	if (between.tv_sec==0 && between.tv_usec==0) {
		between.tv_sec=1;
	}
        printf ("sent %ld bytes in %ld.%06ld seconds to %d.%d.%d.%d, %ld MByte/s\n", 
                sent, between.tv_sec,between.tv_usec,
                ((unsigned char *)(&client.sin_addr.s_addr))[0],
                ((unsigned char *)(&client.sin_addr.s_addr))[1], 
                ((unsigned char *)(&client.sin_addr.s_addr))[2],
                ((unsigned char *)(&client.sin_addr.s_addr))[3],
                sent/(between.tv_sec*1000000+between.tv_usec)
                );
        shutdown(newsock, SHUT_RDWR);
    }
    
    return (EXIT_SUCCESS);
}

