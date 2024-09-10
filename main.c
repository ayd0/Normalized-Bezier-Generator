#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>

typedef struct 
{
    Vector2 p;
    bool hover;
    bool selected;
} Bez_p;

int main()
{
    const int SCR_W = 1920;
    const int SCR_H = 1080;
    const float POINT_SIZE = 2.0f;
    bool POINT_SELECTED = false;

    InitWindow(SCR_W, SCR_H, "Bezier Normalizer");

    Camera2D camera = { 0 };

    Vector2 scr_center = {SCR_W / 2.0f, SCR_H / 2.0f};

    camera.offset   = scr_center;
    camera.target   = scr_center;
    camera.rotation = 0.0f;
    camera.zoom     = 2.0f;

    int steps = 30;

    Bez_p p[] = 
    {
        {{scr_center.x - 100.0f, scr_center.y}, false, false},
        {{scr_center.x - 50.0f, scr_center.y + 50.0f}, false, false},
        {{scr_center.x + 50.0f, scr_center.y - 200.0f}, false, false},
        {{scr_center.x + 100.0f, scr_center.y,}, false, false}
    };
    const int p_len = sizeof(p) / sizeof(p[0]);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        Vector2 mw_pos = GetScreenToWorld2D(GetMousePosition(), camera);

        for (int i = 0; i < p_len; ++i)
        {
            if (!POINT_SELECTED)
            {
                if (mw_pos.x >= p[i].p.x - POINT_SIZE * 1.5 && 
                    mw_pos.x <= p[i].p.x + POINT_SIZE * 1.5 &&
                    mw_pos.y >= p[i].p.y - POINT_SIZE * 1.5 &&
                    mw_pos.y <= p[i].p.y + POINT_SIZE * 1.5)
                {
                    p[i].hover = true;
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    {
                        p[i].selected = true;
                        POINT_SELECTED = true;
                    }
                }
                else
                {
                    p[i].hover = false;
                }
            }
            else if (p[i].selected)
            {
                if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {
                    p[i].selected = false;
                    POINT_SELECTED = false;
                    break;
                }
                p[i].p.x = mw_pos.x;
                p[i].p.y = mw_pos.y;
                break;
            }
        }

        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(BLACK);

        DrawLine(p[0].p.x, p[0].p.y, p[1].p.x, p[1].p.y, RAYWHITE);
        DrawLine(p[2].p.x, p[2].p.y, p[3].p.x, p[3].p.y, RAYWHITE);

        for (int i = 0; i < p_len; ++i)
        {
            if (!p[i].hover && !p[i].selected)
            {
                DrawCircle(p[i].p.x, p[i].p.y, POINT_SIZE, YELLOW);
            }
            else
            {
                DrawCircle(p[i].p.x, p[i].p.y, POINT_SIZE * 1.75f, p[i].selected ? BLUE : YELLOW);
            }
        }

        float l_x = p[0].p.x;
        float h_x = p[0].p.x;
        float l_y = p[0].p.y;
        float h_y = p[0].p.y;

        Vector2 lB = p[0].p;
        for (int i = 0; i < steps + 1; ++i)
        {
            float t = (float)i / steps;

            float u = 1 - t;
            float tt = t * t;
            float uu = u * u;
            float uuu = uu * u;
            float ttt = tt * t;

            float x = uuu * p[0].p.x + 3 * uu * t * p[1].p.x + 3 * u * tt * p[2].p.x + ttt * p[3].p.x;
            float y = uuu * p[0].p.y + 3 * uu * t * p[1].p.y + 3 * u * tt * p[2].p.y + ttt * p[3].p.y;

            if (x < l_x) l_x = x;
            if (x > h_x) h_x = x;
            if (y > l_y) l_y = y;
            if (y < h_y) h_y = y;

            Vector2 B = {x, y};
            DrawLineEx(lB, B, 2.0f, MAROON);
            lB = B;
        }

        EndMode2D();

        const int font_size = 20;
        const int msg_startx = 10;
        const int msg_starty = 40;

        for (int i = 0; i < p_len; ++i)
        {
            char pos[25];
            sprintf(pos, "p%d, (%.1f, %.1f)", i, p[i].p.x, p[i].p.y);
            DrawText(pos, msg_startx, (msg_starty) * i + 1, font_size, p[i].selected ? BLUE : YELLOW);
        }

        char l_h_msg[50];
        char h_x_msg[10];
        char l_y_msg[10];
        char h_y_msg[10];
        sprintf(l_h_msg, "l_x: %.1f | h_x: %.1f | l_y: %.1f | h_y: %.1f", l_x, h_x, l_y, h_y);
        DrawText(l_h_msg, msg_startx, msg_starty * p_len, font_size, YELLOW);

        if (h_x - l_x != 0 && h_y - l_y != 0)
        {
            Vector2 normal_coords[p_len];
            for (int i = 0; i < p_len; ++i)
            {
                normal_coords[i].x = (p[i].p.x - l_x) / (h_x - l_x);
                normal_coords[i].y = (p[i].p.y - l_y) / (h_y - l_y);
            }

            char normalized_coord_str[600] = "{";
            for (int i = 0;i < p_len; ++i)
            {
                char coord[50];
                sprintf(coord, "{%.3f, %.3f}", fabsf(normal_coords[i].x), fabsf(normal_coords[i].y));
                strcat(normalized_coord_str, coord);
                if (i < p_len - 1)
                    strcat(normalized_coord_str, ", ");
            }
            strcat(normalized_coord_str, "};");
            Vector2 msg_dims = MeasureTextEx(GetFontDefault(), normalized_coord_str, font_size, 1.0);

            float wsl_y = GetWorldToScreen2D((Vector2){0.0f, l_y}, camera).y;
            DrawText(normalized_coord_str, SCR_W / 2.0f - msg_dims.x / 2.0f, wsl_y + 20, font_size, YELLOW);
        }
        else
        {
            const char* err_msg = "ERR: Cannot divide by zero. Lowest and highest x and y coordinates must differ respectively.";
            Vector2 msg_dims = MeasureTextEx(GetFontDefault(), err_msg, font_size, 1.0);
            DrawText(err_msg, SCR_W / 2.0f - msg_dims.x / 2.0f, 20, font_size, RED);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
