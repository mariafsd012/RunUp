#include <raylib.h>
#include <stdio.h>

// Adicionamos as telas de Game Over e Vitória
typedef enum { TELA_MENU, TELA_JOGO, TELA_FASE1, TELA_GAMEOVER, TELA_VITORIA } EstadoJogo;

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "RunUp - Zona Zero");

    EstadoJogo telaAtual = TELA_MENU;

    // Carregamento de Texturas
    Texture2D pinguimTex = LoadTexture("pinguim-removebg-preview.png"); 
    float escalaPinguim = 0.4f; 
    
    // --- VARIÁVEIS DE JOGO ---
    Vector2 jogadorPos = { 40, 40 };
    float velocidade = 4.0f;
    int nivelAtual = 1;
    
    // Lógica da Barra de Corrupção (Exclusão)
    float barraY = (float)screenHeight - 20; 
    double tempoUltimoUpdateBarra = GetTime();

    // Definição da Saída (Portal X)
    Rectangle saida = { 740, 380, 30, 30 };

    // --- DEFINIÇÃO DAS PAREDES DO LABIRINTO ---
    Rectangle paredes[] = {
        { 0, 0, 800, 20 },          // Topo
        { 0, 430, 800, 20 },        // Chão
        { 0, 0, 20, 450 },          // Esquerda
        { 780, 0, 20, 450 },        // Direita
        { 150, 0, 30, 300 },        // Parede 1
        { 300, 150, 30, 300 },       // Parede 2
        { 450, 0, 30, 300 },        // Parede 3
        { 600, 150, 30, 300 }        // Parede 4
    };
    int numParedes = sizeof(paredes) / sizeof(paredes[0]);

    Rectangle btnEntrar = { screenWidth/2 - 100, 320, 200, 50 };
    bool mouseNoBotao = false;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        Vector2 mousePos = GetMousePosition();

        // --- LÓGICA / UPDATE ---
        if (telaAtual == TELA_MENU) 
        {
            if (CheckCollisionPointRec(mousePos, btnEntrar)) {
                mouseNoBotao = true;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    telaAtual = TELA_JOGO;
                    nivelAtual = 1;
                }
            } else mouseNoBotao = false;
        }
        else if (telaAtual == TELA_JOGO)
        {
            if (IsKeyPressed(KEY_ENTER)) {
                telaAtual = TELA_FASE1;
                jogadorPos = (Vector2){ 40, 40 };
                barraY = (float)screenHeight - 20;
                tempoUltimoUpdateBarra = GetTime();
            }
        }
        else if (telaAtual == TELA_FASE1)
        {
            // 1. Movimentação da Barra (Igual ao seu código base: sobe a cada 4 segundos)
            if (GetTime() - tempoUltimoUpdateBarra >= 4.0) {
                barraY -= 40; // A barra sobe
                tempoUltimoUpdateBarra = GetTime();
            }

            // 2. Movimentação do Jogador
            Vector2 novaPos = jogadorPos;
            if (IsKeyDown(KEY_RIGHT)) novaPos.x += velocidade;
            if (IsKeyDown(KEY_LEFT)) novaPos.x -= velocidade;
            if (IsKeyDown(KEY_UP)) novaPos.y -= velocidade;
            if (IsKeyDown(KEY_DOWN)) novaPos.y += velocidade;

            Rectangle corpoJogador = { novaPos.x, novaPos.y, pinguimTex.width * 0.15f, pinguimTex.height * 0.15f };

            // 3. Colisão com Paredes
            bool colidiu = false;
            for (int i = 0; i < numParedes; i++) {
                if (CheckCollisionRecs(corpoJogador, paredes[i])) {
                    colidiu = true;
                    break;
                }
            }
            if (!colidiu) jogadorPos = novaPos;

            // 4. Colisão com a Barra (Game Over)
            if (jogadorPos.y + (pinguimTex.height * 0.15f) > barraY) {
                telaAtual = TELA_GAMEOVER;
            }

            // 5. Colisão com a Saída (Próximo Nível)
            if (CheckCollisionRecs(corpoJogador, saida)) {
                if (nivelAtual < 3) {
                    nivelAtual++;
                    jogadorPos = (Vector2){ 40, 40 }; // Reinicia posição
                    barraY = (float)screenHeight - 20; // Reinicia barra
                } else {
                    telaAtual = TELA_VITORIA;
                }
            }
        }
        else if (telaAtual == TELA_GAMEOVER || telaAtual == TELA_VITORIA) {
            if (IsKeyPressed(KEY_R)) telaAtual = TELA_MENU;
        }

        // --- DESENHO ---
        BeginDrawing();
            
            if (telaAtual == TELA_FASE1) ClearBackground(BLACK); 
            else DrawRectangleGradientV(0, 0, screenWidth, screenHeight, DARKBLUE, SKYBLUE);

            if (telaAtual == TELA_MENU) 
            {
                DrawText("RunUp: Zona Zero", screenWidth/2 - MeasureText("RunUp: Zona Zero", 40)/2, 150, 40, WHITE);
                DrawRectangleRec(btnEntrar, mouseNoBotao ? LIGHTGRAY : WHITE); 
                DrawRectangleLinesEx(btnEntrar, 3, DARKBLUE);
                DrawText("ENTRAR", btnEntrar.x + (btnEntrar.width/2 - MeasureText("ENTRAR", 20)/2), btnEntrar.y + 15, 20, DARKBLUE);
                DrawTextureEx(pinguimTex, (Vector2){ (float)screenWidth - 180, (float)screenHeight - 180 }, 0.0f, escalaPinguim, WHITE);
            } 
            else if (telaAtual == TELA_JOGO) 
            {
                DrawText("ZONA ZERO: A SIMULAÇÃO", 50, 40, 25, RED);
                DrawText("A IA ORBITA ativou o protocolo de exclusao.", 50, 100, 20, WHITE);
                DrawText("A barra de corrupcao subira a cada 4 segundos.", 50, 140, 20, YELLOW);
                DrawText("Nao deixe que a barra te toque!", 50, 170, 20, YELLOW);
                DrawText("Pressione ENTER para começar", 50, 400, 15, DARKBLUE);
                DrawTextureEx(pinguimTex, (Vector2){ (float)screenWidth - 180, (float)screenHeight - 180 }, 0.0f, escalaPinguim, WHITE);
            }
            else if (telaAtual == TELA_FASE1)
            {
                // Desenhar Labirinto
                for (int i = 0; i < numParedes; i++) {
                    DrawRectangleRec(paredes[i], BLUE);
                    DrawRectangleLinesEx(paredes[i], 2, DARKBLUE);
                }

                // Desenhar Barra de Corrupção (Igual ao base)
                DrawRectangle(0, (int)barraY, screenWidth, 10, RED);
                DrawRectangleGradientV(0, (int)barraY + 10, screenWidth, screenHeight - (int)barraY, MAROON, BLACK);

                // Desenhar Saída (X)
                DrawRectangleRec(saida, LIME);
                DrawText("X", (int)saida.x + 8, (int)saida.y + 5, 20, BLACK);

                // Desenhar Jogador
                DrawTextureEx(pinguimTex, jogadorPos, 0.0f, 0.2f, WHITE);
                
                DrawText(TextFormat("NÍVEL %d - ESCAPE!", nivelAtual), 20, 25, 20, LIME);
            }
            else if (telaAtual == TELA_GAMEOVER) {
                ClearBackground(BLACK);
                DrawText("VOCÊ FOI DELETADO!", screenWidth/2 - 150, 200, 30, RED);
                DrawText("Pressione R para recomeçar", screenWidth/2 - 120, 250, 20, RAYWHITE);
            }
            else if (telaAtual == TELA_VITORIA) {
                ClearBackground(DARKBLUE);
                DrawText("SISTEMA INVADIDO: VOCÊ ESCAPOU!", screenWidth/2 - 250, 200, 30, GOLD);
                DrawText("Pressione R para voltar ao menu", screenWidth/2 - 120, 250, 20, RAYWHITE);
            }

        EndDrawing();
    }

    UnloadTexture(pinguimTex);
    CloseWindow();
    return 0;
}