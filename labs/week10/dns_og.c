// COMP1521 18s2 Week 10 Lab
// dns.c ... simple DNS lookup client

#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


int main(int argc, char **argv)
{
   struct in_addr ip;   // see "man 3 inet_aton"
   struct hostent *hp;  // see "man 3 gethostbyname"

   if (argc < 2) {
      printf("Usage: %s HostName|IPaddr\n", argv[0]);
      return 1;
   }

   if (inet_aton(argv[1], &ip)) {
   	  // use gethostbyaddr() to determine its hostname
       hp = gethostbyaddr(&ip, strlen(argv[1]), AF_INET);  
	   if (hp == NULL) {
		  printf("No name associated with %s\n", argv[1]);
	   }
	   else {	
		  printf("%s -> %s\n", inet_ntoa(ip), hp->h_name);
	   }
   }
   else {  // might be a hostname
      // use gethostbyname() to determine its IP address
      hp = gethostbyname(argv[1]);
      if (hp == NULL) {
		  // print error message
		  printf("Can't resolve %s\n", argv[1]);
      }
      else {
      	 ip.s_addr = *(u_long*)hp->h_addr;
         printf("%s -> %s\n", hp->h_name, inet_ntoa(ip));
      }
   }

   return 0;
}
