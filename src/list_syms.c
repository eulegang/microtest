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

uint8_t *read_section_at(FILE *, ElfW(Ehdr), int);
uint8_t *read_section(FILE *, ElfW(Shdr));

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

  for (int i = 0; i < header.e_shnum; i++) {
    ElfW(Off) off = header.e_shoff + (i * header.e_shentsize);

    res = fseek(file, off, SEEK_SET);
    check(!res, "failed to seek to section");

    ElfW(Shdr) sect;
    res = fread(&sect, sizeof(sect), 1, file);
    check(res == 1, "failed to read section");

    if (sect.sh_type == SHT_DYNSYM) {
      char *link_buffer = (char*) read_section_at(file, header, sect.sh_link);
      ElfW(Sym) *symbols = (ElfW(Sym)*) read_section(file, sect);

      for (int i = 0, size = 0; size < (int) sect.sh_size; i++, size += sect.sh_entsize) {
        ElfW(Sym) s = symbols[i];
        uint8_t type = ELF64_ST_TYPE(s.st_info);
        uint8_t bind = ELF64_ST_BIND(s.st_info);

        if (bind != STB_GLOBAL && type != STT_FUNC) {
          continue;
        }

        debug("type: %i, bind: %i", type, bind);
        debug("found symbol: %s", link_buffer + s.st_name);
      }

      free(symbols);
      free(link_buffer);
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

uint8_t *read_section_at(FILE* file, ElfW(Ehdr) header, int i) {
  int res, size;
  ElfW(Off) offset = header.e_shoff + (i * header.e_shentsize);
  res = fseek(file, offset, SEEK_SET);
  check(!res, "failed to seek to section header");


  ElfW(Shdr) section_header;
  size = fread(&section_header, sizeof(section_header), 1, file);
  check(size == 1, "failed to read section header");


  uint8_t *buffer = malloc(section_header.sh_size);
  check_mem(buffer);

  res = fseek(file, section_header.sh_offset, SEEK_SET);
  check(!res, "failed to seek to section content");

  size = fread(buffer, sizeof(uint8_t), section_header.sh_size, file);
  check(size == (int)section_header.sh_size, "failed to read section content");

  return buffer;
error:
  return NULL;
}

uint8_t *read_section(FILE* file, ElfW(Shdr) header) {
  int res, size;
  uint8_t *buffer = malloc(header.sh_size);
  check_mem(buffer);

  res = fseek(file, header.sh_offset, SEEK_SET);
  check(!res, "failed to seek to section content");

  size = fread(buffer, sizeof(uint8_t), header.sh_size, file);
  check(size == (int)header.sh_size, "failed to read section content");

  return buffer;
error:
  return NULL;
}
