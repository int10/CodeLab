//#include "chello.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
int cprint();
 
int
main ()
{
  char *error;
  void *handle = dlopen ("./libhello.so", RTLD_NOW);
  if (!handle)
    {
      fprintf (stderr, "%s\n", dlerror ());
      exit (1);
    }
 
  int (*dlprint)() ;
  dlprint = dlsym (handle, "cprint");
  if ((error = dlerror ()) != NULL)
    {
      fprintf (stderr, "%s\n", error);
      exit (1);
    }
 
  (*dlprint)();
 
  dlclose (handle);
 
 
  return 0;
}