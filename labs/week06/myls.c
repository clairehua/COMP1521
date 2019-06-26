// myls.c ... my very own "ls" implementation

#include <stdlib.h>
#include <stdio.h>
#include <bsd/string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>

#define MAXDIRNAME 100
#define MAXFNAME   200
#define MAXNAME    20
#define MAXPATH    200


char *rwxmode(mode_t, char *);
char *username(uid_t, char *);
char *groupname(gid_t, char *);

int main(int argc, char *argv[])
{
   // string buffers for various names
   char dirname[MAXDIRNAME];
   char uname[MAXNAME+1]; 
   char gname[MAXNAME+1]; 
   char mode[MAXNAME+1]; 

   // collect the directory name, with "." as default
   if (argc < 2)
      strlcpy(dirname, ".", MAXDIRNAME);
   else
      strlcpy(dirname, argv[1], MAXDIRNAME);

   // check that the name really is a directory
   struct stat info;
   if (stat(dirname, &info) < 0)
      { perror(argv[0]); exit(EXIT_FAILURE); }
   if ((info.st_mode & S_IFMT) != S_IFDIR)
      { fprintf(stderr, "%s: Not a directory\n",argv[0]); exit(EXIT_FAILURE); }

   // open the directory to start reading
   DIR *df = opendir(dirname);
   assert(df != NULL);
   
   // read directory entries
   struct dirent *entry; 
   while ((entry = readdir(df)) != NULL) {
	    // ignore the object if its name starts with '.'
   		if (entry->d_name[0] != '.') {
		   // get the struct stat info for the object (using its path)
			struct stat buf;
			char path[MAXPATH];
			strcpy(path, dirname);
			strcat(path, "/");
			strcat(path, entry->d_name);
			
			stat(path, &buf);
		   // print the details using the object name and struct stat info
		   printf("%s  %-8.8s %-8.8s %8lld  %s\n",
		   			rwxmode(buf.st_mode, mode),
		   			username(buf.st_uid, uname),
		   			groupname(buf.st_gid, gname),
		   			(long long)buf.st_size,
		   			entry->d_name);
   		}
   }

   // finish up
   closedir(df); 
   return EXIT_SUCCESS;
}

// convert octal mode to -rwxrwxrwx string
char *rwxmode(mode_t mode, char *str)
{
	int i = 0;
	// object type
	// regular file
	if ((mode & S_IFMT) == S_IFREG) {
		str[i] = '-';
	}
	// symbolic link
	else if ((mode & S_IFMT) == S_IFLNK) {
		str[i] = 'l';
	}
	// directory
	else if ((mode & S_IFMT) == S_IFDIR) {
		str[i] = 'd';
	}
	// other object types
	else {
		str[i] = '?';
	}

	i++;
	// permissions for owner, group and others in the same rwx-r-x--x-
	
	// owner
	if ((mode & S_IRUSR) == S_IRUSR) {
		str[i] = 'r';
	}
	else {
		str[i] = '-';
	}
	i++;
	if ((mode & S_IWUSR) == S_IWUSR) {
		str[i] = 'w';
	}
	else {
		str[i] = '-';
	}
	i++;
	if ((mode & S_IXUSR) == S_IXUSR) {
		str[i] = 'x';
	}
	else {
		str[i] = '-';
	}
	i++;
		
	// group
	if ((mode & S_IRGRP) == S_IRGRP) {
		str[i] = 'r';
	}
	else {
		str[i] = '-';
	}
	i++;
	if ((mode & S_IWGRP) == S_IWGRP) {
		str[i] = 'w';
	}
	else {
		str[i] = '-';
	}
	i++;
	if ((mode & S_IXGRP) == S_IXGRP) {
		str[i] = 'x';
	}
	else {
		str[i] = '-';
	}
	i++;
		
	// others
	if ((mode & S_IROTH) == S_IROTH) {
		str[i] = 'r';
	}
	else {
		str[i] = '-';
	}
	i++;
	if ((mode & S_IWOTH) == S_IWOTH) {
		str[i] = 'w';
	}
	else {
		str[i] = '-';
	}
	i++;
	if ((mode & S_IXOTH) == S_IXOTH) {
		str[i] = 'x';
	}
	else {
		str[i] = '-';
	}
	i++;
	
	str[i] = '\0';

   return str;

}

// convert user id to user name
char *username(uid_t uid, char *name)
{
   struct passwd *uinfo = getpwuid(uid);
   if (uinfo == NULL)
      snprintf(name, MAXNAME, "%d?", (int)uid);
   else
      snprintf(name, MAXNAME, "%s", uinfo->pw_name);
   return name;
}

// convert group id to group name
char *groupname(gid_t gid, char *name)
{
   struct group *ginfo = getgrgid(gid);
   if (ginfo == NULL)
      snprintf(name, MAXNAME, "%d?", (int)gid);
   else
      snprintf(name, MAXNAME, "%s", ginfo->gr_name);
   return name;
}
