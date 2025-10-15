#version 410 core

layout(points) in;
layout(triangle_strip, max_vertices = 1024) out;

uniform mat4 uMVP;    // matriz Model-View-Projection
uniform int uDepth;   // profundidade desejada (0..3)
uniform float uScale; // tamanho do tetra inicial
uniform vec3 uColor;  // cor base

// Quantidade máxima de tetras suportada para depthLimit = 3 -> 4^3 = 64
const int MAX_TETRAS = 256;

struct Tetra {
    vec3 a;
    vec3 b;
    vec3 c;
    vec3 d;
};

void emitTriangle(vec3 v0, vec3 v1, vec3 v2)
{
    gl_Position = uMVP * vec4(v0, 1.0);
    EmitVertex();
    gl_Position = uMVP * vec4(v1, 1.0);
    EmitVertex();
    gl_Position = uMVP * vec4(v2, 1.0);
    EmitVertex();
    EndPrimitive();
}

void emitTetraFaces(Tetra t)
{
    // Cada tetra tem 4 faces triangulares
    emitTriangle(t.a, t.b, t.c);
    emitTriangle(t.a, t.b, t.d);
    emitTriangle(t.a, t.c, t.d);
    emitTriangle(t.b, t.c, t.d);
}

void main()
{
    // Limita profundidade para evitar overflow no GS (ajustável)
    int depthLimit = min(uDepth, 3);

    // Tetra inicial regular centrado na origem
    float s = uScale;
    vec3 A = vec3(  s,  s,  s);
    vec3 B = vec3( -s, -s,  s);
    vec3 C = vec3( -s,  s, -s);
    vec3 D = vec3(  s, -s, -s);

    // arrays dimensionados pelo máximo esperado a depthLimit <= 3 => 4^3 = 64 tetras
    Tetra cur[MAX_TETRAS];
    Tetra next[MAX_TETRAS];

    int curCount = 1;
    cur[0].a = A;
    cur[0].b = B;
    cur[0].c = C;
    cur[0].d = D;

    // Subdivide até depthLimit
    for (int level = 0; level < depthLimit; ++level) {
        int nextCount = 0;

        for (int i = 0; i < curCount; ++i) {
            Tetra t = cur[i];
            vec3 a = t.a; vec3 b = t.b; vec3 c = t.c; vec3 d = t.d;

            // Pontos médios
            vec3 ab = (a + b) * 0.5;
            vec3 ac = (a + c) * 0.5;
            vec3 ad = (a + d) * 0.5;
            vec3 bc = (b + c) * 0.5;
            vec3 bd = (b + d) * 0.5;
            vec3 cd = (c + d) * 0.5;

            // 4 sub-tetras
            next[nextCount++] = Tetra(a, ab, ac, ad);
            next[nextCount++] = Tetra(ab, b, bc, bd);
            next[nextCount++] = Tetra(ac, bc, c, cd);
            next[nextCount++] = Tetra(ad, bd, cd, d);
        }

        // Atualiza lista corrente
        curCount = nextCount;
        for (int j = 0; j < curCount; ++j) {
            cur[j] = next[j];
        }
    }

    // Emite todas as faces finais
    for (int i = 0; i < curCount; ++i) {
        emitTetraFaces(cur[i]);
    }
}
