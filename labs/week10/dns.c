// COMP1521 18s2 Week 10 Lab
// dns.c ... simple DNS lookup client

#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

void aliases(struct in_addr *ip, struct hostent *hp);

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
		  aliases(&ip, hp);
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
		 aliases(&ip, hp);
      }
   }

   return 0;
}


void aliases(struct in_addr *ip, struct hostent *hp) {
	int i = 0;
	while (hp->h_aliases[i] != NULL) {
		printf("%s is an alias for %s\n", hp->h_aliases[i], hp->h_name);
		i++;
	}
	i = 0;
	while (hp->h_addr_list[i] != NULL) {
		ip->s_addr = *(u_long*)hp->h_addr_list[i];
		printf("%s has address %s\n", hp->h_name, inet_ntoa(*ip));
		i++;
	}
}
