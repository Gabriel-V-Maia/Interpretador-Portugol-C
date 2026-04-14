# Interpretador-Portugol

<p align="center">
  <img src="https://img.shields.io/badge/linguagem-C-blue?style=for-the-badge&logo=c">
  <img src="https://img.shields.io/badge/status-em%20desenvolvimento-yellow?style=for-the-badge">
  <img src="https://img.shields.io/badge/licença-MIT-green?style=for-the-badge">
  <img src="https://img.shields.io/badge/plataforma-Linux%20%7C%20Windows-lightgrey?style=for-the-badge">
</p>

<p align="center">
  Compilador de <strong>Portugol</strong> escrito em <strong>C</strong>, transpilando para C e compilando para um executável nativo.
  <br>
  Sintaxe baseada no <a href="https://github.com/dgadelha/Portugol-Webstudio">Portugol-Webstudio</a>.
</p>

---

## Pipeline

```mermaid
graph TD
    A[main .por file] --> B[Lexer]
    B --> C[Parser]
    C --> D{importar found?}
    D -- yes --> E[imported .por file]
    E --> F[Lexer]
    F --> G[Parser]
    G --> H[imported AST]
    H --> I[inject into main AST]
    I --> D
    D -- no --> J[merged AST]
    J --> K[Codegen]
    K --> L[.c file]
    L --> M[GCC]
    M --> N[.exe]
    style A fill:#4a90d9,color:#fff
    style E fill:#4a90d9,color:#fff
    style J fill:#27ae60,color:#fff
    style L fill:#e67e22,color:#fff
    style N fill:#c0392b,color:#fff
```

---

## Exemplo

```portugol
programa {
  importar "std/net.por"

  funcao inicio() {
    cadeia nome = "Maria Silva"
    inteiro idade = 28
    logico estudante = verdadeiro

    se (estudante e idade < 30) {
      escreva("jovem estudante")
    } senao {
      escreva("nao e estudante jovem")
    }

    para i = 0 ate 5 {
      escreva(i)
    }
  }
}
```

---

## Status

| Componente | Status |
|---|---|
| Lexer | Concluído |
| Parser | Em andamento |
| Preprocessor (`importar`) | Pendente |
| Codegen (transpile para C) | Pendente |
| Visitor | Pendente |
| Definição de funções | Em andamento |
| Chamada de funções | Pendente |
| Argumentos em funções | Em andamento |
| Leitura de arquivos | Concluído |
| Diagnósticos de erro | Concluído |

---

## Uso

```bash
# compilar o projeto
make

# rodar normalmente
./build/portugol arquivo.por

# rodar com debug (imprime a AST e logs internos)
./build/portugol -d arquivo.por
```

---

## Estrutura do Projeto

```
.
├── src/
│   ├── include/        # headers
│   ├── diagnostics/    # erros com linha e coluna
│   ├── debugger/       # logs de debug
│   ├── lexer.c
│   ├── parser.c
│   ├── AST.c
│   └── main.c
├── examples/           # arquivos .por de exemplo
├── Makefile
└── README.md
```

---

## Contribuições

Abra uma _issue_ ou envie um _pull request_. Qualquer contribuição é bem-vinda.

---

## Licença

[MIT](LICENSE) — Gabriel Vinícius da Maia.
