#include <stdio.h>
#include <stdlib.h>
#include <dbg.h>
#include <elf.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "list_syms.h"

#define BUFFER_SIZE 4096

#ifndef linux
// Move
#error "Needs to be compiled on linux"
#endif

#include <elf.h>
#if defined(__LP64__)
#define ElfW(type) Elf64_ ## type
#else
#define ElfW(type) Elf32_ ## type
#endif

typedef struct bank_index {
  int bank;
  int offset;
} bank_index;

typedef struct symbol_searcher_t {
  int fd;
  int bank_len;
  char **bank;
  int idx_len;
  bank_index* idx;
  int cur;
} symbol_searcher_t;

uint8_t *read_section(int, ElfW(Shdr));
int read_section_header(int fd, ElfW(Ehdr) header, size_t i, ElfW(Shdr) *section_header);

symbol_searcher mk_symbol_searcher(char * filename) {
  ssize_t size;
  int res;

  int fd = open(filename, O_RDONLY);
  check(fd != -1, "not normal file %s", filename);

  ElfW(Ehdr) header;
  size = read(fd, &header, sizeof(ElfW(Ehdr)));
  check(size == sizeof(ElfW(Ehdr)), "failed to read header");
  check(memcmp(header.e_ident, ELFMAG, SELFMAG) == 0, "not a valid elf file: %s", filename);

  struct symbol_searcher_t *dict = malloc(sizeof(struct symbol_searcher_t));

  dict->fd = fd;
  dict->bank_len = header.e_shnum;
  dict->bank = malloc(header.e_shnum * sizeof(char*));
  dict->idx_len = 0;
  dict->idx = malloc(4096);
  dict->cur = 0;

  memset(dict->bank, 0, header.e_shnum * sizeof(void*));

  ElfW(Shdr) sect_header;
  res = read_section_header(fd, header, header.e_shstrndx, &sect_header);
  check(!res, "failed to read string table");

  char *buffer = (char*)read_section(fd, sect_header);
  check_mem(buffer);


  for (int i = 0; i < header.e_shnum; i++) {
    ElfW(Shdr) sect;

    res = read_section_header(fd, header, i, &sect);
    check(!res, "failed to read section header");

    if (sect.sh_type == SHT_DYNSYM) {
      ElfW(Shdr) link;

      res = read_section_header(fd, header, sect.sh_link, &link);
      check(!res, "reading linked section");

      if (!dict->bank[sect.sh_link]) {
        dict->bank[sect.sh_link] = (char*) read_section(fd, link);
      }

      ElfW(Sym) *symbols = (ElfW(Sym)*) read_section(fd, sect);

      for (int i = 0, size = 0; size < (int) sect.sh_size; i++, size += sect.sh_entsize) {
        ElfW(Sym) s = symbols[i];
        uint8_t type = ELF64_ST_TYPE(s.st_info);
        uint8_t bind = ELF64_ST_BIND(s.st_info);

        if (bind != STB_GLOBAL && type != STT_FUNC) {
          continue;
        }

        bank_index idx = { sect.sh_link, s.st_name };
        dict->idx[dict->idx_len++] = idx;
      }

      free(symbols);
    }
  }

  return (symbol_searcher)dict; 
error:
  return NULL;
}

char* symbol_searcher_next_symbol(symbol_searcher searcher) {
  struct symbol_searcher_t *sym = ((struct symbol_searcher_t *)searcher);

  if (sym->cur >= sym->idx_len) {
    return NULL;
  }

  bank_index idx = sym->idx[sym->cur++];

  return sym->bank[idx.bank] + idx.offset;
}

void free_symbol_searcher(symbol_searcher searcher) {
  struct symbol_searcher_t *sym = ((struct symbol_searcher_t *)searcher);

  for (int i = 0; i < sym->bank_len; i++) {
    if (sym->bank[i]) {
      free(sym->bank[i]);
    }
  }

  free(sym->idx);
  close(sym->fd);
}

uint8_t *read_section(int fd, ElfW(Shdr) header) {
  uint8_t *buffer = malloc(header.sh_size);
  check_mem(buffer);

  ssize_t res = pread(fd, buffer, header.sh_size, header.sh_offset);
  check(res == (int)header.sh_size, "failed to read section content");

  return buffer;
error:
  return NULL;
}

int read_section_header(int fd, ElfW(Ehdr) header, size_t i, ElfW(Shdr) *section_header) {
  ssize_t res = pread(fd, section_header, sizeof(ElfW(Shdr)), header.e_shoff + (i * header.e_shentsize));
  check(res == sizeof(ElfW(Shdr)), "failed to read Section Header");

  return 0;
error:
  return -1;
}
