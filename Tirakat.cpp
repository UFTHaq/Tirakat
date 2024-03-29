// Tirakat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <raylib.h>


extern "C" {
    #include <libavformat/avformat.h>
}

//#define FONT_LOC {"D:/Coding/Raylib C++/Tirakat/resources/Fonts/Playfair_Display/static/PlayfairDisplay-Regular.ttf"}
#define FONT_LOC_Roboto_Slab {"D:/Coding/Raylib C++/Tirakat/resources/Fonts/Roboto_Slab/static/RobotoSlab-Regular.ttf"}
//#define FONT_LOC_BOLD {"D:/Coding/Raylib C++/Tirakat/resources/Fonts/Roboto_Slab/static/RobotoSlab-SemiBold.ttf"}
#define FONT_LOC_Roboto_Mono {"D:/Coding/Raylib C++/Tirakat/resources/Fonts/Roboto_Mono/static/RobotoMono-SemiBold.ttf"}
#define FONT_LOC_Source_Sans {"D:/Coding/Raylib C++/Tirakat/resources/Fonts/Source_Sans_3/static/SourceSans3-SemiBold.ttf"}

#define ICON_APP_LOC        {"D:/Coding/Raylib C++/Tirakat/resources/Icons/Tirakat-favicon.png"}
#define ICON_PLAYPAUSE_LOC  {"D:/Coding/Raylib C++/Tirakat/resources/Icons/PlayPause.png"}
#define ICON_FULLSCREEN_LOC {"D:/Coding/Raylib C++/Tirakat/resources/Icons/Fullscreen.png"}
#define ICON_VOLUME_LOC     {"D:/Coding/Raylib C++/Tirakat/resources/Icons/Volume.png"}

#define HUD_TIMER_SECS 1.0F
#define PANEL_LEFT_WIDTH 325.0F
#define PANEL_INFO_HEIGHT 70.0F
#define PANEL_PROGRESS_HEIGHT 15.0F

enum Page {
    PAGE_DRAG_DROP,
    PAGE_MAIN
};

enum Drag {
    DRAG_MUSIC_PROGRESS,
    DRAG_VOLUME,
    DRAG_RELEASE
};

struct Plug {
    int page{};
    int play{};
    int dragging{};
    bool music_playing{};
    bool reset_time{};
    bool icon_pp_index{};
    bool volume_mute{ false };
    float last_volume{};
    size_t icon_fullscreen_index{};
    bool fullscreen{ false };
    bool mouse_onscreen{ true };
};

Plug tirakat{};
static Plug* p = &tirakat;

struct Data {
    std::string path{};
    std::string name{};
    int target{};
    int counter{};
    int duration{};
};


void InitFile(const std::filesystem::path& filename);

void FileCheck(const std::filesystem::path& filename);

void FileZeroDataCheck(const std::filesystem::path& filename);

int GetDuration(const char* c_file_path);

void ReloadVector();

void DrawTitleMP3(Rectangle& panel_main);

void DrawCounter(Rectangle& panel_main);

void DrawDuration(Rectangle& panel_duration);

void DrawPlayPause(const Rectangle& play_rect);


Vector2 mouse_position{};
std::vector<Data> data{};
size_t data_size{};
size_t order{};
const std::filesystem::path data_dir{ "resources/Data" };
const std::filesystem::path data_txt{ "resources/Data/data.txt" };
bool zero_data{false};
Font* font = nullptr;

Texture2D PLAYPAUSE_TEX{};
Texture2D FULLSCREEN_TEX{};
Texture2D VOLUME_TEX{};

std::ostringstream formatted_duration{};
std::ostringstream formatted_progress{};

int music_time{};
int duration{};

Music music{};

int main()
{
    std::cout << "Hello World!\n";
    std::cout << "RAYLIB VERSION: " << RAYLIB_VERSION << std::endl;

    Color BASE_COLOR = Color{ 20, 20, 20, 150 };
    Color PANEL_COLOR = Color{ 35, 35, 35, 255 };
    Color PANEL_LINE_COLOR = Color{ 60, 60, 60, 255 };
    Color PANEL_PROGRESS_BASE_COLOR = Color{ 25, 25, 25, 255 };
    Color PANEL_PROGRESS_COLOR = Color{ 60, 178, 181, 255 };
    Color CONTENT_COLOR = Color{ 50, 50, 50, 255 };
    Color CONTENT_CHOOSE_COLOR = Color{ 50, 169, 75, 255 };

    const int screen_w = 1200;
    const int screen_h = 675;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
    //SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screen_w, screen_h, "Tirakat");
    InitAudioDevice();
    SetTargetFPS(60);
    SetWindowIcon(LoadImage(ICON_APP_LOC));

    Font font_m = LoadFontEx(FONT_LOC_Roboto_Slab, 60, 0, 0);
    SetTextureFilter(font_m.texture, TEXTURE_FILTER_BILINEAR);

    Font font_s = LoadFontEx(FONT_LOC_Source_Sans, 38, 0, 0);
    SetTextureFilter(font_s.texture, TEXTURE_FILTER_BILINEAR);

    Font font_number = LoadFontEx(FONT_LOC_Roboto_Mono, 40, 0, 0);
    SetTextureFilter(font_number.texture, TEXTURE_FILTER_BILINEAR);

    Font font_counter = LoadFontEx(FONT_LOC_Roboto_Mono, 100, 0, 0);
    SetTextureFilter(font_counter.texture, TEXTURE_FILTER_BILINEAR);

    Image play_pause_icon = LoadImage(ICON_PLAYPAUSE_LOC);
    PLAYPAUSE_TEX = LoadTextureFromImage(play_pause_icon);
    SetTextureFilter(PLAYPAUSE_TEX, TEXTURE_FILTER_BILINEAR);

    Image fullscreen_icon = LoadImage(ICON_FULLSCREEN_LOC);
    FULLSCREEN_TEX = LoadTextureFromImage(fullscreen_icon);
    SetTextureFilter(FULLSCREEN_TEX, TEXTURE_FILTER_BILINEAR);

    Image volume_icon = LoadImage(ICON_VOLUME_LOC);
    VOLUME_TEX = LoadTextureFromImage(volume_icon);
    SetTextureFilter(VOLUME_TEX, TEXTURE_FILTER_BILINEAR);

    FileCheck(data_txt);

    

    
    if (zero_data == true) {
        p->page = PAGE_DRAG_DROP;
    }
    else {
        p->page = PAGE_MAIN;

        ReloadVector();

    }

    music = LoadMusicStream(data.at(order).path.c_str());
    if (IsMusicReady(music)) {
        SetMusicVolume(music, 0.05F);
        p->dragging = DRAG_RELEASE;
        p->music_playing = true;
        p->last_volume = GetMasterVolume();
    }

    while (!WindowShouldClose()) {
        if (zero_data == true) {
            p->page = PAGE_DRAG_DROP;
        }
        else {
            p->page = PAGE_MAIN;
        }

        BeginDrawing();
        ClearBackground(BASE_COLOR);

        mouse_position = GetMousePosition();

        float screen_w = static_cast<float>(GetScreenWidth());
        float screen_h = static_cast<float>(GetScreenHeight());

        SetWindowMinSize(800, 450);

        font = &font_m;
        if (p->page == PAGE_DRAG_DROP) {
            const char* text = "Drag&Drop MP3 Here";
            float font_size = 50;
            float font_space = 1;
            Color font_color = RAYWHITE;
            Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
            Vector2 text_coor = {
                (screen_w - text_measure.x) / 2,
                (screen_h - text_measure.y) / 2
            };
            DrawTextEx(*font, text, text_coor, font_size, font_space, font_color);
        }
        else if (p->page == PAGE_MAIN) {

            duration = data.at(order).duration;
            
            if (p->fullscreen) {
                // TODO: different layouts and add HUD
                static float hud_timer = HUD_TIMER_SECS;

                Rectangle panel_main = {
                    0,
                    0,
                    screen_w,
                    screen_h - PANEL_INFO_HEIGHT
                };

                DrawTitleMP3(panel_main);
                font = &font_counter;
                DrawCounter(panel_main);


                if (hud_timer > 0.0F) {

                }
                Rectangle panel_bottom{
                    0, 
                    screen_h - PANEL_INFO_HEIGHT,
                    screen_w,
                    PANEL_INFO_HEIGHT
                };

                Rectangle panel_info{
                    panel_bottom.x,
                    panel_bottom.y,
                    panel_bottom.width,
                    PANEL_INFO_HEIGHT - PANEL_PROGRESS_HEIGHT
                };

                Rectangle panel_duration{
                    panel_info.x,
                    panel_info.y,
                    PANEL_LEFT_WIDTH,
                    panel_info.height
                };
                DrawRectangleRec(panel_duration, BLACK);
                font = &font_number;
                DrawDuration(panel_duration);

                float pad = 12.0F;
                float button_w = panel_info.height;
                Rectangle play_base_panel = {
                    panel_duration.width,
                    panel_info.y,
                    button_w,
                    button_w
                };
                Rectangle play_rect{
                    play_base_panel.x + (pad * 1),
                    play_base_panel.y + (pad * 1),
                    play_base_panel.width - (pad * 2),
                    play_base_panel.height - (pad * 2),
                };
                DrawPlayPause(play_rect);
            }
            else {

            }
            
            // INPUT KEYBOARD & MOUSE
            if (IsKeyPressed(KEY_SPACE)) {
                p->music_playing = !p->music_playing;
            }

            //

            if (IsMusicReady(music)) {
                SetMusicVolume(music, 0.05F); 
                UpdateMusicStream(music);

                if (p->music_playing) {
                    PlayMusicStream(music);
                }
            } 

            static bool need_resume = false;
            if (p->music_playing) {
                p->icon_pp_index = true;

                if (need_resume) {
                    ResumeMusicStream(music); // Perhaps need to make condition toggle for resume only once after pause
                    need_resume = false;
                }
            }
            else {
                p->icon_pp_index = false;
                PauseMusicStream(music);
                need_resume = true;
            }

            // INFO PANEL
            float panel_info_h = PANEL_INFO_HEIGHT;
            float panel_progress_h = PANEL_PROGRESS_HEIGHT;
            Rectangle panel_info_base{
                0,
                screen_h - panel_info_h,
                screen_w,
                panel_info_h - panel_progress_h
            };

            float offset_y = 4.0F;
            Rectangle panel_info{
                panel_info_base.x,
                panel_info_base.y + offset_y,
                panel_info_base.width,
                panel_info_base.height - offset_y
            };
            DrawRectangleRec(panel_info, PANEL_COLOR);

            float panel_w = PANEL_LEFT_WIDTH;
            Rectangle panel_duration{
                panel_info.x,
                panel_info.y,
                panel_w,
                panel_info.height
            };

            if (p->reset_time == true) {
                SeekMusicStream(music, 0.1F);
                p->reset_time = false;
                p->music_playing = true;
            }

            //music_time = 0;
            if (p->dragging == DRAG_RELEASE) {
                music_time = static_cast<int>(GetMusicTimePlayed(music) * 1000);
            }

            // DURATION PANEL
            font = &font_number;
            DrawDuration(panel_duration);


            // LEFT PANEL
            Rectangle panel_left{
                0,
                0,
                panel_w,
                panel_info_base.y
            };
            DrawRectangleRec(panel_left, PANEL_COLOR);
            // VERTICAL LINE
            float line_width = 4.0F;
            Rectangle panel_left_line{
                panel_left.x + panel_left.width - line_width,
                0,
                line_width,
                screen_h
            };
            DrawRectangleRec(panel_left_line, PANEL_LINE_COLOR);
            // HORIZONTAL LINE
            Rectangle panel_info_line{
                0,
                panel_info_base.y,
                screen_w,
                line_width
            };
            DrawRectangleRec(panel_info_line, PANEL_LINE_COLOR);

            // INNER PANEL LEFT
            Rectangle inner_panel_left_boundary{
                0,
                0,
                panel_left.width * 0.925F,
                panel_left.height
            };
            //DrawRectangleRec(inner_panel_left_boundary, GRAY);

            static float content_scroll = 0;

            float content_h = 55.0F;
            float content_panel_pad = content_h * 0.075F;
            Rectangle content_boundary{
                inner_panel_left_boundary.x,
                content_scroll,
                inner_panel_left_boundary.width,
                content_h
            };


            BeginScissorMode(
                static_cast<int>(inner_panel_left_boundary.x),
                static_cast<int>(inner_panel_left_boundary.y),
                static_cast<int>(inner_panel_left_boundary.width),
                static_cast<int>(inner_panel_left_boundary.height)
            );

            for (size_t i = 0; i < data_size; i++) {
                Rectangle content{
                    content_boundary.x + (content_panel_pad * 2),
                    content_boundary.y + (content_panel_pad * 2) + (i * content_h),
                    content_boundary.width - (content_panel_pad * 2),
                    content_boundary.height - (content_panel_pad * 2)
                };
                //DrawRectangleRec(content, GRAY);
                Color color_content = CONTENT_COLOR;
                Color color_font = RAYWHITE;
                if (CheckCollisionPointRec(mouse_position, inner_panel_left_boundary)) {

                    if (CheckCollisionPointRec(mouse_position, content)) {
                        color_content = DARKGRAY;

                        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                            order = i;
                            // Music Play Load
                            music = LoadMusicStream(data.at(order).path.c_str());
                            p->reset_time = true;
                        }
                    }
                }
                if (i == order) {
                    color_content = CONTENT_CHOOSE_COLOR;
                    color_font = BLACK;
                }

                DrawRectangleRounded(content, 0.2F, 10, color_content);

                font = &font_s;
                float text_width_limit = content.width - 30.0F;
                float font_size = content_h * 0.475F;
                float font_space = -0.25F;
                float text_width = 0.0F;
                int max_chars = 0;

                std::string cpp_text = data.at(i).name;
                while (text_width < text_width_limit && max_chars < cpp_text.length()) {
                    text_width = MeasureTextEx(*font, cpp_text.substr(0, max_chars + 1).c_str(), font_size, font_space).x;
                    max_chars++;
                }

                std::string first_10 = cpp_text.substr(0, max_chars);
                const char* text = first_10.c_str();
                Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
                Vector2 text_coor = {
                    content.x + 12.0F,
                    content.y + (content.height - text_measure.y) / 2,
                };
                DrawTextEx(*font, text, text_coor, font_size, font_space, color_font);

            }

            EndScissorMode();


            // MULTIMEDIA PANEL
            {
                // PLAY BUTTON
                float button_w = panel_info.height;
                float pad = 8.0F;
                //float offset_y = 2.0F;
                Rectangle play_base_panel = {
                    panel_left.width,
                    panel_info.y,
                    button_w,
                    button_w
                };
                Rectangle play_rect{
                    play_base_panel.x + (pad * 1),
                    play_base_panel.y + (pad * 1),
                    play_base_panel.width - (pad * 2),
                    play_base_panel.height - (pad * 2),
                };
                //DrawRectangleRec(play_rect, RED);
                DrawPlayPause(play_rect);

                // VOLUME BUTTON
                pad = 10.0F;
                Rectangle volume_base_panel = {
                    panel_left.width + (button_w),
                    play_base_panel.y,
                    button_w,
                    button_w
                };
                Rectangle volume_rect{
                    volume_base_panel.x + (pad * 1),
                    volume_base_panel.y + (pad * 1),
                    volume_base_panel.width - (pad * 2),
                    volume_base_panel.height - (pad * 2),
                };
                //DrawRectangleRec(volume_rect, RED);

                Color icon_color = LIGHTGRAY;
                if (CheckCollisionPointRec(mouse_position, volume_rect)) {
                    icon_color = WHITE;
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        p->volume_mute = !p->volume_mute;
                    }
                }
                else {
                    icon_color = LIGHTGRAY;
                }

                static size_t icon_index = 0;
                if (p->volume_mute) {
                    icon_index = 0;
                    SetMasterVolume(0.0F);
                }
                else {
                    SetMasterVolume(p->last_volume);
                    p->last_volume = GetMasterVolume();

                    if (p->last_volume < 0.5F) {
                        icon_index = 1;
                    }
                    else {
                        icon_index = 2;
                    }
                }

                float icon_size = 100.0F;
                {
                    Rectangle dest = volume_rect;
                    Rectangle source{ icon_index * icon_size, 0, icon_size, icon_size };
                    DrawTexturePro(VOLUME_TEX, source, dest, { 0,0 }, 0, icon_color);
                }

                // VOLUME SLIDER
                static float hud_volume = 0;
                if (CheckCollisionPointRec(mouse_position, volume_rect)) {
                    hud_volume = HUD_TIMER_SECS;
                }

                if (hud_volume > 0.0F) {
                    hud_volume -= GetFrameTime();

                    float volume_slider_length_base = 230.F;
                    Rectangle volume_slider_base_panel{
                        volume_base_panel.x + volume_base_panel.width,
                        volume_base_panel.y,
                        volume_slider_length_base,
                        button_w
                    };
                    //DrawRectangleRec(volume_slider_base_panel, DARKGRAY);

                    float volume_slider_length = 200.F;
                    float volume_slider_h = button_w * 0.25F;
                    Rectangle volume_slider_outer{
                        volume_slider_base_panel.x + 15,
                        volume_slider_base_panel.y + (volume_slider_base_panel.height - volume_slider_h) / 2,
                        volume_slider_length,
                        volume_slider_h
                    };
                    DrawRectangleRounded(volume_slider_outer, 0.7F, 5, BASE_COLOR);

                    float vol_pad = 3.0F;
                    float vol_ratio = volume_slider_outer.width / 1;
                    float vol_length = vol_ratio * GetMasterVolume();
                    Rectangle volume_slider_inner{
                        volume_slider_outer.x + (vol_pad * 1),
                        volume_slider_outer.y + (vol_pad * 1),
                        vol_length - (vol_pad * 2),
                        volume_slider_outer.height - (vol_pad * 2),
                    };
                    DrawRectangleRounded(volume_slider_inner, 0.7F, 5, LIGHTGRAY);

                    if (CheckCollisionPointRec(mouse_position, volume_slider_base_panel)) {
                        hud_volume = HUD_TIMER_SECS;
                    }

                }


                // FULLSCREEN BUTTON
                pad = 7.0F;
                Rectangle fullscreen_base_panel{
                    panel_info.width - button_w,
                    play_base_panel.y,
                    button_w,
                    button_w
                };
                Rectangle fullscreen_rect{
                    fullscreen_base_panel.x + (pad * 1),
                    fullscreen_base_panel.y + (pad * 1),
                    fullscreen_base_panel.width - (pad * 2),
                    fullscreen_base_panel.height - (pad * 2),
                };
                //DrawRectangleRec(fullscreen_rect, RED);

                if (p->fullscreen == false) {
                    if (CheckCollisionPointRec(mouse_position, fullscreen_rect)) {
                        p->icon_fullscreen_index = 1;
                        icon_color = WHITE;
                        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                            p->fullscreen = true;
                        }
                    }
                    else {
                        p->icon_fullscreen_index = 0;
                        icon_color = LIGHTGRAY;
                    }
                }
                else {
                    if (CheckCollisionPointRec(mouse_position, fullscreen_rect)) {
                        p->icon_fullscreen_index = 3;
                        icon_color = WHITE;
                        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                            p->fullscreen = false;
                        }
                    }
                    else {
                        p->icon_fullscreen_index = 2;
                        icon_color = LIGHTGRAY;
                    }
                }

                {
                    float icon_size = 100.0F;
                    Rectangle dest = fullscreen_rect;
                    Rectangle source{ p->icon_fullscreen_index * icon_size, 0, icon_size, icon_size };
                    DrawTexturePro(FULLSCREEN_TEX, source, dest, { 0,0 }, 0, icon_color);
                }
                
            }

            // PROGRESS PANEL
            Rectangle panel_progress{
                0,
                panel_info.y + panel_info.height,
                screen_w,
                panel_progress_h
            };
            DrawRectangleRec(panel_progress, PANEL_PROGRESS_BASE_COLOR);

            float progress_ratio = static_cast<float>(screen_w) / duration;
            static float progress_w = 0;
            if (p->dragging != DRAG_MUSIC_PROGRESS) {
                progress_w = progress_ratio * music_time;
            }
            
            Rectangle progress_bar{
                panel_progress.x,
                panel_progress.y,
                progress_w,
                panel_progress.height
            };
            DrawRectangleRec(progress_bar, PANEL_PROGRESS_COLOR);

            static bool dragging_progress = false;
            if (CheckCollisionPointRec(mouse_position, panel_progress)) {
                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    float t = (mouse_position.x - panel_progress.x) / panel_progress.width;
                    SeekMusicStream(music, (t * duration / 1000));
                } 
                else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    p->dragging = DRAG_MUSIC_PROGRESS;
                }
            }

            if (p->dragging == DRAG_MUSIC_PROGRESS && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                p->music_playing = false;
                progress_w = mouse_position.x;
                music_time = static_cast<int>(progress_w / progress_ratio);

                if (music_time < 0) {
                    music_time = 0;
                }
                else if (music_time > duration) {
                    music_time = duration;
                }
            }
            else if (p->dragging == DRAG_MUSIC_PROGRESS && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                p->dragging = DRAG_RELEASE;
                p->music_playing = true;

                float t = (mouse_position.x - panel_progress.x) / panel_progress.width;
                if (mouse_position.x < 0) {
                    t = (1 - panel_progress.x) / panel_progress.width;
                }
                else if (mouse_position.x > screen_w) {
                    t = (1 + screen_w - panel_progress.x) / panel_progress.width;
                }
                SeekMusicStream(music, (t * duration / 1000));
            }


            // MAIN PANEL
            Rectangle panel_main{
                panel_left.x + panel_left.width,
                0,
                screen_w - panel_left.width,
                screen_h - panel_info_base.height - panel_progress.height
            };
            //DrawRectangleRec(panel_main, BLACK);
            float padding = 20.0F;

            {
                BeginScissorMode(
                    static_cast<int>(panel_main.x + (padding * 1)),
                     static_cast<int>(panel_main.y + (padding * 1)),
                     static_cast<int>(panel_main.width - (padding * 2)),
                     static_cast<int>(panel_main.height - (padding * 2))
                );

                    DrawTitleMP3(panel_main);

                    font = &font_counter;
                    DrawCounter(panel_main);

                EndScissorMode();
            }


        }

        if (IsFileDropped()) {
            // TODO: put dialog. what is the target repetition value for this mp3.

            FilePathList dropped_files = LoadDroppedFiles();

            const char* c_file_path = dropped_files.paths[0];
            std::string cpp_file_path = std::string(c_file_path);
            std::string file_name = std::filesystem::path(cpp_file_path).stem().string();

            if (IsFileExtension(c_file_path, ".mp3")) {
                TraceLog(LOG_INFO, "SUCCESS: Adding new file [ %s.mp3 ]", file_name.c_str());

                Data newData{};
                newData.path = cpp_file_path;
                newData.name = file_name;
                newData.duration = GetDuration(c_file_path);

                data.push_back(newData);

                std::ofstream file(data_txt);

                if (file.is_open()) {
                    for (const auto& entry : data) {
                        file << entry.path << ","
                             << entry.name << ","
                             << entry.target << ","
                             << entry.counter << ","
                             << entry.duration << std::endl;
                    }
                    file.close();
                    TraceLog(LOG_INFO, "[SUCCESS] Save [%s] to data.txt", file_name.c_str());
                    zero_data = false;
                    ReloadVector();
                    order = data.size() - 1;
                }
                else {
                    TraceLog(LOG_ERROR, "Failed to save [%s]", file_name.c_str());
                }

            }
            else {
                TraceLog(LOG_ERROR, "Adding new file, that's not mp3");
            }

            UnloadDroppedFiles(dropped_files);
        }


        EndDrawing();
    }
    CloseWindow();
}

void DrawPlayPause(const Rectangle& play_rect)
{
    Color icon_color = LIGHTGRAY;
    if (CheckCollisionPointRec(mouse_position, play_rect)) {
        icon_color = WHITE;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            p->music_playing = !p->music_playing;
        }
    }
    else {
        icon_color = LIGHTGRAY;
    }

    //size_t icon_index = 0;
    float icon_size = 100.0F;
    {
        Rectangle dest = play_rect;
        Rectangle source{ p->icon_pp_index * icon_size, 0, icon_size, icon_size };
        DrawTexturePro(PLAYPAUSE_TEX, source, dest, { 0,0 }, 0, icon_color);
    }
}

void DrawDuration(Rectangle& panel_duration)
{
    // RE-CLEAR OSTRINGSTREAMS
    formatted_duration.str("");
    formatted_progress.str("");

    if (duration < 3600 * 1000) {
        int minutes_dur = duration / (60 * 1000);
        int seconds_dur = (duration / 1000) % 60;

        int minutes_pro = music_time / (60 * 1000);
        int seconds_pro = (music_time / 1000) % 60;

        formatted_duration << std::setw(2) << std::setfill('0') << minutes_dur << ":" << std::setw(2) << std::setfill('0') << seconds_dur;
        formatted_progress << std::setw(2) << std::setfill('0') << minutes_pro << ":" << std::setw(2) << std::setfill('0') << seconds_pro;
    }
    else {
        int hour_dur = duration / (3600 * 1000);
        int minutes_dur = (duration / (60 * 1000)) % 60;
        int seconds_dur = (duration / 1000) % 60;

        int hour_pro = music_time / (3600 * 1000);
        int minutes_pro = (music_time / (60 * 1000)) % 60;
        int seconds_pro = (music_time / 1000) % 60;

        formatted_duration << std::setw(2) << std::setfill('0') << hour_dur << ":"
            << std::setw(2) << std::setfill('0') << minutes_dur << ":"
            << std::setw(2) << std::setfill('0') << seconds_dur;

        formatted_progress << std::setw(2) << std::setfill('0') << hour_pro << ":"
            << std::setw(2) << std::setfill('0') << minutes_pro << ":"
            << std::setw(2) << std::setfill('0') << seconds_pro;
    }

    std::string duration_string = formatted_duration.str();
    std::string progress_string = formatted_progress.str();

    std::string cpp_text = progress_string + " / " + duration_string;
    float font_size = panel_duration.height * 0.55F;
    float font_space = 0.5F;
    const char* text = cpp_text.c_str();
    Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
    Vector2 text_coor{
        panel_duration.x + (panel_duration.width - text_measure.x) / 2,
        panel_duration.y + (panel_duration.height - text_measure.y) / 2
    };
    DrawTextEx(*font, text, text_coor, font_size, font_space, RAYWHITE);

}

void DrawCounter(Rectangle& panel_main)
{
    std::string counter = std::to_string(data.at(order).counter);
    std::string target = std::to_string(data.at(order).target);
    std::string cpp_text = counter + " / " + target;

    const char* text = cpp_text.c_str();
    float font_size = 80.0F;
    float font_space = 0.0F;
    Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
    Vector2 text_coor{
        panel_main.x + (panel_main.width - text_measure.x) / 2,
        panel_main.y + (panel_main.height - text_measure.y) / 2 + 20.0F
    };
    DrawTextEx(*font, text, text_coor, font_size, font_space, RAYWHITE);
}

void DrawTitleMP3(Rectangle& panel_main)
{
    std::string cpp_text = data.at(order).name;
    const char* text = cpp_text.c_str();
    float font_size = 32.0F;
    float font_space = 0.0F;
    Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
    Vector2 text_coor{
        panel_main.x + (panel_main.width - text_measure.x) / 2,
        panel_main.y + (40.0F)
    };
    DrawTextEx(*font, text, text_coor, font_size, font_space, RAYWHITE);
}

void ReloadVector()
{
    // Load all data
    std::ifstream file(data_txt);
    data.clear();

    if (file.is_open()) {
        std::string line{};
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::vector<std::string> tokens{};
            std::string token{};

            while (std::getline(iss, token, ',')) {
                tokens.push_back(token);
            }

            Data entry{};
            entry.path = tokens.at(0);
            entry.name = tokens.at(1);
            entry.target = std::stoi(tokens.at(2));
            entry.counter = std::stoi(tokens.at(3));
            entry.duration = std::stoi(tokens.at(4));

            data.push_back(entry);
        }
    }

    data_size = data.size();

    for (const auto& i : data) {
        std::cout << i.name << std::endl;
        std::cout << i.duration << std::endl;
    }
}

int GetDuration(const char* c_file_path)
{
    avformat_network_init();
    AVFormatContext* formatContext = nullptr;
    if (avformat_open_input(&formatContext, c_file_path, nullptr, nullptr) != 0) {
        std::cerr << "Error: Couldn't open file '" << c_file_path << "'" << std::endl;
        //return 1;
    }
    // Retrieve stream information
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        std::cerr << "Error: Couldn't find stream information" << std::endl;
        avformat_close_input(&formatContext);
        //return 1;
    }
    // Get duration in seconds
    int64_t durationInSeconds = formatContext->duration / AV_TIME_BASE;

    // Convert duration to milliseconds
    int durationInMilliseconds = static_cast<int>(durationInSeconds * 1000);

    // Print duration in milliseconds
    std::cout << "Duration: " << durationInMilliseconds << " milliseconds" << std::endl;

    // Close the input file
    avformat_close_input(&formatContext);

    return durationInMilliseconds;
}

void FileCheck(const std::filesystem::path& filename)
{
    if (std::filesystem::exists(data_dir) == false) {
        TraceLog(LOG_ERROR, "[resources/Data] Directory is missing.");
        std::filesystem::create_directories(data_dir);
        TraceLog(LOG_INFO, "Create Data Directory");
    }

    if (std::filesystem::exists(filename) == true) {
        TraceLog(LOG_INFO, "File: [%s] Already Exists", filename.filename().string().c_str());

        FileZeroDataCheck(filename);
    }
    else if (std::filesystem::exists(filename) == false) {
        TraceLog(LOG_ERROR, "[Data/data.txt] not found.");
        InitFile(filename);
    }
}

void InitFile(const std::filesystem::path& filename) {
    std::ofstream file(filename);

    if (file.is_open()) {
        file << "0";
        file.close();
        TraceLog(LOG_INFO, "[SUCCESS] File created: %s", filename.string().c_str());

        FileZeroDataCheck(filename);
    }
    else {
        TraceLog(LOG_ERROR, "[ERROR] Failed to created file: %s", filename.string().c_str());
    }
}

void FileZeroDataCheck(const std::filesystem::path& filename)
{
    std::ifstream file(filename);

    std::string token{};
    if (file.is_open()) {
        if (file >> token) {
            if (token == "0") {
                zero_data = true; // Set zero_data to false if value is not 0
            }
        }
        file.close();
    }

    if (zero_data) {
        TraceLog(LOG_INFO, "Zero Data");
        //std::cout << "Zero data" << std::endl;
    }
    else {
        TraceLog(LOG_INFO, "Non-Zero Data: %s", std::string(token).c_str());
        //std::cout << "Non-zero data: " << token << std::endl;
    }
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
