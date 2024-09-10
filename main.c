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

Vector2 BezierCubic(Vector2* p, float t);

int main()
{
    const int SCR_W = 1920;
    const int SCR_H = 1080;
    const float POINT_SIZE = 2.0f;
    bool POINT_SELECTED = false;
    bool IS_LERPING = false;
    float LERP_TIME = 0.0f;
    float LERP_DURATION = 3.0f;

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
        if (IsKeyPressed(KEY_R))
        {
            p[0] = (Bez_p){{scr_center.x - 100.0f, scr_center.y}, false, false};
            p[1] = (Bez_p){{scr_center.x - 50.0f, scr_center.y + 50.0f}, false, false};
            p[2] = (Bez_p){{scr_center.x + 50.0f, scr_center.y - 200.0f}, false, false};
            p[3] = (Bez_p){{scr_center.x + 100.0f, scr_center.y,}, false, false};
        }

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

        Vector2 p_v[p_len];
        for (int i = 0; i < p_len; ++i)
        {
            p_v[i] = p[i].p;
        }

        Vector2 lB = p[0].p;
        for (int i = 0; i < steps + 1; ++i)
        {
            float t = (float)i / steps;

            Vector2 B = BezierCubic(p_v, t);

            if (B.x < l_x) l_x = B.x;
            if (B.x > h_x) h_x = B.x;
            if (B.y > l_y) l_y = B.y;
            if (B.y < h_y) h_y = B.y;

            DrawLineEx(lB, B, 2.0f, MAROON);
            lB = B;
        }

        for (int i = 1; i < p_len - 1; ++i)
        {
            if (p[i].p.x < l_x) l_x = p[i].p.x;
            if (p[i].p.x > h_x) h_x = p[i].p.x;
            if (p[i].p.y > l_y) l_y = p[i].p.y;
            if (p[i].p.y < h_y) h_y = p[i].p.y;
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

        char mouse_msg[50];
        sprintf(mouse_msg, "mouse_x: %.1f, mouse_y: %.1f", mw_pos.x, mw_pos.y);
        DrawText(mouse_msg, msg_startx, msg_starty * (p_len + 1), font_size, YELLOW);

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

            if (IsKeyPressed(KEY_T))
            {
                IS_LERPING = true;
                LERP_TIME = 0.0f;
            }
            if (IsKeyPressed(KEY_C))
            {
                SetClipboardText(normalized_coord_str);
            }

            if (IS_LERPING)
            {
                LERP_TIME += GetFrameTime();

                float t = fmin(LERP_TIME / LERP_DURATION, 1.0f);
                Vector2 bez = BezierCubic(normal_coords, t);

                Color lerp_col = DARKBLUE;
                DrawText("Lerp Test", SCR_W / 2.0f, 15.0f, font_size, lerp_col);
                int starty = 10 + ((SCR_H - 10) * (1 - bez.y));
                DrawRectangle(SCR_W - 90, starty, 80, SCR_H - starty - 40, lerp_col);
                DrawCircle(SCR_W - 130, 40, 30.0f * bez.y, lerp_col);

                if (t == 1.0f)
                {
                    IS_LERPING = false;
                    LERP_TIME = 0.0f;
                }
            }
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

Vector2 BezierCubic(Vector2* p, float t)
{
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;

    float x = u * uu * p[0].x + 3 * uu * t * p[1].x + 3 * u * tt * p[2].x + t * tt * p[3].x;
    float y = u * uu * p[0].y + 3 * uu * t * p[1].y + 3 * u * tt * p[2].y + t * tt * p[3].y;

    return (Vector2){x, y};
}
