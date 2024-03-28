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

enum Page {
    PAGE_DRAG_DROP,
    PAGE_MAIN
};

struct Plug {
    int page{};
    int play{};
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


std::vector<Data> data{};
size_t data_size{};
size_t order{};
const std::filesystem::path data_dir{ "resources/Data" };
const std::filesystem::path data_txt{ "resources/Data/data.txt" };
bool zero_data{false};
Font* font = nullptr;
bool playing{ true };

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

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    //SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screen_w, screen_h, "Tirakat");
    InitAudioDevice();
    SetTargetFPS(60);
    //SetWindowIcon(LoadImage());

    Font font_m = LoadFontEx(FONT_LOC_Roboto_Slab, 60, 0, 0);
    SetTextureFilter(font_m.texture, TEXTURE_FILTER_BILINEAR);

    Font font_s = LoadFontEx(FONT_LOC_Source_Sans, 38, 0, 0);
    SetTextureFilter(font_s.texture, TEXTURE_FILTER_BILINEAR);

    Font font_number = LoadFontEx(FONT_LOC_Roboto_Mono, 40, 0, 0);
    SetTextureFilter(font_number.texture, TEXTURE_FILTER_BILINEAR);

    Font font_counter = LoadFontEx(FONT_LOC_Roboto_Mono, 100, 0, 0);
    SetTextureFilter(font_counter.texture, TEXTURE_FILTER_BILINEAR);

    FileCheck(data_txt);

    Music music{};

    
    if (zero_data == true) {
        p->page = PAGE_DRAG_DROP;
    }
    else {
        p->page = PAGE_MAIN;

        ReloadVector();

    }

    music = LoadMusicStream(data.at(order).path.c_str());
    if (IsMusicReady(music)) {
        SetMusicVolume(music, 0.15F);
        PlayMusicStream(music);
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

        Vector2 mouse_position = GetMousePosition();

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
            
            if (IsMusicReady(music)) {
                SetMusicVolume(music, 0.15F); 
                UpdateMusicStream(music);
                if (playing) {
                    PlayMusicStream(music);
                }
            }

            if (IsKeyPressed(KEY_SPACE)) {
                playing = !playing;
                if (playing == false) {
                    PauseMusicStream(music);
                }
                else {
                    ResumeMusicStream(music);
                }
            }


            // INFO PANEL
            float panel_info_h = 80.0F;
            float panel_progress_h = 15.0F;
            Rectangle panel_info{
                0,
                screen_h - panel_info_h,
                screen_w,
                panel_info_h - panel_progress_h
            };
            DrawRectangleRec(panel_info, PANEL_COLOR);

            float panel_w = 300.0F;
            Rectangle duration_rect{
                panel_info.x,
                panel_info.y,
                panel_w,
                panel_info.height
            };

            int duration = data.at(order).duration;
            static int progress = 0;
            progress = static_cast<int>(GetMusicTimePlayed(music) * 1000);
            std::ostringstream formatted_duration{};
            std::ostringstream formatted_progress{};

            if (duration < 3600) {
                int minutes_dur = duration / (60 * 1000);
                int seconds_dur = (duration / 1000) % 60;

                int minutes_pro = progress / (60 * 1000);
                int seconds_pro = (progress / 1000) % 60;

                formatted_duration << std::setw(2) << std::setfill('0') << minutes_dur << ":" << std::setw(2) << std::setfill('0') << seconds_dur;
                formatted_progress << std::setw(2) << std::setfill('0') << minutes_pro << ":" << std::setw(2) << std::setfill('0') << seconds_pro;
            }
            else {
                int hour_dur = duration / (3600 * 1000);
                int minutes_dur = (duration / (60 * 1000)) % 60;
                int seconds_dur = (duration / 1000) % 60;

                int hour_pro = progress / (3600 * 1000);
                int minutes_pro = (progress / (60 * 1000)) % 60;
                int seconds_pro = (progress / 1000) % 60;

                formatted_duration << std::setw(2) << std::setfill('0') << hour_dur << ":" 
                                   << std::setw(2) << std::setfill('0') << minutes_dur << ":" 
                                   << std::setw(2) << std::setfill('0') << seconds_dur;

                formatted_progress << std::setw(2) << std::setfill('0') << hour_pro << ":" 
                                   << std::setw(2) << std::setfill('0') << minutes_pro << ":" 
                                   << std::setw(2) << std::setfill('0') << seconds_pro;
            }

            {
                std::string duration_string = formatted_duration.str();
                std::string progress_string = formatted_progress.str();

                std::string cpp_text = progress_string + " / " + duration_string;
                font = &font_number;
                float font_size = duration_rect.height * 0.45F;
                float font_space = 0.5F;
                const char* text = cpp_text.c_str();
                Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
                Vector2 text_coor{
                    duration_rect.x + (duration_rect.width - text_measure.x) / 2,
                    duration_rect.y + (duration_rect.height - text_measure.y) / 2
                };
                DrawTextEx(*font, text, text_coor, font_size, font_space, RAYWHITE);
            }

            // LEFT PANEL
            Rectangle panel_left{
                0,
                0,
                panel_w,
                panel_info.y
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
                panel_info.y,
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
            //DrawRectangleRec(inner_panel_left, GRAY);

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
                    content_boundary.width - (content_panel_pad * 3),
                    content_boundary.height - (content_panel_pad * 2)
                };
                //DrawRectangleRec(content, GRAY);
                Color color_content = CONTENT_COLOR;
                Color color_font = RAYWHITE;
                if (CheckCollisionPointRec(mouse_position, content)) {
                    color_content = DARKGRAY;

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        order = i;
                        // Music Play Load
                        music = LoadMusicStream(data.at(order).path.c_str());
                        progress = 0;
                        playing = true;
                    }
                }
                if (i == order) {
                    color_content = CONTENT_CHOOSE_COLOR;
                    color_font = BLACK;
                }
                DrawRectangleRounded(content, 0.2F, 10, color_content);

                font = &font_s;
                float text_width_limit = content.width - 30.0F;
                float font_size = content_h * 0.45F;
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
                    content.x + 10.0F,
                    content.y + (content.height - text_measure.y) / 2,
                };
                DrawTextEx(*font, text, text_coor, font_size, font_space, color_font);

            }

            EndScissorMode();

            // PROGRESS PANEL
            Rectangle panel_progress{
                0,
                panel_info.y + panel_info.height,
                screen_w,
                panel_progress_h
            };
            DrawRectangleRec(panel_progress, PANEL_PROGRESS_BASE_COLOR);

            float progress_ratio = static_cast<float>(screen_w) / duration;
            float progress_w = progress_ratio * progress;
            Rectangle progress_bar{
                panel_progress.x,
                panel_progress.y,
                progress_w,
                panel_progress.height
            };
            DrawRectangleRec(progress_bar, PANEL_PROGRESS_COLOR);

            if (CheckCollisionPointRec(mouse_position, panel_progress)) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    float t = (mouse_position.x - panel_progress.x) / panel_progress.width;
                    SeekMusicStream(music, t * (duration / 1000));
                }
            }

            // MAIN PANEL
            Rectangle panel_main{
                panel_left.x + panel_left.width,
                0,
                screen_w - panel_left.width,
                screen_h - panel_info.height - panel_progress.height
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

                EndScissorMode();
            }

            {
                font = &font_counter;
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
