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