programa {
  funcao inicio() {
    cadeia nome = "Maria Silva"
    real altura = 1.75
    inteiro idade = 28
    logico estudante = verdadeiro

    escreva("=== Variaveis ===")
    escreva(nome)
    escreva(altura)
    escreva(idade)
    escreva(estudante)

    escreva("=== Se/Senao ===")
    se (idade >= 18) {
      escreva("maior de idade")
    } senao {
      escreva("menor de idade")
    }

    escreva("=== Se com logico ===")
    se (estudante e idade < 30) {
      escreva("jovem estudante")
    }

    escreva("=== Se com nao ===")
    se (nao estudante) {
      escreva("nao e estudante")
    } senao {
      escreva("e estudante")
    }

    escreva("=== Enquanto ===")
    inteiro i = 0
    enquanto (i < 5) {
      escreva("enquanto i =")
      escreva(i)
      i = i + 1
    }

    escreva("=== Para ===")
    para j = 0 ate 5 {
      escreva("para j =")
      escreva(j)
    }

    escreva("=== Repita ===")
    inteiro x = 0
    repita {
      escreva("repita x =")
      escreva(x)
      x = x + 1
    } ate (x == 4)

    escreva("=== Operacoes aritmeticas ===")
    real a = 10.0
    real b = 3.0
    escreva(a + b)
    escreva(a - b)
    escreva(a * b)
    escreva(a / b)

    escreva("=== Comparacoes ===")
    se (a > b) { escreva("a maior que b") }
    se (a != b) { escreva("a diferente de b") }
    se (b < a) { escreva("b menor que a") }

    escreva("=== Funcoes ===")
    cumprimento()
    matematica()

    escreva("=== Retorne ===")
    retorneValor()
  }

  funcao cumprimento() {
    cadeia msg = "ola da funcao cumprimento!"
    escreva(msg)
  }

  funcao matematica() {
    inteiro resultado = 0
    para k = 1 ate 5 {
      resultado = resultado + k
    }
    escreva("soma de 1 a 5 =")
    escreva(resultado)
  }

  funcao retorneValor() {
    inteiro n = 0
    enquanto (n < 10) {
      n = n + 1
      se (n == 5) {
        escreva("achei o 5, retornando")
        retorne n
      }
    }
  }
}
