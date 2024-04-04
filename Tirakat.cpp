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
#define FONT_LOC_Source_Sans_BOLD {"D:/Coding/Raylib C++/Tirakat/resources/Fonts/Source_Sans_3/static/SourceSans3-Bold.ttf"}
#define FONT_LOC_Source_Sans_SEMIBOLD {"D:/Coding/Raylib C++/Tirakat/resources/Fonts/Source_Sans_3/static/SourceSans3-SemiBold.ttf"}
#define FONT_LOC_Source_Sans_REG {"D:/Coding/Raylib C++/Tirakat/resources/Fonts/Source_Sans_3/static/SourceSans3-Regular.ttf"}

#define ICON_APP_LOC        {"D:/Coding/Raylib C++/Tirakat/resources/Icons/Tirakat-favicon.png"}
#define ICON_PLAYPAUSE_LOC  {"D:/Coding/Raylib C++/Tirakat/resources/Icons/PlayPause.png"}
#define ICON_FULLSCREEN_LOC {"D:/Coding/Raylib C++/Tirakat/resources/Icons/Fullscreen.png"}
#define ICON_VOLUME_LOC     {"D:/Coding/Raylib C++/Tirakat/resources/Icons/Volume.png"}
#define ICON_SETTING_LOC    {"D:/Coding/Raylib C++/Tirakat/resources/Icons/Setting.png"}
#define ICON_X_LOC          {"D:/Coding/Raylib C++/Tirakat/resources/Icons/X.png"}
#define ICON_DELETE_LOC     {"D:/Coding/Raylib C++/Tirakat/resources/Icons/Trash.png"}

#define HUD_TIMER_SECS 1.0F
#define PANEL_LEFT_WIDTH 320.0F
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

enum Toggle {
    OFF,
    ON
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

void DrawPlayPause(const Rectangle& play_rect, const Rectangle& hover_panel);

void ApplyInputReset(std::string& input, bool& popup_on, std::string& name, bool& setting_on);

bool Save();

void LoadMP3();


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
Texture2D SETTING_TEX{};
Texture2D X_TEX{};
Texture2D DELETE_TEX{};

std::ostringstream formatted_duration{};
std::ostringstream formatted_progress{};

int music_time{};
int duration{};

Music music{};

static std::string input{};
size_t letter_size = input.size();

int main()
{
    std::cout << "Hello World!\n";
    std::cout << "RAYLIB VERSION: " << RAYLIB_VERSION << std::endl;

    Color BASE_COLOR = Color{ 20, 20, 20, 200 };
    Color PANEL_COLOR = Color{ 35, 35, 35, 255 };
    Color PANEL_LINE_COLOR = Color{ 60, 60, 60, 255 };
    Color PANEL_PROGRESS_BASE_COLOR = Color{ 25, 25, 25, 255 };
    Color PANEL_PROGRESS_COLOR = Color{ 60, 178, 181, 255 };
    Color CONTENT_COLOR = Color{ 50, 50, 50, 255 };
    Color CONTENT_CHOOSE_COLOR = Color{ 50, 169, 75, 255 };
    Color BLUE_BUTTON_COLOR = Color{ 58, 76, 131, 255 };
    Color POPUP_CARD_COLOR = Color{ 112, 141, 225, 255 };
    Color POPUP_X_COLOR = Color{ 190, 60, 50, 255 };
    Color POPUP_BODY_COLOR = Color{ 203, 209, 216, 255 };
    Color POPUP_APPLY_COLOR = Color{ 75, 109, 214, 255 };
    Color POPUP_CANCEL_COLOR = Color{ 142, 149, 178, 255 };

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

    Font font_s_bold = LoadFontEx(FONT_LOC_Source_Sans_BOLD, 40, 0, 0);
    SetTextureFilter(font_s_bold.texture, TEXTURE_FILTER_BILINEAR);

    Font font_s_semibold = LoadFontEx(FONT_LOC_Source_Sans_SEMIBOLD, 40, 0, 0);
    SetTextureFilter(font_s_semibold.texture, TEXTURE_FILTER_BILINEAR);

    Font font_s_reg = LoadFontEx(FONT_LOC_Source_Sans_REG, 40, 0, 0);
    SetTextureFilter(font_s_reg.texture, TEXTURE_FILTER_BILINEAR);

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

    Image setting_icon = LoadImage(ICON_SETTING_LOC);
    SETTING_TEX = LoadTextureFromImage(setting_icon);
    SetTextureFilter(SETTING_TEX, TEXTURE_FILTER_BILINEAR);

    Image x_icon = LoadImage(ICON_X_LOC);
    X_TEX = LoadTextureFromImage(x_icon);
    SetTextureFilter(X_TEX, TEXTURE_FILTER_BILINEAR);

    Image delete_icon = LoadImage(ICON_DELETE_LOC);
    DELETE_TEX = LoadTextureFromImage(delete_icon);
    SetTextureFilter(DELETE_TEX, TEXTURE_FILTER_BILINEAR);

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
            // RELOAD DURATION INCASE CHANGING MUSIC
            duration = data.at(order).duration;

            // INPUT KEYBOARD & MOUSE
            if (IsKeyPressed(KEY_SPACE)) {
                p->music_playing = !p->music_playing;
            }

            //

            if (IsMusicReady(music)) {
                SetMusicVolume(music, 0.1F);
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

            
            if (p->fullscreen) {
                // TODO: different layouts and add HUD
                static float hud_timer = HUD_TIMER_SECS;

                // MAIN PANEL
                Rectangle panel_main = {
                    0,
                    0,
                    screen_w,
                    screen_h - PANEL_INFO_HEIGHT
                };

                // DRAW
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
                DrawPlayPause(play_rect, play_base_panel);
            }
            if (!p->fullscreen) {

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
            if (p->dragging != DRAG_MUSIC_PROGRESS) {
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
            float line_width = offset_y;
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
                panel_left.width * 0.940F,
                panel_left.height
            };
            //DrawRectangleRec(inner_panel_left_boundary, GRAY);

            float content_h = 53.0F;
            float content_panel_pad = content_h * 0.075F;
            float visible_area_size = panel_left.height;
            float entire_scrollable_area = content_h * data.size();

            static float content_scroll = 0;
            static float content_velocity = 0;
            content_velocity *= 0.9F;

            Rectangle content_boundary{
                inner_panel_left_boundary.x,
                inner_panel_left_boundary.y,
                inner_panel_left_boundary.width,
                content_h
            };

            if (CheckCollisionPointRec(mouse_position, panel_left)) {
                content_velocity += GetMouseWheelMove() * data.size() * (content_h * 3 / 4);
            }
            content_scroll -= content_velocity * GetFrameTime();

            static bool scrolling = false;
            static float scrolling_mouse_offset = 0.0F;
            if (scrolling) {
                content_scroll = (mouse_position.y - panel_left.y - scrolling_mouse_offset) / visible_area_size * entire_scrollable_area;
            }

            float min_scroll = 0;
            if (content_scroll < min_scroll) content_scroll = min_scroll;
            float max_scroll = entire_scrollable_area - visible_area_size;
            if (max_scroll < 0) max_scroll = 0;
            if (content_scroll > max_scroll) content_scroll = max_scroll;


            BeginScissorMode(
                static_cast<int>(inner_panel_left_boundary.x),
                static_cast<int>(inner_panel_left_boundary.y),
                static_cast<int>(inner_panel_left_boundary.width),
                static_cast<int>(inner_panel_left_boundary.height)
            );

            for (size_t i = 0; i < data.size(); i++) {

                Rectangle content{
                    content_boundary.x + (content_panel_pad * 2),
                    content_boundary.y + (content_panel_pad * 1) + (i * content_h) - content_scroll,
                    content_boundary.width - (content_panel_pad * 3),
                    content_boundary.height - (content_panel_pad * 2)
                };
                //DrawRectangleRec(content, GRAY);

                Color color_content = CONTENT_COLOR;
                Color color_font = RAYWHITE;
                if (CheckCollisionPointRec(mouse_position, inner_panel_left_boundary)) {

                    if (CheckCollisionPointRec(mouse_position, content)) {
                        color_content = DARKGRAY;

                        if (p->dragging == DRAG_RELEASE) {
                            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                                order = i;
                                // Music Play Load
                                music = LoadMusicStream(data.at(order).path.c_str());
                                p->reset_time = true;
                            }
                        }
                    }
                }
                if (i == order) {
                    color_content = CONTENT_CHOOSE_COLOR;
                    color_font = BLACK;
                }

                DrawRectangleRounded(content, 0.2F, 10, color_content);

                font = &font_s_semibold;
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

            if (entire_scrollable_area > visible_area_size) {
                float t = visible_area_size / entire_scrollable_area;
                float q = content_scroll / entire_scrollable_area;
                float padding = line_width;
                Rectangle scroll_bar_area{
                    inner_panel_left_boundary.x + inner_panel_left_boundary.width + (padding * 0.5F),
                    inner_panel_left_boundary.y + (padding * 1),
                    panel_left.width - inner_panel_left_boundary.width - (padding * 2.75F),
                    inner_panel_left_boundary.height - (padding * 2)
                };
                //DrawRectangleRec(scroll_bar_area, DARKBLUE);

                Rectangle scroll_bar_boundary{
                    scroll_bar_area.x,
                    scroll_bar_area.y + scroll_bar_area.height * q,
                    scroll_bar_area.width,
                    scroll_bar_area.height * t
                };
                DrawRectangleRounded(scroll_bar_boundary, 0.7F, 10, Fade(CONTENT_CHOOSE_COLOR, 0.4F));
                
                if (scrolling) {
                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        scrolling = false;
                    }

                }
                else {
                    if (CheckCollisionPointRec(mouse_position, scroll_bar_boundary)) {


                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            scrolling = true;
                            scrolling_mouse_offset = mouse_position.y - scroll_bar_boundary.y;
                        }
                    } 
                    else if (CheckCollisionPointRec(mouse_position, scroll_bar_area)) {
                        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                            if (mouse_position.y < scroll_bar_boundary.y) {
                                content_velocity += (content_h * 20);
                            }
                            else if (scroll_bar_boundary.y + scroll_bar_boundary.height < mouse_position.y) {
                                content_velocity += -(content_h * 20);
                            }
                        }
                    }
                }
            }



            // MULTIMEDIA PANEL
            
            // PLAY BUTTON
            float button_w = panel_info.height;
            float pad = 8.0F;
            //float offset_y = 2.0F;
            Rectangle play_panel = {
                panel_left.width,
                panel_info.y,
                button_w,
                button_w
            };
            Rectangle play_rect{
                play_panel.x + (pad * 1),
                play_panel.y + (pad * 1),
                play_panel.width - (pad * 2),
                play_panel.height - (pad * 2),
            };
            //DrawRectangleRec(play_rect, RED);
            DrawPlayPause(play_rect, play_panel);

            // VOLUME BUTTON
            static bool HUD_toggle = false;
            float volume_slider_length_base = 230.F;
            Rectangle volume_base_panel = {
                panel_left.width + button_w,
                play_panel.y,
                button_w + volume_slider_length_base ,
                button_w
            };
            //DrawRectangleRec(volume_base_panel, DARKGRAY);

            Rectangle volume_panel{
                volume_base_panel.x,
                volume_base_panel.y,
                button_w,
                button_w
            };
            //DrawRectangleRec(volume_panel, DARKBLUE);

            pad = 8.0F;
            Rectangle volume_icon_rect{
                volume_base_panel.x + (pad * 1),
                volume_base_panel.y + (pad * 1),
                button_w - (pad * 2),
                button_w - (pad * 2),
            };
            //DrawRectangleRec(volume_icon_rect, RED);

            Color icon_color = LIGHTGRAY;
            float volume = GetMasterVolume();
            if (CheckCollisionPointRec(mouse_position, volume_panel)) {
                icon_color = WHITE;
                HUD_toggle = true;
                if (CheckCollisionPointRec(mouse_position, volume_panel) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (volume > 0) {
                        p->last_volume = volume;
                        volume = 0;
                    }
                    else {
                        volume = p->last_volume;
                    }
                    SetMasterVolume(volume);
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

                if (volume == 0.0F) {
                    p->volume_mute;
                    icon_index = 0;
                }
                else if (volume < 0.5F) {
                    icon_index = 1;
                }
                else {
                    icon_index = 2;
                }
            }

            float icon_size = 100.0F;
            {
                Rectangle dest = volume_icon_rect;
                Rectangle source{ icon_index * icon_size, 0, icon_size, icon_size };
                DrawTexturePro(VOLUME_TEX, source, dest, { 0,0 }, 0, icon_color);
            }

            // VOLUME SLIDER 2.0
            static bool drag_volume = false;
            if (HUD_toggle) {
                // make new rect
                Rectangle volume_slider_panel{
                    volume_panel.x + volume_panel.width,
                    volume_panel.y,
                    volume_slider_length_base,
                    volume_panel.height
                };
                //DrawRectangleRec(volume_slider_panel, DARKBROWN);

                // SLIDER DRAW - START
                float volume_slider_w = 200.F;
                float volume_slider_h = button_w * 0.15F;
                float vol_ratio = volume_slider_w / 1;
                float vol_length = vol_ratio * GetMasterVolume();

                // OUTLINE SLIDER
                Rectangle volume_slider_outer{
                    volume_slider_panel.x + 15,
                    volume_slider_panel.y + (volume_slider_panel.height - volume_slider_h) / 2,
                    volume_slider_w,
                    volume_slider_h
                };
                DrawRectangleRounded(volume_slider_outer, 0.7F, 5, BASE_COLOR);
                DrawRectangleRoundedLines(volume_slider_outer, 0.7F, 5, 3.0F, BASE_COLOR);

                Rectangle volume_slider{
                    volume_slider_panel.x + 15,
                    volume_slider_panel.y + (volume_slider_panel.height - volume_slider_h) / 2,
                    vol_length,
                    volume_slider_h
                };
                DrawRectangleRounded(volume_slider, 0.7F, 5, GRAY);

                // DRAW CIRCLE
                int circle_center_x = static_cast<int>(volume_slider.x) + static_cast<int>(volume_slider.width);
                int circle_center_y = static_cast<int>(volume_slider.y) + static_cast<int>(volume_slider.height / 2) + 1;
                float radius = 7.0F;
                DrawCircle(circle_center_x, circle_center_y, radius + 5, BASE_COLOR);
                DrawCircle(circle_center_x, circle_center_y, radius, LIGHTGRAY);
                // SLIDER DRAW - END


                // DRAG
                bool inSlider = (CheckCollisionPointRec(mouse_position, volume_slider_panel));
                if (inSlider) {
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        vol_length = mouse_position.x - volume_slider.x;
                        if (vol_length < 0) vol_length = 0;
                        if (vol_length > 200) vol_length = 200;
                        volume = vol_length / vol_ratio;
                        SetMasterVolume(volume);

                        std::cout << vol_length << " : " << volume << std::endl;

                    }
                    else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                        if (p->dragging != DRAG_MUSIC_PROGRESS) {
                            //drag_volume = true;
                            p->dragging = DRAG_VOLUME;
                        }
                    }
                }

                //if (drag_volume) {
                if (p->dragging == DRAG_VOLUME) {
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                        vol_length = mouse_position.x - volume_slider.x;
                        if (vol_length < 0) vol_length = 0;
                        if (vol_length > 200) vol_length = 200;
                        volume = vol_length / vol_ratio;
                        SetMasterVolume(volume);
                    
                        std::cout << vol_length << " : " << volume << std::endl;
                    }
                    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        //drag_volume = false;
                        p->dragging = DRAG_RELEASE;
                    }
                }

                // MOUSE WHEEL INPUT
                float mouse_wheel_step = 0.05F;
                float wheel_delta = GetMouseWheelMove();
                volume += wheel_delta * mouse_wheel_step;
                if (volume < 0) volume = 0;
                if (volume > 1) volume = 1;
                    
                SetMasterVolume(volume);

            }

            // OUTSIDE OF HUD
            bool outVolumeBase = !(CheckCollisionPointRec(mouse_position, volume_base_panel));
            if (outVolumeBase) {
                HUD_toggle = false;
            }

            //if (drag_volume) {
            if (p->dragging == DRAG_VOLUME) {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && outVolumeBase) {
                    HUD_toggle = true;
                }
                else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && outVolumeBase) {
                    HUD_toggle = false;
                    //drag_volume = false;
                    p->dragging = DRAG_RELEASE;
                }
            }

            // FULLSCREEN BUTTON
            pad = 4.5F;
            Rectangle fullscreen_panel{
                panel_info.width - button_w,
                play_panel.y,
                button_w,
                button_w
            };
            Rectangle fullscreen_rect{
                fullscreen_panel.x + (pad * 1),
                fullscreen_panel.y + (pad * 1),
                fullscreen_panel.width - (pad * 2),
                fullscreen_panel.height - (pad * 2),
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
            Color progress_color_hover = PANEL_PROGRESS_COLOR;
            //if (CheckCollisionPointRec(mouse_position, panel_progress)) {
            //    progress_color_hover = GREEN;
            //}
            //else {
            //    progress_color_hover = PANEL_PROGRESS_COLOR;
            //}
            DrawRectangleRec(progress_bar, progress_color_hover);

            //static bool dragging_progress = false;
            if (p->dragging != DRAG_VOLUME) {
                if (CheckCollisionPointRec(mouse_position, panel_progress)) {

                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        float t = (mouse_position.x - panel_progress.x) / panel_progress.width;
                        SeekMusicStream(music, (t * duration / 1000));
                    }

                    else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                        p->dragging = DRAG_MUSIC_PROGRESS;
                    }
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

            // SETTING PANEL
            static bool setting_on = false;
            float setting_card_w = 180.0F;
            Rectangle setting_base_panel{
                panel_main.x,
                panel_main.y + panel_main.height - volume_panel.height - 5,
                button_w + setting_card_w + button_w,
                button_w
            };
            //DrawRectangleRec(setting_base_panel, PANEL_COLOR);
            pad = 8.0F;
            Rectangle setting_rect_icon{
                setting_base_panel.x + (pad * 1),
                setting_base_panel.y + (pad * 1),
                button_w - (pad * 2),
                button_w - (pad * 2),
            };
            //DrawRectangleRec(setting_rect_icon, RED);

            icon_color = GRAY;
            if (CheckCollisionPointRec(mouse_position, setting_rect_icon)) {
                icon_color = RAYWHITE;
                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    setting_on = !setting_on;
                }
            }
            else {
                icon_color = GRAY;
            }

            {
                float icon_size = 100.0F;
                Rectangle dest = setting_rect_icon;
                Rectangle source{ 0, 0, icon_size, icon_size };
                DrawTexturePro(SETTING_TEX, source, dest, { 0,0 }, 0, icon_color);
            }

            if (setting_on == ON) {
                Rectangle setting_card_base{
                    setting_base_panel.x + button_w,
                    setting_base_panel.y,
                    setting_base_panel.width,
                    setting_base_panel.height
                };
                //DrawRectangleRec(setting_card_base, LIGHTGRAY);

                // DRAW SETTING CARD
                pad = 4.0F;
                Rectangle setting_card{
                    setting_card_base.x,
                    setting_card_base.y + (pad * 1),
                    setting_card_base.width - setting_card_base.height,
                    setting_card_base.height - (pad * 2)
                };
                //DrawRectangleRec(setting_card, PANEL_COLOR);
                DrawRectangleRounded(setting_card, 0.5F, 10, PANEL_COLOR);

                // DRAW CENTER LINE
                float line_h = setting_card.height * 0.7F;
                float line_w = 5.0F;
                Rectangle setting_center_line{
                    setting_card.x + ((setting_card.width - line_w) * (1.15F / 3.0F)),
                    setting_card.y + ((setting_card.height - line_h) / 2),
                    line_w,
                    line_h
                };
                DrawRectangleRounded(setting_center_line, 0.8F, 10, DARKGRAY);

                // DRAW MUSIC TARGET NOW
                pad = 6.0F;
                Rectangle target_rect{
                    setting_card.x + (pad * 1.5F),
                    setting_card.y + (pad * 1),
                    setting_center_line.x - setting_card.x - (pad * 3.0F),
                    setting_card.height - (pad * 2)
                };
                //DrawRectangleRounded(target_rect, 0.5F, 10, DARKGRAY);
                {
                    font = &font_number;
                    std::string target = std::to_string(data.at(order).target);
                    const char* text = target.c_str();
                    float font_size = target_rect.height * 0.9F;
                    float font_space = 0.0F;
                    Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
                    Vector2 text_coor{
                        target_rect.x + (target_rect.width - text_measure.x) / 2,
                        target_rect.y + (target_rect.height - text_measure.y) / 2
                    };
                    DrawTextEx(*font, text, text_coor, font_size, font_space, RAYWHITE);
                }
                // DRAW RESET BUTTON
                static bool popup_on = false;
                pad = 6.0F;
                Rectangle reset_rect{
                    (setting_center_line.x + setting_center_line.width) + 5 + (pad * 1.5F),
                    setting_card.y + (pad * 1.25F),
                    setting_center_line.x - setting_card.x + 10 - (pad * 3.0F),
                    setting_card.height - (pad * 2.5F)
                };
                DrawRectangleRounded(reset_rect, 0.5F, 10, BLUE_BUTTON_COLOR);

                Color font_color = LIGHTGRAY;
                float font_coef = 0.8F;
                font = &font_s_semibold;
                if (CheckCollisionPointRec(mouse_position, reset_rect)) {
                    font_coef = 0.825F;
                    font_color = WHITE;
                    font = &font_s_bold;
                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        popup_on = !popup_on;
                    }
                }

                {
                    const char* text = "RESET";
                    float font_size = reset_rect.height * font_coef;
                    float font_space = 0.0F;
                    Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
                    Vector2 text_coor{
                        reset_rect.x + (reset_rect.width - text_measure.x) / 2,
                        reset_rect.y + (reset_rect.height - text_measure.y) / 2
                    };
                    DrawTextEx(*font, text, text_coor, font_size, font_space, font_color);
                }

                // DRAW DELELE RECT
                Rectangle delete_rect{
                    setting_card.x + setting_card.width - setting_base_panel.height + 5 + (pad * 1.0F),
                    setting_card.y + (pad * 1.0F),
                    setting_card.height - (pad * 2.0F),
                    setting_card.height - (pad * 2.0F)
                };
                //DrawRectangleRounded(delete_rect, 0.2F, 10, RED);

                //bool delete_music = false;
                icon_color = LIGHTGRAY;
                if (CheckCollisionPointRec(mouse_position, delete_rect)) {
                    icon_color = RED;
                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {

                        if (order >= 0 && order < data.size()) {
                            data.erase(data.begin() + order);
                        
                            if (order == data.size()) {
                                order--;
                            }
                        }
                        
                        // SAVE TO TXT AGAIN
                        music = LoadMusicStream(data.at(order).path.c_str());
                        p->reset_time = true;

                        if (Save())  TraceLog(LOG_INFO, "[SUCCESS] Delete Music");
                    }
                }
                else {
                    icon_color = LIGHTGRAY;
                }

                {
                    float icon_size = 100.0F;
                    Rectangle dest = delete_rect;
                    Rectangle source{ 0, 0, icon_size, icon_size };
                    DrawTexturePro(DELETE_TEX, source, dest, { 0,0 }, 0, icon_color);
                }

                // DRAW POPUP RESET TARGET
                std::string popup_title{ "Reset Target" };
                std::string name{ data.at(order).name };
                size_t size_name = name.size();
                std::string popup_name{ data.at(order).name.substr(0, 25) };
                std::string popup_old_target{ std::to_string(data.at(order).target) };
                //std::string popup_old_target{ "999"};
                if (popup_on == ON) {

                    float popup_w = 400.0F;
                    float popup_h = 200.0F;
                    Rectangle popup_card{
                        panel_main.x + ((panel_main.width - popup_w) / 2),
                        panel_main.y + ((panel_main.height - popup_h) / 2),
                        popup_w,
                        popup_h
                    };
                    DrawRectangleRounded(popup_card, 0.1F, 10, POPUP_CARD_COLOR);

                    // TOP PANEL
                    float pad = 7.0F;
                    Rectangle popup_top_panel{
                        popup_card.x,
                        popup_card.y,
                        popup_card.width,
                        (popup_card.height * 1 / 4) - (pad * 1)
                    };
                    //DrawRectangleRounded(popup_top_panel, 0.4F, 10, DARKGRAY);
                    // DRAW TITLE
                    Rectangle title_rect{
                        popup_top_panel.x,
                        popup_top_panel.y,
                        popup_top_panel.width - popup_top_panel.height,
                        popup_top_panel.height
                    };
                    //DrawRectangleRounded(title_rect, 0.4F, 10, BLUE);
                    {
                        font = &font_s_bold;
                        font_color = BLACK;
                        const char* text = popup_title.c_str();
                        float font_size = title_rect.height * 0.6F;
                        float font_space = 0.0F;
                        Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
                        Vector2 text_coor{
                            title_rect.x + 10.0F,
                            title_rect.y + (title_rect.height - text_measure.y) / 2
                        };
                        DrawTextEx(*font, text, text_coor, font_size, font_space, font_color);
                    }


                    // DRAW X BUTTON
                    pad = 7.0F;
                    Rectangle x_rect_base{
                        (popup_top_panel.x + popup_top_panel.width - popup_top_panel.height) + (pad * 1),
                        popup_top_panel.y + (pad * 1),
                        popup_top_panel.height - (pad * 2),
                        popup_top_panel.height - (pad * 2)
                    };
                    DrawRectangleRounded(x_rect_base, 0.4F, 10, POPUP_X_COLOR);

                    float x_pad = 1.5F;
                    icon_color = LIGHTGRAY;
                    if (CheckCollisionPointRec(mouse_position, x_rect_base)) {
                        icon_color = RAYWHITE;
                        x_pad = 0.0F;
                        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                            popup_on = OFF;
                        }
                    }
                    else {
                        icon_color = LIGHTGRAY;
                    }
                    Rectangle x_rect{
                        x_rect_base.x + (x_pad * 1),
                        x_rect_base.y + (x_pad * 1),
                        x_rect_base.height - (x_pad * 2),
                        x_rect_base.height - (x_pad * 2)
                    };

                    {
                        float icon_size = 100.0F;
                        Rectangle dest = x_rect;
                        Rectangle source{ 0, 0, icon_size, icon_size };
                        DrawTexturePro(X_TEX, source, dest, { 0,0 }, 0, icon_color);
                    }

                    // DRAW BODY
                    pad = 7.0F;
                    Rectangle popup_body{
                        (popup_top_panel.x) + (pad * 1),
                        (popup_top_panel.y + popup_top_panel.height) - (pad * 0),
                        (popup_card.width) - (pad * 2),
                        (popup_card.height * 3 / 4) - (pad * 0)
                    };
                    DrawRectangleRounded(popup_body, 0.1F, 10, POPUP_BODY_COLOR);

                    // DRAW NAME : OLD TARGET
                    {
                        std::string cpp_text{};
                        if (size_name > 25) cpp_text = popup_name + "..  :  " + popup_old_target;
                        else cpp_text = popup_name + "  :  " + popup_old_target;

                        font = &font_s_bold;
                        font_color = BLACK;
                        const char* text = cpp_text.c_str();
                        float font_size = title_rect.height * 0.7F;
                        float font_space = -0.25F;
                        Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
                        Vector2 text_coor{
                            popup_body.x + (popup_body.width - text_measure.x) / 2,
                            popup_body.y + (55.0F - text_measure.y) / 2
                        };
                        DrawTextEx(*font, text, text_coor, font_size, font_space, font_color);
                    }

                    // DRAW Enter new target & INPUT BOX
                    float input_w = 140.0F;
                    float input_h = 37.5F;
                    Rectangle input_rect{
                        popup_body.x + (popup_body.width / 2),
                        popup_body.y + ((popup_body.height - input_h) / 2),
                        input_w,
                        input_h
                    };
                    DrawRectangleRec(input_rect, RAYWHITE);

                    {
                        std::string cpp_text = "Enter new target";
                        font = &font_s_reg;
                        font_color = BLACK;
                        const char* text = cpp_text.c_str();
                        float font_size = input_rect.height * 0.725F;
                        float font_space = 0.0F;
                        Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
                        Vector2 text_coor{
                            popup_body.x + 50.0F,
                            popup_body.y + (popup_body.height - text_measure.y) / 2
                        };
                        DrawTextEx(*font, text, text_coor, font_size, font_space, font_color);
                    }
                    
                    static bool mouse_on_card{ false };
                    static int frames_counter{ 0 };
                    int max_input_chars{ 4 };

                    if (CheckCollisionPointRec(mouse_position, input_rect)) {
                        SetMouseCursor(MOUSE_CURSOR_IBEAM);
                        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                            mouse_on_card = true;
                        }
                    }
                    else {
                        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
                    }

                    if (!CheckCollisionPointRec(mouse_position, input_rect)) {
                        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                            mouse_on_card = false;
                        }
                    }

                    if (mouse_on_card) {
                        frames_counter++;
                        int key = GetCharPressed();

                        while (key > 0) {
                            if ((key >= KEY_ZERO) && (key <= KEY_NINE) && (letter_size < max_input_chars)) {
                                input += static_cast<char>(key);
                            }
                            key = GetCharPressed();
                        }

                        if (IsKeyPressed(KEY_BACKSPACE)) {
                            if (letter_size > 0) input.pop_back();
                        }

                        if (IsKeyPressed(KEY_ENTER)) {
                            ApplyInputReset(input, popup_on, name, setting_on);
                        }

                    }
                    else {
                        frames_counter = 0;
                    }

                    {
                        // DRAW INPUT
                        font = &font_number;
                        const char* input_text = input.c_str();
                        float font_size_input = input_rect.height * 0.7F;
                        float font_space_input = 1.0F;
                        Vector2 input_measure = MeasureTextEx(*font, input_text, font_size_input, font_space_input);
                        Vector2 input_coor{
                            input_rect.x + 8,
                            input_rect.y + (input_rect.height - input_measure.y) / 2
                        };
                        DrawTextEx(*font, input_text, input_coor, font_size_input, font_space_input, font_color);
                        
                        if (mouse_on_card) {
                            DrawRectangleLinesEx(input_rect, 1.5F, POPUP_CARD_COLOR);
                        
                            //if (letter_size < max_input_chars) {
                                if (((frames_counter / 30) % 2) == 0) {
                                    // blinking cursor
                                    font = &font_number;
                                    const char* text = "|";
                                    float font_size = input_rect.height * 0.8F;
                                    float font_space = 0.0F;
                                    Vector2 text_measure = MeasureTextEx(*font, input_text, font_size_input, font_space_input);
                                    Vector2 text_coor{
                                        input_coor.x + (text_measure.x) - 2.0F,
                                        input_rect.y + ((input_rect.height - text_measure.y) / 2) - 4.35F
                                    };
                                    DrawTextEx(*font, text, text_coor, font_size, font_space, font_color);
                                }
                            //}
                        }
                    }

                    
                    // DRAW APPLY & CANCEL
                    float bottom_w = 140.0F;
                    float bottom_h = 30.0F;
                    Rectangle bottom_row{
                        popup_body.x + popup_body.width - bottom_w - 10,
                        popup_body.y + popup_body.height - bottom_h - 10,
                        bottom_w,
                        bottom_h
                    };
                    //DrawRectangleRec(bottom_row, DARKGRAY);

                    // BUTTON APPLY
                    float button_w = 65.0F;
                    float button_h = bottom_h;
                    Rectangle apply_rect{
                        bottom_row.x,
                        bottom_row.y,
                        button_w,
                        button_h
                    };
                    DrawRectangleRounded(apply_rect, 0.2F, 10, POPUP_APPLY_COLOR);
                    
                    {
                        font_color = RAYWHITE;
                        font_coef = 0.8F;
                        font = &font_s_reg;
                        if (CheckCollisionPointRec(mouse_position, apply_rect)) {
                            font_coef = 0.825F;
                            font_color = WHITE;
                            font = &font_s_semibold;
                            if (letter_size > 0) {
                                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                                    ApplyInputReset(input, popup_on, name, setting_on);
                                }

                            }
                        }

                        const char* text = "Apply";
                        float font_size = apply_rect.height * font_coef;
                        float font_space = 0.0F;
                        Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
                        Vector2 text_coor{
                            apply_rect.x + (apply_rect.width - text_measure.x) / 2,
                            apply_rect.y + (apply_rect.height - text_measure.y) / 2
                        };
                        DrawTextEx(*font, text, text_coor, font_size, font_space, font_color);
                    }


                    // BUTTON CANCEL
                    Rectangle cancel_rect{
                        bottom_row.x + bottom_row.width - button_w,
                        bottom_row.y,
                        button_w,
                        button_h
                    };
                    DrawRectangleRounded(cancel_rect, 0.2F, 10, POPUP_CANCEL_COLOR);

                    {
                        font_color = RAYWHITE;
                        font_coef = 0.8F;
                        font = &font_s_reg;
                        if (CheckCollisionPointRec(mouse_position, cancel_rect)) {
                            font_coef = 0.825F;
                            font_color = WHITE;
                            font = &font_s_semibold;
                            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                                popup_on = OFF;
                            }
                        }

                        const char* text = "Cancel";
                        float font_size = cancel_rect.height * font_coef;
                        float font_space = 0.0F;
                        Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
                        Vector2 text_coor{
                            cancel_rect.x + (cancel_rect.width - text_measure.x) / 2,
                            cancel_rect.y + (cancel_rect.height - text_measure.y) / 2
                        };
                        DrawTextEx(*font, text, text_coor, font_size, font_space, font_color);
                    }

                }
            } 

            if (setting_on == OFF) input.clear();
        }

        if (IsFileDropped()) {
            // TODO: put dialog. what is the target repetition value for this mp3.
            LoadMP3();
        }


        EndDrawing();
    }
    CloseWindow();
}

void LoadMP3()
{
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

        if (Save()) {
            TraceLog(LOG_INFO, "[SUCCESS] Save [%s] to data.txt", file_name.c_str());
            zero_data = false;
            ReloadVector();
            order = data.size() - 1;
            music = LoadMusicStream(data.at(order).path.c_str());
            p->reset_time = true;
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

void ApplyInputReset(std::string& input, bool& popup_on, std::string& name, bool& setting_on)
{
    int new_target = std::stoi(input);
    int old_target = data.at(order).target;
    data.at(order).target = new_target;
    popup_on = OFF;

    if (Save()) TraceLog(LOG_INFO, "[SUCCESS] Reset Target of [%s] from : [%d] to : [%d]", name.c_str(), old_target, new_target);

    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    // CLEAR
    if (popup_on == OFF) input.clear();

    setting_on = OFF;
}

bool Save()
{
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

        return true;
    }
    else return false;
}

void DrawPlayPause(const Rectangle& play_rect, const Rectangle& hover_panel)
{
    Color icon_color = LIGHTGRAY;
    if (CheckCollisionPointRec(mouse_position, hover_panel)) {
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
    float font_size = 70.0F;
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
