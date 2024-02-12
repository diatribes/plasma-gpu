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

#define TARGET_FPS 100
#define W 320
#define H 200

const int num_blocks = 20;
int show_buttons = 1;

enum shader_enum{
    shader_enum_plasma = 0,
    shader_enum_curtains,
    shader_enum_count,
};

enum block_pattern_enum{
    block_pattern_enum_plasma = 0,
    block_pattern_enum_bird,
    block_pattern_enum_none,
    block_pattern_enum_count,
};

Shader shader[shader_enum_count];
int shader_index = shader_enum_plasma;
int block_pattern = block_pattern_enum_plasma;
int timeLoc; 
int wLoc;
int hLoc;

RenderTexture2D target;
Camera3D camera = { 0 };

extern inline float dist(float x1, float y1, float x2, float y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

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

void draw_bird_block_pattern()
{
    float time = GetTime();

    float cameraTime = time*0.4;
    camera.position.x = 30.f + (float)cosf(cameraTime)*90.0f;
    camera.position.z = 30.f + (float)sinf(cameraTime)*300.0f;

    BeginMode3D(camera);

        //Vector3 lastPos = {0, 0, 0};
        for (int x = 0; x < num_blocks; x++)
        {
            for (int y = 0; y < num_blocks; y++)
            {
                for (int z = 0; z < num_blocks; z++)
                {
                    float t = time / 10.f;
                    float d1 = dist(x, z, sinf(t), sinf(y));
                    float d2 = dist(z, x, sinf(t), sinf(x));
                    float d3 = dist(x - z, z + x, sinf(t), sinf(y));
                    float d = sinf(d1) + sinf(d2) + sinf(d3) * 30.f;

                    Vector3 cube_pos = {
                        (float)(x - num_blocks / 2) * d / 10.f + d,
                        (float)(y * cosf(t * 20.f) * d / 12.f) * d / 20.f + d,
                        (float)(z - num_blocks/ 2 ) * d / 10.f + d
                    };

                    Color cube_color;
                    switch(shader_index) {
                        case shader_enum_plasma:
                            cube_color = ColorFromHSV((float)(((x + y + z)*18)%360), 1.f, .9f);
                            break;
                        case shader_enum_curtains:
                            cube_color = ColorFromHSV(fabs(cosf(d / 23.f)) * 300, 1.f, .9f);
                            break;
                        default:
                            cube_color = ColorFromHSV(fabs(cosf(d / 23.f)) * 300, 1.f, .9f);
                            break;
                    }

                    DrawCube(cube_pos, 1.f, 1.f, 1.f, cube_color);
                    //DrawLine3D(cube_pos, lastPos, cube_color);
                    //lastPos = cube_pos;
                }
            }
        }

    EndMode3D();
}


void draw_plasma_block_pattern()
{
    float time = GetTime();

    camera.position.x = 30.f + (float)cosf(time)*93.0f;
    camera.position.z = 30.f + (float)sinf(time)*80.0f;

    BeginMode3D(camera);

        //Vector3 lastPos = {0, 0, 0};
        for (int x = 0; x < num_blocks; x++)
        {
            for (int y = 0; y < num_blocks; y++)
            {
                for (int z = 0; z < num_blocks; z++)
                {
                    float t = time / 10.f;
                    float d1 = dist(x, y, W / 7.0, H / 3.0);
                    float d2 = d1;
                    float d3 = d1;
                    float d = sinf(d1) + sinf(d2) + sinf(d3);

                    Vector3 cube_pos = {
                        (float)(x - num_blocks / 2) * d + d,
                        (float)(y * sin(cosf(t * 20.f + sin(t * 3.0) + sin(x + y + z)) * d)) + d,
                        (float)(z - num_blocks/ 2 ) * d + d,
                    };

                    Color cube_color;
                    switch(shader_index) {
                        case shader_enum_plasma:
                            cube_color = ColorFromHSV((float)(((x + y + z)*18)%360), 1.f, .9f);
                            break;
                        case shader_enum_curtains:
                            cube_color = ColorFromHSV(fabs(cosf(d / 5.f)) * 300, 1.f, .9f);
                            break;
                        default:
                            cube_color = ColorFromHSV(fabs(cosf(d / 5.f)) * 300, 1.f, .9f);
                    }

                    DrawCube(cube_pos, 1.f, 1.f, 1.f, cube_color);
                    //DrawLine3D(cube_pos, lastPos, cube_color);
                    //lastPos = cube_pos;
                }
            }
        }

    EndMode3D();
}

void draw_block_pattern()
{
    switch(block_pattern) {
        case block_pattern_enum_plasma:
            draw_plasma_block_pattern();
            break;
        case block_pattern_enum_bird:
            draw_bird_block_pattern();
            break;
        case block_pattern_enum_none:
        default:
            break;
    }
}

void gui_draw_handle_events()
{
    if (IsKeyDown(KEY_F)) {
        WaitTime(.2);
        if (IsWindowFullscreen()) {
            RestoreWindow();
        } else {
            ToggleBorderlessWindowed();
        }
    }

    if (IsKeyDown(KEY_B)) {
        WaitTime(.2);
        show_buttons ^= 1;
    }

    const int bw = 100;
    const int bh = 30;
    Rectangle bounds = { 0, 0 + 60, bw, bh };
    if (IsKeyDown(KEY_SPACE) || (show_buttons && GuiButton(bounds, "Next Plasma"))) {

        WaitTime(.2);
        shader_index++;
        if (shader_index >= shader_enum_count) {
            shader_index = shader_enum_plasma;;
        }

    }

    bounds.y += 40;
    if (IsKeyDown(KEY_ENTER) || (show_buttons && GuiButton(bounds, "Next Block"))) {
        WaitTime(.2);
        block_pattern++;
        if (block_pattern >= block_pattern_enum_count) {
            block_pattern = block_pattern_enum_plasma;;
        }
    }


    bounds.y += 40;
    if (show_buttons && GuiButton(bounds, "Hide Buttons")) {
        show_buttons = 0;
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
        draw_block_pattern();
        gui_draw_handle_events();
        DrawFPS(10, 10);
    EndDrawing();
}

int main(int argc, char * argv[])
{

    InitWindow(W, H, "gpu plasma");
    //GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    
    shader[shader_enum_plasma] = LoadShader(0, TextFormat("resources/plasma_%i.fs", GLSL_VERSION));
    shader[shader_enum_curtains] = LoadShader(0, TextFormat("resources/curtains_%i.fs", GLSL_VERSION));
    target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    camera.position = (Vector3){ 30.0f, 25.0f, 30.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 80.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(main_loop_body, 120, 1);
#else

    SetTargetFPS(TARGET_FPS);
    //DisableCursor();
    ToggleBorderlessWindowed();
    while (!WindowShouldClose()) {
        main_loop_body();
    }
    //EnableCursor();
#endif

    CloseWindow();
    return 0;
}

