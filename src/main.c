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
#include "preprocessor/preprocessor.h"
#include "diagnostics/diagnostics.h"
#include "debugger/debugger.h"
#include "codegen/codegen.h"

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

  Debugger debugger = make_debugger("main", debug);


  debugger_print(&debugger, "Debug mode enabled!");
  
  PANIC_IF(optind >= argc, "uso: portugol [-d] <arquivo.por>");

  char *source = read_file(argv[optind]);

  start_builtins();
  
  Debugger lexerDebugger = make_debugger("lexer", debug);
  lexer_T *lexer = init_lexer(source, &lexerDebugger);

  Diagnostic *diag = diagnostic_create(argv[optind], source);

  Debugger parserDebugger = make_debugger("parser", debug);
  parser_T *parser = init_parser(lexer, diag, &parserDebugger);
  
  AST_T*    root   = parser_parse(parser);
  free(source);

  
  Debugger preDebugger = make_debugger("preprocessor", debug);
  preprocessor_T* pre = init_preprocessor(diag, &preDebugger);
  root = preprocessor_run(pre, root);

  Debugger codegenDebugger = make_debugger("codegen", debug);
  codegen_T* cg = init_codegen("build/output.c", &codegenDebugger);
  codegen_emit(cg, root);
  fclose(cg->output);

  codegen_compile("build/output.c", "build/output");

  
  if (debug) {
      ast_print(root);
      printf("\n");
  }

  int executed = system("./build/output");
  if (executed == -1)
    PANIC("Couldn't execute the outputted file");
  
  return 0;
}
