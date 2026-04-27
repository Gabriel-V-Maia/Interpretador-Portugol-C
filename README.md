# Interpretador Portugol

Projeto focado no desenvolvimento de um interpretador e compilador para a linguagem Portugol, escrito inteiramente em C. O sistema realiza o processamento completo do código, desde a análise inicial até a geração de uma Árvore de Sintaxe Abstrata (AST) com suporte para transpilação para C.

A sintaxe é compatível com o padrão Portugol-Webstudio.

## Funcionamento do Sistema

O processamento do código segue um fluxo linear dividido em camadas de responsabilidade:

1. Entrada: Leitura do arquivo fonte .por para a memória.
2. Análise Léxica: Conversão do texto em tokens (identificadores, números, strings).
3. Análise Sintática: Construção da AST e validação da estrutura gramatical.
4. Pré-processamento: Resolução de diretivas de importação e injeção de dependências.
5. Análise Semântica: Verificação de tipos e escopos de variáveis.
6. Geração de Código: Conversão da AST final para código C equivalente.
7. Saída: Compilação via GCC para geração do executável nativo.

## Exemplos de Código

### Sequência de Fibonacci
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

### Cálculo de Fatorial
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

## Progresso do Desenvolvimento

| Componente | Situação |
| :--- | :--- |
| Lexer | Concluído |
| Parser | Em andamento |
| Estrutura AST | Concluído |
| Pré-processador | Em desenvolvimento |
| Geração de Código C | Pendente |
| Diagnósticos de Erro | Concluído |
| Debugger Interno | Concluído |

## Instruções de Uso

Para compilar o projeto e executar os exemplos:

```bash
# Compilação do interpretador
make

# Execução de um script Portugol
./build/portugol examples/fibbonaci.por

# Execução com visualização de logs e AST
./build/portugol -d examples/fatorial.por
```

## Organização de Diretórios

* build: Arquivos binários e resultados de compilação.
* examples: Scripts de teste em linguagem Portugol.
* libs: Bibliotecas padrão e módulos externos.
* src: Código fonte organizado por módulos (Lexer, Parser, Codegen).
* src/include: Definições de cabeçalhos e interfaces.

## Metas de Implementação

* Finalização do Parser para suporte total à gramática.
* Estabilização do sistema de importação de arquivos.
* Integração automática com o GCC para compilação final.
* Criação de biblioteca padrão para entrada e saída de dados.
* Implementação de suite de testes automatizados para a AST.

## Licença

Este software é distribuído sob a Licença MIT. Desenvolvido por Gabriel Vinícius da Maia.
```
