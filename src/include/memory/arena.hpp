#pragma once

#include <unistd.h>
#include <string.h>

struct Arena
{
 private:
  char *m_mem;
  unsigned int m_size;
  unsigned int m_amount;
  unsigned int m_limit;
  const char *m_head;
  bool m_overwrite;
  
 public:
  ~Arena();
  explicit Arena ( unsigned int , unsigned int p_limit = sysconf ( _SC_PAGESIZE ), bool = true );
  Arena ( const Arena &fast );
  explicit Arena();
  Arena operator= ( const Arena &fast );

  void realloc ( unsigned int );
  void *req ( unsigned int );
  void dell();
  void erase();
  void remove ( void *p_mem );
  unsigned int fquantity();
};