#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include "../include/token.h"

typedef enum {
    DIAG_ERROR,
    DIAG_WARNING,
    DIAG_NOTE,
} DiagnosticLevel;

typedef struct {
    const char* filepath;
    const char* source;
} Diagnostic;

Diagnostic* diagnostic_create(const char* filepath, const char* source);
void        diagnostic_free(Diagnostic* diag);

void diagnostic_emit(Diagnostic* diag, DiagnosticLevel level, token_T* token, const char* fmt, ...);

#define diagnostic_error(diag, token, fmt, ...)   diagnostic_emit(diag, DIAG_ERROR,   token, fmt, ##__VA_ARGS__)
#define diagnostic_warning(diag, token, fmt, ...) diagnostic_emit(diag, DIAG_WARNING, token, fmt, ##__VA_ARGS__)
#define diagnostic_note(diag, token, fmt, ...)    diagnostic_emit(diag, DIAG_NOTE,    token, fmt, ##__VA_ARGS__)

#endif
