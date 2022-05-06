#pragma once

#include <unistd.h>
#include <string.h>

struct Arena
{
 private:
  char *m_mem;
  unsigned int m_size;
  unsigned int m_amount;
  const char *m_ptr;
  bool m_overwrite;

 public:
  ~Arena();
  Arena ( unsigned int = sysconf ( _SC_PAGESIZE ), bool = true );
  Arena ( const Arena &fast );
  Arena operator= ( const Arena &fast );

  void mmem ( unsigned int );
  void *req ( unsigned int );
  void dell();
  //void remove ( void *p_mem );
  unsigned int afree();

};
