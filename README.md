# Interpretador-Portugol

<p align="center">
  <img src="https://img.shields.io/badge/linguagem-C-blue?style=for-the-badge&logo=c">
  <img src="https://img.shields.io/badge/status-em%20desenvolvimento-yellow?style=for-the-badge">
  <img src="https://img.shields.io/badge/licença-MIT-green?style=for-the-badge">
  <img src="https://img.shields.io/badge/plataforma-Linux%20%7C%20Windows-lightgrey?style=for-the-badge">
</p>

<p align="center">
  Interpretador/compilador de <strong>Portugol</strong> escrito em <strong>C</strong>.
  <br>
  Atualmente realiza análise léxica e sintática, com geração de AST e base para transpilar para C.
  <br>
  Sintaxe inspirada no <a href="https://github.com/dgadelha/Portugol-Webstudio">Portugol-Webstudio</a>.
</p>

---

## Pipeline

```mermaid
flowchart LR
    A[arquivo.por] --> B[Lexer]
    B --> C[Parser]
    C --> D[AST]

    D --> E{importar?}
    E -->|sim| F[Preprocessor]
    E -->|nao| G[AST final]

    F --> G
    G --> H[Codegen]
    H --> I[output.c]
    I --> J[GCC]
    J --> K[executavel]
```

---

## Exemplos

### Fibonacci

```portugol
programa {
  inteiro funcao fib(inteiro n) {
    se (n <= 1) {
      retorne n
    }
    retorne fib(n - 1) + fib(n - 2)
  }

  nulo funcao inicio() {
    escreva("${fib(10)}\n")
  }
}
```

### Fatorial

```portugol
programa {
  inteiro funcao fat(inteiro n) {
    se (n == 0) {
      retorne 1
    }
    retorne n * fat(n - 1)
  }

  nulo funcao inicio() {
    escreva("${fat(5)}\n")
  }
}
```

Arquivos disponíveis em `examples/`:

* `fibbonaci.por`
* `fatorial.por`

---

## Status

| Componente                | Status                        |
| ------------------------- | ----------------------------- |
| Lexer                     | Concluído                     |
| Parser                    | Em andamento                  |
| AST                       | Concluído (estrutura base)    |
| Preprocessor (`importar`) | Em desenvolvimento            |
| Codegen (C)               | Pendente                      |
| Execução direta           | Pendente                      |
| Funções (declaração)      | Em andamento                  |
| Funções (chamada)         | Parcial                       |
| Argumentos                | Em andamento                  |
| Recursão                  | Parcial (estrutura suportada) |
| Diagnósticos de erro      | Concluído                     |
| Debugger                  | Concluído                     |

---

## Uso

```bash
# compilar
make

# executar arquivo
./build/portugol examples/fibbonaci.por

# modo debug (AST + logs)
./build/portugol -d examples/fatorial.por
```

---

## Estrutura do Projeto

```
.
├── build/              # binários gerados
├── examples/           # exemplos em Portugol
├── libs/               # futuras bibliotecas padrão
├── src/
│   ├── include/        # headers
│   ├── diagnostics/    # erros e mensagens
│   ├── debugger/       # logs internos
│   ├── helpers/        # utilitários
│   ├── preprocessor/   # sistema de importação
│   ├── codegen/        # geração de C
│   ├── AST.c
│   ├── lexer.c
│   ├── parser.c
│   ├── token.c
│   └── main.c
├── Makefile
└── README.md
```

---

## Roadmap

* [ ] Finalizar parser
* [ ] Implementar `importar`
* [ ] Gerar código C completo
* [ ] Compilação automática com GCC
* [ ] Biblioteca padrão (`std`)
* [ ] Melhorar mensagens de erro
* [ ] Testes automatizados

---

## Contribuições

Sinta-se à vontade para abrir *issues* ou enviar *pull requests*.
Sugestões, melhorias e correções são bem-vindas.

---

## Licença

[MIT](LICENSE) — Gabriel Vinícius da Maia.

```
