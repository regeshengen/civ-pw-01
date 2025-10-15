# 🧩 Modelagem Fractal em Tempo Real com Geometry Shaders  
### Implementação da Pirâmide de Sierpinski em OpenGL

---

## 🎓 Contexto Acadêmico

Este repositório apresenta o código desenvolvido para o **Trabalho Prático 1 da disciplina Computação Interativa e Visualização**, ministrada pelo **Professor Abel Gomes**, no âmbito do **Mestrado em Engenharia Informática** da **Universidade da Beira Interior (UBI)**.

- **Aluno:** Reges Hengen  
- **Número de aluno:** M13521  
- **Ano letivo:** 2025  
- **Tema do trabalho:** *Modeling Geometric Objects through Geometry Shaders*  

---

## 🧠 Descrição do Projeto

O projeto implementa a **Pirâmide de Sierpinski** utilizando o **OpenGL moderno (Core Profile 4.1)** e **Geometry Shaders** para geração procedural em GPU.  
O fractal é criado a partir de um único ponto, e o *geometry shader* subdivide recursivamente o tetraedro original, gerando sua estrutura auto-semelhante em tempo real.

O objetivo principal é explorar o **potencial dos geometry shaders na modelagem fractal**, demonstrando sua eficiência para renderização interativa e suas limitações em relação ao número de vértices emitidos pela GPU.

---

## 🧰 Tecnologias Utilizadas

- **Linguagem:** C++17  
- **API gráfica:** OpenGL 4.1 Core Profile  
- **Bibliotecas:**  
  - [GLFW](https://www.glfw.org/) – Criação da janela e contexto OpenGL  
  - [GLEW](http://glew.sourceforge.net/) – Carregamento de extensões OpenGL  
  - [GLM](https://github.com/g-truc/glm) – Matemática 3D (vetores e matrizes)  

---

## ⚙️ Dependências (macOS)

Instale as dependências via **Homebrew**:

```bash
brew install glfw glew glm
```

🧩 Estrutura de Arquivos

📦 sierpinski-pyramid
├── main.cpp             # Código principal (setup OpenGL e loop de renderização)
├── shader.vert          # Vertex Shader
├── shader.frag          # Fragment Shader
├── sierpinski.geom      # Geometry Shader (gera o fractal)
└── README.md            # Este arquivo

🚀 Como Compilar e Executar

No terminal, navegue até o diretório do projeto e execute:

clang++ main.cpp -o sierpinski -std=c++17 \
-I/opt/homebrew/opt/glew/include \
-I/opt/homebrew/opt/glfw/include \
-I/opt/homebrew/opt/glm/include \
-L/opt/homebrew/opt/glew/lib \
-L/opt/homebrew/opt/glfw/lib \
-framework OpenGL -lglfw -lglew

Em seguida, execute o programa:

./sierpinski


