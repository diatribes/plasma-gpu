//#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "rlgl.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
    #define GLSL_VERSION 100
#else
    #define GLSL_VERSION 330
#endif

#define TARGET_FPS 80
#define W 320
#define H 200

enum shader_enum{
    shader_enum_plasma = 0,
    shader_enum_curtains,
    shader_enum_plasma_disco,
    shader_enum_plasma_squares,
    shader_enum_count,
};

Shader shader[shader_enum_count];
int shader_index = shader_enum_plasma;
int timeLoc; 
int wLoc;
int hLoc;

RenderTexture2D target;
Camera3D camera = { 0 };

void draw_shader()
{
    BeginShaderMode(shader[shader_index]);
        ClearBackground(WHITE);
        Rectangle source = {0, 0, W, H};
        Rectangle dest = {0, 0, GetRenderWidth(), GetRenderHeight()};
        Vector2 origin = {0, 0};
        DrawTexturePro(target.texture, source, dest, origin, 0.0, WHITE);
    EndShaderMode();
}


void gui_draw_handle_events()
{
    if (IsKeyPressed(KEY_F)) {
        if (IsWindowFullscreen()) {
            RestoreWindow();
        } else {
            ToggleBorderlessWindowed();
        }
    }
    
    if (IsKeyPressed(KEY_SPACE)) {
        shader_index++;
        if (shader_index >= shader_enum_count) {
            shader_index = shader_enum_plasma;;
        }
    }

}

void main_loop_body()
{

    timeLoc = GetShaderLocation(shader[shader_index], "time");
    wLoc = GetShaderLocation(shader[shader_index], "W");
    hLoc = GetShaderLocation(shader[shader_index], "H");

    float time = GetTime();
    float w = W;
    float h = H;
    SetShaderValue(shader[shader_index], timeLoc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader[shader_index], wLoc, &w, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader[shader_index], hLoc, &h, SHADER_UNIFORM_FLOAT);

    BeginDrawing();
        draw_shader();
        gui_draw_handle_events();
        DrawFPS(10, 10);
    EndDrawing();
}

int main(int argc, char * argv[])
{

    InitWindow(W, H, "GPU Plasma");
    
    shader[shader_enum_plasma] = LoadShader(0, TextFormat("resources/plasma_%i.fs", GLSL_VERSION));
    shader[shader_enum_curtains] = LoadShader(0, TextFormat("resources/curtains_%i.fs", GLSL_VERSION));
    shader[shader_enum_plasma_disco] = LoadShader(0, TextFormat("resources/plasma_disco_%i.fs", GLSL_VERSION));
    shader[shader_enum_plasma_squares] = LoadShader(0, TextFormat("resources/plasma_squares_%i.fs", GLSL_VERSION));
    target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    camera.position = (Vector3){ 30.0f, 25.0f, 30.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 80.0f;
    camera.projection = CAMERA_PERSPECTIVE;

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(main_loop_body, TARGET_FPS, 1);
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

