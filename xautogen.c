/*
#
# melbo @ https://x-plane.org
#
# 
# compile:   cl xautogen.c /D "NODEBUG" /O2 
# 
*/

#define VERSION "0.3"

#ifdef _WIN32
  #include <windows.h>
  #include <process.h>
  #include <shlobj.h>
  #include "dirent.h"
#else
  #include <libgen.h>
  #include <unistd.h>
  #include <dirent.h>
#endif

#define _CRT_INTERNAL_NONSTDC_NAMES 1
#include <sys/stat.h>
#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
  #define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
  #define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define MAX_TXT          1024
#define MAX_WRD          256

#define XSCENERYDIR  "Custom Scenery"
#define XTILESDIR    XSCENERYDIR"/Xautogen"
#define DEFAUTOGEN   "Resources/default scenery/1000 autogen"

int debugLevel = 0;
int forceLinks = 0;
char XTEST[MAX_TXT]  = "";
char words[MAX_WRD][MAX_TXT];

/*-----------------------------------------------------------------*/

int strip(char *s) {        /* remove trailing whitespace */
  int i = strlen(s);
  while (i >= 0 && s[i] <= ' ') {
    s[i--] = '\0';
  }
  return(0);
}

/*-----------------------------------------------------------------*/

int join(char *s, int n) {
  int i = 0;

  s[0]='\0';
  while (i<n) {
    strcat(s,words[i++]);  
    strcat(s," ");  
  }
  return(0);
}

/*-----------------------------------------------------------------*/

int split(char *s) {
  int w = 0;
  int i = 0;
 
  char tmp[MAX_TXT];
  char *k;

  k = tmp;

  strcpy(tmp,s);

  while(tmp[i] > '\0' && w < MAX_WRD ) {
    if (tmp[i] <= ' ' ) {
      tmp[i++] = '\0';

      strcpy(words[w++], k);
      while(tmp[i] > '\0' && tmp[i] <= ' ' )
        i++;
      k = &tmp[i];
    } else {
      i++;
    }
  }
 
  strcpy(words[w++], k);
  return(w);
}
/*-----------------------------------------------------------------*/

int shift(char *s) {        /* shift string to the right */
  int i = strlen(s);
  while (i >= 0) {
    s[i+1] = s[i];
    i--;
  }
  return(0);
}

/*-----------------------------------------------------------------*/

int isDir(char *s) {
  struct stat sb;

  if (stat(s, &sb) == 0 && S_ISDIR(sb.st_mode)) {
    return(1); 
  } else {
    return(0); 
  }
}

/*-----------------------------------------------------------------*/

int isFile(char *s) {
  struct stat sb;

  if (stat(s, &sb) == 0 && S_ISREG(sb.st_mode)) {
    return(1); 
  } else {
    return(0); 
  }
}

/*-----------------------------------------------------------------*/

int copyFileMod(char *fromFile, char *toFile) {

  FILE   *in,*out;
  char   buf[MAX_TXT];
 
  in = fopen( fromFile, "r" );
  out= fopen( toFile, "w" );

  while ( fgets(buf, MAX_TXT, in) ) {
    fputs(buf, out);
    if ( strstr(buf, "AG_STRING") || strstr(buf, "AG_BLOCK") ) { 
      fputs("HIDE_TILES\n",out);
    }
  }

  fclose(in);
  fclose(out);
  return 0;

}

/*-----------------------------------------------------------------*/

int linkFile(char *fromFile, char *toFile) {
  char *m = toFile;
  int n=0;
  char fromFixed[MAX_TXT];

  // calculate relative path shift
  fromFixed[0] = '\0';
  while(*m > '\0') {           // count slashed
    if ( *m++ == '/' ) n++;
  }
  while(n>0) {
    strcat(fromFixed,"../");   // prepend "n" times "../"
    n--;
  }
  sprintf(fromFixed,"%s%s",fromFixed,fromFile);
 
  if ( ! isFile(toFile) ) {

#ifdef _WIN32
    char tmp[MAX_TXT];
  
    // fix seperators
    m = fromFixed;
    while(*m > '\0') {
      if ( *m == '/' ) *m = '\\';
	  m++;
    }
    m = toFile;
    while(*m > '\0') {
      if ( *m == '/' ) *m = '\\';
	  m++;
    }
  
    if ( isDir(fromFixed) ) {
      sprintf(tmp,"mklink /D \"%s\" \"%s\"",toFile,fromFixed);  // directory symlink
    } else {
      sprintf(tmp,"mklink \"%s\" \"%s\"",toFile,fromFixed);     // file symlink
    }
    if ( debugLevel>1 ) printf("# linkFile(W) %s\n",tmp);
    system(tmp);
#else
 
    if ( debugLevel>1 ) printf("# linkFile(U) %s %s\n",fromFixed,toFile);  
    symlink(fromFixed,toFile);
#endif

  } else {
    if ( debugLevel>1 ) printf("# linkFile skipping %s\n",toFile);  
  }
  return(0);
}

/*-----------------------------------------------------------------*/

int copyFile(char *fromFile, char *toFile) {

  FILE   *in,*out;
  char   buff[BUFSIZ];
  size_t n;
 
  in = fopen( fromFile, "rb" );
  out= fopen( toFile, "wb" );
  while ( (n=fread(buff,1,BUFSIZ,in)) != 0 ) {
    fwrite( buff, 1, n, out );
  }
  fclose(in);
  fclose(out);
  return(0);
}

/*-----------------------------------------------------------------*/

int copyTree(char *fromDir, char *toDir) {

  FILE *in,*out;
  char buf[MAX_TXT];
  char s1[MAX_TXT];
  char s2[MAX_TXT];

  DIR *d;
  struct dirent *dir;

  if ( ! isDir(toDir) ) {
    if ( mkdir(toDir,0755) ) {
      printf("ERROR: cannot create %s\n",toDir);
      return(-1);
    } else {
      printf("%s created\n",toDir);
    }
  } else {
    if ( debugLevel ) printf("%s exists\n",toDir);
  }

  d = opendir(fromDir);
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if ( dir->d_name[0] != '.' ) {
        sprintf(s1,"%s/%s",fromDir,dir->d_name);
        if ( ! strcmp(toDir,".") ) {
          sprintf(s2,"%s",dir->d_name);
        } else {
          sprintf(s2,"%s/%s",toDir,dir->d_name);
        }
        DIR *isDir = opendir(s1);
        if ( isDir ) {
          if ( debugLevel ) printf("call copyTree %s %s\n",s1,s2);
          copyTree(s1,s2);
        } else {
          if ( strstr(dir->d_name,".ags") ||  strstr(dir->d_name,".agb")  ) {
            if ( debugLevel>1 ) printf("call copyFileMod %s %s\n",s1,s2);
            copyFileMod(s1,s2);
          } else {
            if ( debugLevel>1 ) printf("call linkFile %s %s\n",s1,s2);
            if ( forceLinks ) unlink(s2);   // cleanup old links or files
            linkFile(s1,s2);
          }
        }
      }
    }
  }
  return(0);
}

/*-----------------------------------------------------------------*/


int main(int argc, char **argv) {

  char tmp[256];
  int i,n;

  printf("Xautogen - %s - melbo @x-plane.org\n",VERSION);

  /* parsing command line parameters */
  i = 1;
  while ( i < argc ) {
    if ( ! strcmp(argv[i],"-d") ) {
      debugLevel++;
    } else if ( ! strcmp(argv[i],"-f") ) {
      forceLinks = 1;
    } else if ( ! strcmp(argv[i],"-h") ) {
      /* show help */
      printf("\n  usage: %s [-d] [-f] [-h]\n\n",argv[0]);
      printf("  -d  increase debug level\n"
             "  -f  force link creation\n"
             "  -h  this help\n\n");
      return(0);
    }
    i++;
  }

#ifndef _WIN32
  strcpy(tmp,dirname(argv[0]));
  printf("changing to %s\n",tmp);
  chdir(tmp);
#endif

  if ( ! isDir(XTILESDIR) ) {
    if ( mkdir(XTILESDIR,0755) ) {
      printf("ERROR: cannot create %s\n",XTILESDIR);
      return(-1);
    } else {
      printf("%s created\n",XTILESDIR);
    }
  } else {
    if ( debugLevel) printf("%s exists\n",XTILESDIR);
  }
  chdir(XTILESDIR);

  copyTree("../../"DEFAUTOGEN,".");
  copyTree("../../"DEFAUTOGEN,".");

}

