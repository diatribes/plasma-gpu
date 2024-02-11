//#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
    #define GLSL_VERSION 100
#else
    #define GLSL_VERSION 330
#endif

#define TARGET_FPS 100
#define W 200 
#define H 200 
#define DROP_SIZE 100

#define MODE_PLASMA 0
#define MODE_CURTAINS 1
#define MODE_METABALLS_RGB 2
#define MODE_METABALLS_HSV 3
#define MODE_METABALLS_WAVY_HSV 4
#define MODE_MAX 2
int mode = MODE_PLASMA;


Shader shader[MODE_MAX];
int timeLoc; 
int wLoc;
int hLoc;
RenderTexture2D target;

void main_loop_body()
{
    if (IsKeyDown(KEY_F)) {
        WaitTime(.2);
        if (IsWindowFullscreen()) {
            RestoreWindow();
        } else {
            ToggleBorderlessWindowed();
        }
    }

    if (IsKeyDown(KEY_SPACE)) {
        WaitTime(.2);
        mode++;
        if (mode >= MODE_MAX) {
            mode = MODE_PLASMA;
        }
    }

    timeLoc = GetShaderLocation(shader[mode], "time");
    wLoc = GetShaderLocation(shader[mode], "W");
    hLoc = GetShaderLocation(shader[mode], "H");

    float time = GetTime();
    float w = W;
    float h = H;
    SetShaderValue(shader[mode], timeLoc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader[mode], wLoc, &w, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader[mode], hLoc, &h, SHADER_UNIFORM_FLOAT);

    BeginDrawing();
        BeginShaderMode(shader[mode]);
            ClearBackground(WHITE);
            Rectangle source = {0, 0, W, H};
            Rectangle dest = {0, 0, GetRenderWidth(), GetRenderHeight()};
            Vector2 origin = {0, 0};
            DrawTexturePro(target.texture, source, dest, origin, 0.0, WHITE);
        EndShaderMode();

    EndDrawing();
}

int main(int argc, char * argv[])
{

    InitWindow(W, H, "gpu plasma");
    
    shader[MODE_PLASMA] = LoadShader(0, TextFormat("resources/plasma_%i.fs", GLSL_VERSION));
    shader[MODE_CURTAINS] = LoadShader(0, TextFormat("resources/curtains_%i.fs", GLSL_VERSION));
    target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());


#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(main_loop_body, 120, 1);
#else

    SetTargetFPS(TARGET_FPS);
    DisableCursor();
    ToggleBorderlessWindowed();
    while (!WindowShouldClose()) {
        main_loop_body();
    }
    EnableCursor();
#endif

    CloseWindow();
    return 0;
}
