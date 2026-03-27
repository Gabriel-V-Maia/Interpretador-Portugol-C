#include "diagnostics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define ANSI_RESET   "\033[0m"
#define ANSI_BOLD    "\033[1m"
#define ANSI_RED     "\033[31m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_WHITE   "\033[37m"

Diagnostic* diagnostic_create(const char* filepath, const char* source)
{
    Diagnostic* diag = malloc(sizeof(Diagnostic));
    diag->filepath   = filepath;
    diag->source     = source;
    return diag;
}

void diagnostic_free(Diagnostic* diag)
{
    free(diag);
}

static void extract_line(const char* source, int line, char* out, size_t out_size)
{
    const char* p = source;

    for (int i = 1; i < line; i++) {
        p = strchr(p, '\n');
        if (!p) { out[0] = '\0'; return; }
        p++;
    }

    const char* end = strchr(p, '\n');
    size_t len = end ? (size_t)(end - p) : strlen(p);
    if (len >= out_size) len = out_size - 1;

    memcpy(out, p, len);
    out[len] = '\0';
}

static const char* level_label(DiagnosticLevel level)
{
    switch (level) {
        case DIAG_ERROR:   return ANSI_BOLD ANSI_RED    "erro"      ANSI_RESET;
        case DIAG_WARNING: return ANSI_BOLD ANSI_YELLOW "aviso"     ANSI_RESET;
        case DIAG_NOTE:    return ANSI_BOLD ANSI_CYAN   "nota"      ANSI_RESET;
        default:           return "desconhecido";
    }
}

static const char* level_color(DiagnosticLevel level)
{
    switch (level) {
        case DIAG_ERROR:   return ANSI_RED;
        case DIAG_WARNING: return ANSI_YELLOW;
        case DIAG_NOTE:    return ANSI_CYAN;
        default:           return ANSI_WHITE;
    }
}

void diagnostic_emit(Diagnostic* diag, DiagnosticLevel level,
                     token_T* token, const char* fmt, ...)
{
    char message[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    fprintf(stderr, "%s: %s%s%s\n",
            level_label(level),
            ANSI_BOLD, message, ANSI_RESET);

    if (token) {
        fprintf(stderr, "  " ANSI_CYAN "-->" ANSI_RESET " %s:%d:%d\n",
                diag->filepath, token->line, token->column);

        char source_line[512];
        extract_line(diag->source, token->line, source_line, sizeof(source_line));

        fprintf(stderr, "   %s|%s\n", ANSI_CYAN, ANSI_RESET);
        fprintf(stderr, "%s%3d%s %s|%s %s\n",
                ANSI_CYAN, token->line, ANSI_RESET,
                ANSI_CYAN, ANSI_RESET,
                source_line);
        fprintf(stderr, "    %s|%s ", ANSI_CYAN, ANSI_RESET);

        for (int i = 1; i < token->column; i++)
            fprintf(stderr, " ");

        fprintf(stderr, "%s^%s\n", level_color(level), ANSI_RESET);
        fprintf(stderr, "   %s|%s\n\n", ANSI_CYAN, ANSI_RESET);
    }

    if (level == DIAG_ERROR)
        exit(1);
}
