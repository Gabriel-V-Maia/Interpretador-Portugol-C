#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "include/lexer.h"
#include "include/parser.h"
#include "include/AST.H"
#include "helpers/operations.h"
#include "diagnostics/diagnostics.h"

static char* read_file(const char* filepath)
{
    FILE* fp = fopen(filepath, "rb");
    PANIC_IF(fp == NULL, "falha ao abrir '%s': %s", filepath, strerror(errno));

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char* buffer = malloc(size + 1);
    PANIC_IF(buffer == NULL, "falha ao alocar memoria");

    size_t bytes_read = fread(buffer, 1, size, fp);
    buffer[bytes_read] = '\0';
    fclose(fp);
    return buffer;
}

int main(int argc, char *argv[]) {
  int debug = 0;
  int opt;

  while ((opt = getopt(argc, argv, "d")) != -1) {
    switch (opt) {
    case 'd':
      debug = 1;
      break;

    default:
      PANIC("flag desconhecida: -%c\n uso: portugol [-d] <arquivo.por>", optopt);
      break;
    }
  };

  
  PANIC_IF(optind >= argc, "uso: portugol [-d] <arquivo.por>");

  char* source = read_file(argv[optind]);

  lexer_T*  lexer  = init_lexer(source);
  Diagnostic* diag = diagnostic_create(argv[optind], source);
  parser_T* parser = init_parser(lexer, diag);
  AST_T*    root   = parser_parse(parser);

  free(source);
  ast_print(root);
  printf("\n");

  return 0;
}
