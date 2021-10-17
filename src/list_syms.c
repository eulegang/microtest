#include <stdio.h>
#include <stdlib.h>
#include <dbg.h>
#include <elf.h>

#include "list_syms.h"

#define BUFFER_SIZE 4096

#ifndef linux
// Move
#error "Needs to be compiled on linux"
#endif

symbol_dict_t *mk_symbol_dict(char * filename) {
  FILE *file = fopen(filename, "rb");
  check(file, "not normal file %s", filename);

  ElfW(Ehdr) header;

  int res = fread(&header, sizeof(header), 1, file);
  check(res == 1, "failed to read elf header");
  check(memcmp(header.e_ident, ELFMAG, SELFMAG) == 0, "not a valid elf file: %s", filename);

  struct symbol_dict_t *dict = malloc(sizeof(struct symbol_dict_t));

  dict->file = file;
  dict->buffer = calloc(sizeof(char*), BUFFER_SIZE);
  dict->cur = 0;

  ElfW(Off) strsh = header.e_shoff + (header.e_shstrndx * header.e_shentsize);

  res = fseek(file, strsh, SEEK_SET);
  check(!res, "failed to seek to str section header");

  ElfW(Shdr) sect_header;
  res = fread(&sect_header, sizeof(sect_header), 1, file);
  check(res == 1, "failed to read in section header");

  ElfW(Off) str_off = sect_header.sh_offset;
  ElfW(Xword) str_size = sect_header.sh_size;

  res = fseek(file, str_off, SEEK_SET);
  check(!res, "failed to seek to str buffer");

  char *buffer = malloc(str_size);
  check_mem(buffer);

  res = fread(buffer, 1, str_size, file);
  check((ElfW(Xword))res == str_size, "failed to read section header buffer");

  dict->strs = buffer;


  debug("size: %i", str_size);

  for (int i = 0; i < header.e_shnum; i++) {
    ElfW(Off) off = header.e_shoff + (i * header.e_shentsize);

    res = fseek(file, off, SEEK_SET);
    check(!res, "failed to seek to section");

    ElfW(Shdr) sect;
    res = fread(&sect, sizeof(sect), 1, file);
    check(res == 1, "failed to read section");

    if (sect.sh_type == SHT_DYNSYM) {
      debug("found section: %s", buffer + sect.sh_name);
      debug("offset %i, size: %i, entsize: %i", sect.sh_offset, sect.sh_size, sect.sh_entsize);

      res = fseek(file, sect.sh_offset, SEEK_SET);
      check(!res, "failed to seek to section");

      ElfW(Sym) *sect_buffer = malloc(sect.sh_size);

      res = fread(sect_buffer, 1, sect.sh_size, file);
      check(res == (int)sect.sh_size, "failed to read in dynsym");

      for (int i = 0; (int)sect.sh_entsize * i < (int)sect.sh_size; i++) {
        ElfW(Sym) s = sect_buffer[i];

        debug("found symbol: %s", buffer + s.st_name);
      }


      free(sect_buffer);
    }
  }

  return dict; 
error:
  return NULL;
}

int next_page(symbol_dict_t *dict) {
  (void) dict;
  
  return 0;
}

void free_symbol_dict(symbol_dict_t *dict) {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (dict->buffer[i]) {
      free(dict->buffer[i]);
    } else {
      break;
    }
  }

  free(dict->buffer);
  fclose(dict->file);
}

