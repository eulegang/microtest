#ifndef LIST_SYMS_H
#define LIST_SYMS_H

#ifdef linux
#include <elf.h>
#if defined(__LP64__)
#define ElfW(type) Elf64_ ## type
#else
#define ElfW(type) Elf32_ ## type
#endif
#endif

typedef struct symbol_dict_t {
  FILE *file;
  char **buffer;
  int cur;
#ifdef linux
  char *strs;
#endif
} symbol_dict_t;

symbol_dict_t *mk_symbol_dict(char *filename);
int next_page(symbol_dict_t*);
void free_symbol_dict(symbol_dict_t*);

#endif
