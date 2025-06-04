
#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "mvisit.h"

class Plugin {
  
 public:

  const char * name;
  const char * description;

  virtual void printPrologue(PARAMETRI * parametri);

};

#endif
