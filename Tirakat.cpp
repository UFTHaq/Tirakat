// Tirakat.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Author : Ukhem Fahmi Thoriqul Haq 
// Made : 31 - 03 - 2024
//

// MAYBE NEXT BIG TODO: ADD VISUALIZATION ?
// 1. FFT (FREQ DOMAIN) SIGNAL STYLE - DONE
// 2. TIME DOMAIN SIGNAL STYLE - DONE
// 3. ADD Functionality to can add multiple music at one time - DONE
// 4. IMITATE THIS THINGS: 
//    1. https://www.youtube.com/watch?v=SZzehktUeko. and go to 50 seconds positions
//    2. https://www.youtube.com/watch?v=LqUuMqfW1PE
//    3. maybe use drawspline, check if it can do good in 64 data. 
//       siapkan 5 array
//       ambil spline taruh di array0, ambil sample
//       jika telah 0.1s pindahkan ke array1, dan ambil array baru masukkan ke array0,
//       jika telah 0.1s pidahkan ke array sebelumnya,
//       jika telah 0.1s pidahkan ke array sebelumnya,
//       jika telah 0.1s pidahkan ke array sebelumnya,
//       jika telah 0.1s pidahkan ke array sebelumnya,
//       jadi dalam 1 detik ada 10 sample string. dan terus berganti dengan sample string spline yang baru.
//       string0 paling terang dan tebal, dan string4 paling redup dan tipis. coba dulu di raylib. jika jelek baru pikirkan custom shadernya nanti.
//       dan waktu pengambilan sample mungkin diubah ke 0.05s.
//       Mungkin dibuat dengan membuat class pada tiap kali waktunya sample dan memiliki method untuk menghitung waktu hidupnya 
//       dan menginterpolasi thickness dan alpha dengan inverse nilai waktu lifetime-nya.2
// 5. Coba untuk capture dan buat seperti cascade milik muzkaw tetapi tidak diagonal, melainkan kebelakang dan ditengah semakin kecil, naik dan kecilkan misal 95%, dan naik lagi 95%
//    Hingga membuat effect semakin jauh semakin kecil. mungkin bagus.
//    Masukkan ke sebuah struct mungkin, berisi ukuran rectangle dan posisi, data pointer spline, lalu posisi y dari spline, dan ukuran yang makin mengecil, coef yang makin jauh makin kecil dan selalu di kali 0.95, coef *= 0.95F 
//    sehingga ketinggian dan lebar tinggal menyesuaikan coef. dan juga tebal, dan alpha untuk splines bisa menggunakan coef tersebut. coef bisa digunakan untuk posisi rectangle.
//    0000a -> 000ab -> 00abc -> 0abcd -> abcde -> bcdef -> cdefg -> defgh -> dst. jika jumlah struct == batas, maka buang data paling tua, dan tambahkan data baru. 
// 
//    Perlu buat rectangle untuk tempat draw spline, mungkin 20 - 30 rect dengan posisi makin kecil makin jauh. masukkan ke dalam vector, atau array.
//    Jadi tinggal buat loop untuk para rectangle tadi, untuk titik spline tinggal ikuti bar_h tetapi dengan rect masing masing. yang urutan rect tetap, urutan data spline yang berubah seperti data diatas.
//    yang paling awal yang paling jauh, jadi jika ada data baru, buang data awal, dan append data baru. mungkin pake linked list lebih cepat dibanding vector? entahlah, nanti saja itu, untuk optimization.
// 
// 5. Extract Musical Note from Audio FFT
//    https://www.youtube.com/watch?v=rj9NOiFLxWA&t=369s, i think this good for FFT Output mode 1.
//  
// 7. library to write MIDI using code : Midifile by sapp.org : 
//    https://midifile.sapp.org/class/MidiFile/
//    https://github.com/craigsapp/midifile -> MIDI file writing example
//    
// 8. Dear ImGui Best tutorial to use in Visual Studio 2022: 
//    Part 1: https://www.youtube.com/watch?v=SP6Djf6ku1E
//    Part 2: https://www.youtube.com/watch?v=HivfFkhpLjE
//
// 9. Maybe consider to use JUCE? maybe not, maybe in the next project, maybe for MusMiBot?

// SMALL THINGS TODO:
// 1. FFT RESPON BUAT LEBIH BAIK:
//    - Jika data baru lebih tinggi dari data hasil moving average:
//      - maka update data terbaru, bukan data hasil moving average terbaru.
//    - Jika data baru lebih rendah dari data hasil moving average:
//      - tidak perlu di gambar.   // aku tidak tahu mana yang lebih baik.
//      - tidak perlu update. 
//    Agar menghasilkan sinyal gambar yang lebih responsive saat ada respon sinyal. 
//    Tetapi tidak berpindah naik dan turun berdasarkan data terbaru, melainkan turun seperti gravitasi dengan moving average.
// 
// 2. Seringkali FFT tidak tampil, mungkin attach terjadi ketika music belum siap, jadi perlu while loop dulu sampai siap lalu lanjut ke attach music.


// MAXIMUM MIDI FREQ -> G#9/Ab9 = 13289.75Hz

#include <iostream>
#include <filesystem>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
#include <array>
#include <string>
#include <cassert>
#include <algorithm>
#include <iomanip>
//#include <cmath>

#include <chrono>
#include <thread>

#include <raylib.h>
#include <rlgl.h>
#include <fftw3.h>
#include <SFML/Audio.hpp>

#define FONT_LOC_Roboto_Slab {"resources/Fonts/Roboto_Slab/static/RobotoSlab-Regular.ttf"}
#define FONT_LOC_Roboto_Mono {"resources/Fonts/Roboto_Mono/static/RobotoMono-SemiBold.ttf"}
#define FONT_LOC_Source_Sans_BOLD {"resources/Fonts/Source_Sans_3/static/SourceSans3-Bold.ttf"}
#define FONT_LOC_Source_Sans_SEMIBOLD {"resources/Fonts/Source_Sans_3/static/SourceSans3-SemiBold.ttf"}
#define FONT_LOC_Source_Sans_REG {"resources/Fonts/Source_Sans_3/static/SourceSans3-Regular.ttf"}

#define ICON_APP_LOC        {"resources/Icons/Tirakat-V4.png"}
#define ICON_PLAYPAUSE_LOC  {"resources/Icons/PlayPause.png"}
#define ICON_FULLSCREEN_LOC {"resources/Icons/Fullscreen.png"}
#define ICON_VOLUME_LOC     {"resources/Icons/Volume.png"}
#define ICON_SETTING_LOC    {"resources/Icons/Setting.png"}
#define ICON_X_LOC          {"resources/Icons/X.png"}
#define ICON_DELETE_LOC     {"resources/Icons/Trash.png"}
#define ICON_MODE_LOC       {"resources/Icons/Mode.png"}
//#define ICON_MODE_LOC       {"resources/Icons/Mode.png"}

#define HUD_TIMER_SECS 1.5F
#define PANEL_LEFT_WIDTH 275.0F
#define PANEL_DURATION_HEIGHT 40.0F
#define PANEL_DURATION_WIDTH PANEL_LEFT_WIDTH
#define PANEL_BOTTOM 50.0F
#define PANEL_MEDIA_HEIGHT PANEL_BOTTOM
#define PANEL_MEIDA_WIDTH PANEL_LEFT_WIDTH
#define PANEL_PROGRESS_HEIGHT PANEL_BOTTOM
#define PANEL_PROGRESS_HEIGHT_FULLSCREEN_OFFSCREEN 5.0F
#define PANEL_LINE_THICK 4.0F // 4.0F

#define BASE_COLOR                  Color{  20,  20,  20, 255 }
#define PANEL_COLOR                 Color{  30,  30,  30, 255 }
#define PANEL_LEFT_COLOR            Color{  40,  40,  40, 255 }
#define PANEL_COLOR2                Color{  30,  30,  30, 255 }
#define PANEL_LINE_COLOR            Color{  20,  20,  20, 255 }
#define PANEL_PROGRESS_BASE_COLOR   Color{  25,  25,  25, 255 }
#define PANEL_PROGRESS_COLOR        LIGHTGRAY

#define CONTENT_COLOR               Color{  60,  60,  60, 255 }
#define CONTENT_CHOOSE_COLOR        Color{ 150, 150, 150, 255 }

#define BLUE_BUTTON_COLOR           Color{  58,  76, 131, 255 }
#define POPUP_CARD_COLOR            Color{ 112, 141, 225, 255 }
#define POPUP_X_COLOR               Color{ 190,  60,  50, 255 }
#define POPUP_BODY_COLOR            Color{ 203, 209, 216, 255 }
#define POPUP_APPLY_COLOR           Color{ 75,  109, 214, 255 }
#define POPUP_CANCEL_COLOR          Color{ 142, 149, 178, 255 }
#define TARGET_DONE_COLOR           Color{  80, 180, 120, 255 }

enum Page {
    PAGE_DRAG_DROP,
    PAGE_MAIN
};

enum Drag {
    DRAG_MUSIC_PROGRESS,
    DRAG_VOLUME,
    DRAG_CONTENT,
    DRAG_RELEASE
};

enum Toggle {
    OFF,
    ON
};

enum MODE {
    MODE_NATURAL = 1,
    MODE_EXPONENTIAL,
    MODE_MULTI_PEAK,
    MODE_MAX_PEAK
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
    bool repeat{ OFF };
    int mouse_cursor{};
    bool glow{ false };
    int mode{ MODE_NATURAL };
    bool moving_save{ false };
    Shader circle{};
    Shader bubble{};
    int option_status = OFF;
    size_t option_music_order{};
    bool popup_on = false;
    std::string popup_title{};
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

struct ScreenSize {
    float w{};
    float h{};
};

struct Frame {
    float left{};
    float right{};
};

const int N{ 1 << 10 };
fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

const int BUCKETS{ 1 << 6 };
std::array<float, BUCKETS> Spectrum{};
std::array<float, BUCKETS + 1> Freq_Bin{};

const int SMOOTHING_BUFFER_SIZE{ 12 };
std::array<std::array<float, SMOOTHING_BUFFER_SIZE>, BUCKETS> prevAmplitude{};
std::array<float, BUCKETS> smoothedAmplitude{};
std::array<bool, BUCKETS> stronger{};
float maxAmplitude = 0.0F;

struct PeakInfo {
    int frequency_index{};
    float amplitude{};
};
std::array<PeakInfo, BUCKETS> Peak{};

void callback(void* bufferData, unsigned int frames) {
    if (frames > N) frames = N;

    Frame* fs = reinterpret_cast<Frame*>(bufferData);

    for (size_t i = 0; i < frames; i++) {
        float left = fs[i].left;
        float right = fs[i].right;

        in[i][0] = left;
        in[i][1] = 0.0F;
    }
}

void cleanup() {
    if (in != nullptr) {
        fftw_free(in);
    } 
    if (out != nullptr) {
        fftw_free(out);
    }
}

void dc_offset(fftw_complex in[]) {
    double dc_offset = 0.0F;
    for (int i = 0; i < N; i++) {
        dc_offset += in[i][0];
    }
    dc_offset = dc_offset / (float)N;

    for (int i = 0; i < N; i++) {
        in[i][0] -= dc_offset;
    }
}

void hann_window(fftw_complex in[], size_t n) {
    for (size_t i = 0; i < n; i++) {
        float w = 0.5F * (1.0F - cosf(2.0F * PI * i / (n - 1)));
        in[i][0] *= w;
    }
}

void fft_calculation(fftw_complex in[], fftw_complex out[], size_t n) {
    assert(n > 0);

    fftw_plan plan{};
    plan = fftw_plan_dft_1d(static_cast<int>(n), in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    fftw_execute(plan);

    fftw_destroy_plan(plan);
}

float natural_scale(float amplitude, float Fit_factor) {
    return amplitude * Fit_factor;
}

float exponential_scale(float amplitude, float Fit_factor) {
    //return std::log10(amplitude * Fit_factor) * Fit_factor;
    return std::log10(amplitude) * Fit_factor;
}

float multi_peak_scale(float amplitude, int i, float Fit_factor, const std::array<PeakInfo, BUCKETS>& Peak) {
    if (Peak.at(i).frequency_index >= 0) {
        if (i < 5) return amplitude / Peak.at(i).amplitude * Fit_factor * 1.3F; // untuk membuat drum bass pada awal bins lebih naik.
        else return amplitude / Peak.at(i).amplitude * Fit_factor;
    }
    else {
        return amplitude * Fit_factor;
    }
}

float max_peak_scale(float amplitude, float Global_Peak, float Fit_factor) {
    return amplitude / Global_Peak * Fit_factor;
}

float calculateMovingAverage(std::array<float, SMOOTHING_BUFFER_SIZE>& arr, int size) {
    float sum{};
    for (int i = 0; i < size; i++) {
        sum += arr.at(i);
    }
    return sum / size;
}

float min_frequency = 10.0F;
float max_frequency = 24000.0F;
float bin_width = (max_frequency - min_frequency) / BUCKETS;

float log_f_min = std::log10(min_frequency);
float log_f_max = std::log10(max_frequency);
float delta_log = (log_f_max - log_f_min) / BUCKETS;

void make_bins() {
    std::cout << std::fixed << std::setprecision(2);
    for (int i = 0; i <= BUCKETS; i++) {
        Freq_Bin.at(i) = min_frequency + i * bin_width;
        //Freq_Bin.at(i) = std::powf(10, log_f_min + i * delta_log);
        std::cout << Freq_Bin[i] << std::endl;
    }
}

float normalization(float val, float min_val, float max_val) {
    if (min_val == max_val) return val;

    return (val - min_val) / (max_val - min_val);
}


void InitFile(const std::filesystem::path& filename);

void FileCheck(const std::filesystem::path& filename);

void FileZeroDataCheck(const std::filesystem::path& filename);

int GetDuration(const char* c_file_path);

void ReloadVector();

void DrawTitleMP3(Rectangle& panel_main);

void DrawCounter(Rectangle& panel_main);

void DrawDuration(Rectangle& panel_duration);

void DrawPlayPause(const Rectangle& play_rect, const Rectangle& hover_panel);

void ApplyInputReset(std::string& input, bool& popup_on, std::string& name);

bool Save();

void LoadMP3();

void DrawMainPage(ScreenSize screen, int& retFlag);

void DrawPopUpReset(Rectangle& panel_main);

void DrawProgressTimeDomain(Rectangle& panel, float progress_w);

void DrawMedia(Rectangle& panel_media);

void DrawVolume(Rectangle& panel_playpause, float button_panel);

void DrawMusicList(Rectangle& panel_left, int& retFlag);

void DeleteMusic(int& retFlag, size_t order);

void ResetVisualizerParameter();

void DrawMainDisplay(Rectangle& panel_main);

void DrawFullscreenButton(Rectangle& panel_main, float dt);

void DrawMusicPlayMode(Rectangle& panel_main, float dt);

void DrawMusicProgress(Rectangle& panel_progress, float& music_volume);

bool is_Draw_Icons();

void DrawDragDropPage(ScreenSize screen);

bool Check_StartUp_Page();

static std::vector<float> ExtractMusicData(std::string& filename) {
    std::vector<float> audio_data{};

    // Load the entire audio for processing (modify for large files)
    sf::SoundBuffer soundBuffer{};
    soundBuffer.loadFromFile(filename);

    sf::Uint64 total_samples = soundBuffer.getSampleCount();
    audio_data.reserve(total_samples);

    const sf::Int16* samples = soundBuffer.getSamples();

    for (size_t i = 0; i < total_samples; i++) {
        // Convert and push back all samples (no downsampling, only normalization)
        float sample = static_cast<float>(samples[i]) / 32768.0F; // assuming 16-bit signed integer.
        audio_data.push_back(sample);
    }
    int total_frames = (int)audio_data.size();

    // Downsampling
    std::vector<float> processed_signal{};
    int downsampling_rate = int(total_frames / 9000);
    std::cout << "downsampling rate : " << downsampling_rate << std::endl;
    for (size_t i = 0; i < audio_data.size(); i += downsampling_rate) {
        float sample = audio_data.at(i);
        processed_signal.push_back(sample);
    }
    std::cout << "time domain size : " << processed_signal.size() << std::endl;

    float max_amp{ -1000 };
    float min_amp{ 1000 };
    for (size_t i = 0; i < processed_signal.size(); i++) {
        max_amp = std::max(max_amp, std::abs(processed_signal.at(i)));
        min_amp = std::min(min_amp, std::abs(processed_signal.at(i)));
    }

    std::cout << "max amp : " << max_amp << std::endl;
    std::cout << "min amp : " << min_amp << std::endl;

    // Normalization 0 - 1 && Scalling down little bit;
    for (size_t i = 0; i < processed_signal.size(); i++) {
        processed_signal.at(i) = (processed_signal.at(i) - min_amp) / (max_amp - min_amp) * 0.6F;
    }

    return processed_signal;

    // Catatan:
    // Jika input berupa file wav, perlu penguatan pada amplitude, sekitar 3 - 6 kali lipat.
}


Vector2 mouse_position{};
std::vector<Data> data{};
size_t data_size{};
size_t music_play{};
const std::filesystem::path data_dir{ "resources/Data" };
const std::filesystem::path data_txt{ "resources/Data/data.txt" };
bool zero_data{false};
Font* font = nullptr;

Font font_m{};
Font font_s_bold{};
Font font_s_semibold{};
Font font_s_reg{};
Font font_number{};
Font font_counter{};

Texture2D PLAYPAUSE_TEX{};
Texture2D FULLSCREEN_TEX{};
Texture2D VOLUME_TEX{};
Texture2D SETTING_TEX{};
Texture2D MODE_TEX{};
Texture2D X_TEX{};
Texture2D DELETE_TEX{};

std::ostringstream formatted_duration{};
std::ostringstream formatted_progress{};

int music_time{};
int duration{};
int time_played = 1;

Music music{};

static std::string input{};
size_t letter_size = input.size();

std::vector<float> time_domain_signal{};

int selected_index{};
Data selected_data{};
float y_while_selected{};
float delta_y_mouse_down{};
float delta_y_while_released{};
int moving{};
int moveup{};
int movedown{};

int content_preview{};

std::deque<std::vector<Vector2>> landscape_splines{};

int main()
{
    //landscape_splines.reserve(BUCKETS * 60);

    std::cout << std::setprecision(3);
    std::cout << "Hello World!\n";
    std::cout << "RAYLIB VERSION: " << RAYLIB_VERSION << std::endl;

    ScreenSize screen{ 1000, 580 };

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow((int)screen.w, (int)screen.h, "Tirakat");
    InitAudioDevice();
    SetTargetFPS(75);
    SetWindowIcon(LoadImage(ICON_APP_LOC));

    font_m = LoadFontEx(FONT_LOC_Roboto_Slab, 90, 0, 0);
    SetTextureFilter(font_m.texture, TEXTURE_FILTER_BILINEAR);
     
    font_s_bold = LoadFontEx(FONT_LOC_Source_Sans_BOLD, 64, 0, 0);
    SetTextureFilter(font_s_bold.texture, TEXTURE_FILTER_BILINEAR);

    font_s_semibold = LoadFontEx(FONT_LOC_Source_Sans_SEMIBOLD, 60, 0, 0);
    SetTextureFilter(font_s_semibold.texture, TEXTURE_FILTER_BILINEAR);

    font_s_reg = LoadFontEx(FONT_LOC_Source_Sans_REG, 48, 0, 0);
    SetTextureFilter(font_s_reg.texture, TEXTURE_FILTER_BILINEAR);

    font_number = LoadFontEx(FONT_LOC_Roboto_Mono, 52, 0, 0);
    SetTextureFilter(font_number.texture, TEXTURE_FILTER_BILINEAR);

    font_counter = LoadFontEx(FONT_LOC_Roboto_Mono, 50, 0, 0);
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

    Image mode_icon = LoadImage(ICON_MODE_LOC);
    MODE_TEX = LoadTextureFromImage(mode_icon);
    SetTextureFilter(MODE_TEX, TEXTURE_FILTER_BILINEAR);

    Image x_icon = LoadImage(ICON_X_LOC);
    X_TEX = LoadTextureFromImage(x_icon);
    SetTextureFilter(X_TEX, TEXTURE_FILTER_BILINEAR);

    Image delete_icon = LoadImage(ICON_DELETE_LOC);
    DELETE_TEX = LoadTextureFromImage(delete_icon);
    SetTextureFilter(DELETE_TEX, TEXTURE_FILTER_BILINEAR);

    p->circle = LoadShader(NULL, "resources/shaders/circle.fs");
    p->bubble = LoadShader(NULL, "resources/shaders/bubble.fs");

    FileCheck(data_txt);

    if (Check_StartUp_Page()) {
        ReloadVector();

        music = LoadMusicStream(data.at(music_play).path.c_str());
        time_domain_signal = ExtractMusicData(data.at(music_play).path);

        while (!IsMusicReady(music)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        PlayMusicStream(music);
        p->last_volume = GetMasterVolume();
        p->music_playing = true;
        AttachAudioStreamProcessor(music.stream, callback);

    }

    SetMasterVolume(0.1F);
    p->dragging = DRAG_RELEASE;

    make_bins();

    while (!WindowShouldClose()) {
        SetWindowMinSize(800, 450);

        if (zero_data == true) {
            p->page = PAGE_DRAG_DROP;
        }
        else {
            p->page = PAGE_MAIN;
        }

        if (IsFileDropped()) {
            LoadMP3();
        }


        BeginDrawing();
        ClearBackground(BASE_COLOR);

        mouse_position = GetMousePosition();

        screen.w = static_cast<float>(GetScreenWidth());
        screen.h = static_cast<float>(GetScreenHeight());

        font = &font_m;

        switch (p->page)
        {
        case PAGE_DRAG_DROP:

            DrawDragDropPage(screen);
            
            break;
        case PAGE_MAIN:
        
            int retFlag;
            DrawMainPage(screen, retFlag);
            if (retFlag == 2) break;

            break;
        default:
            break;
        }

        DrawFPS(screen.w - 83, 10);

        EndDrawing();
    }

    UnloadShader(p->circle);
    UnloadShader(p->bubble);
    
    cleanup();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

bool Check_StartUp_Page()
{
    if (zero_data == true) {
        p->page = PAGE_DRAG_DROP;

        return false;
    }
    else {
        p->page = PAGE_MAIN;

        return true;
    }
}

void DrawDragDropPage(ScreenSize screen)
{
    const char* text = "Drag&Drop MP3 Here";
    float font_size = 55;
    float font_space = 1;
    Color font_color = RAYWHITE;
    Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
    Vector2 text_coor = {
        (screen.w - text_measure.x) / 2,
        (screen.h - text_measure.y) / 2
    };
    DrawTextEx(*font, text, text_coor, font_size, font_space, font_color);
}

void DrawMainPage(ScreenSize screen, int& retFlag)
{
    retFlag = 1;

    if (p->mouse_cursor == MOUSE_CURSOR_POINTING_HAND) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }
    else if (p->mouse_cursor == MOUSE_CURSOR_IBEAM) {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
    }
    else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    duration = data.at(music_play).duration;

    if (p->reset_time == true) {
        SeekMusicStream(music, 0.1F);
        p->reset_time = false;
        p->music_playing = true;
        time_played = static_cast<int>(GetMusicTimePlayed(music) * 1000);

    }

    // INPUT KEYBOARD & MOUSE
    const float INTERVAL = 0.25F;
    static float interval_time = INTERVAL;
    if (IsKeyPressed(KEY_SPACE)) {
        p->music_playing = !p->music_playing;
    }
    else if (IsKeyDown(KEY_LEFT)) {

        if (interval_time > 0.0F) {
            interval_time -= GetFrameTime();
        }
        else {
            float music_move_to = (static_cast<float>(music_time) / 1000) - 5.0F;
            SeekMusicStream(music, music_move_to);
            std::cout << "LEFT ARROW : -5 SECONDS" << std::endl;
            interval_time = INTERVAL;
        }
        
    }
    else if (IsKeyDown(KEY_RIGHT)) {

        if (interval_time > 0.0F) {
            interval_time -= GetFrameTime();
        }
        else {
            float music_move_to = (static_cast<float>(music_time) / 1000) + 5.0F;
            SeekMusicStream(music, music_move_to);
            std::cout << "RIGHT ARROW : +5 SECONDS" << std::endl;
            interval_time = INTERVAL;
        }
    }
    else if (IsKeyPressed(KEY_LEFT)) {
        float music_move_to = (static_cast<float>(music_time) / 1000) - 5.0F;
        SeekMusicStream(music, music_move_to);
        std::cout << "LEFT ARROW : -5 SECONDS" << std::endl;
    }
    else if (IsKeyPressed(KEY_RIGHT)) {
        float music_move_to = (static_cast<float>(music_time) / 1000) + 5.0F;
        SeekMusicStream(music, music_move_to);
        std::cout << "RIGHT ARROW : +5 SECONDS" << std::endl;
    }
    else {
        interval_time = 0;
    }

    static float music_volume = 0.5F;
    if (music_volume < 0.5F) music_volume += 0.02F;

    if (IsMusicReady(music)) {
        SetMusicVolume(music, music_volume);
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

        int threshold_80 = static_cast<int>(duration * 0.8F);
        //std::cout << time_played << " : " << music_time << std::endl;

        // COUNTING REPETITION
        static bool repetition_saved = false;
        if (repetition_saved == false) {

            if (music_time >= (duration - 100)) {

                if (time_played >= threshold_80) {
                    data.at(music_play).counter++;
                    Save();
                    repetition_saved = true;
                    int data_counter = data.at(music_play).counter;
                    TraceLog(LOG_INFO, "[SUCCESS] Counter++ [%s] from : [%d] to : [%d]", data.at(music_play).name.c_str(), (data_counter - 1), data_counter);
                }

                if (p->repeat == OFF) {

                    if (music_play == data.size() - 1) music_play = 0;
                    else music_play++;

                    DetachAudioStreamProcessor(music.stream, callback);
                    ResetVisualizerParameter();
                    music = LoadMusicStream(data.at(music_play).path.c_str());
                    AttachAudioStreamProcessor(music.stream, callback);

                    time_domain_signal = ExtractMusicData(data.at(music_play).path);
                }

                p->reset_time = true;
                repetition_saved = false;
            }

        }

        time_played += static_cast<int>(GetFrameTime() * 1000);
    }
    else {
        p->icon_pp_index = false;
        PauseMusicStream(music);
        need_resume = true;
    }


    if (p->dragging != DRAG_MUSIC_PROGRESS) {
        music_time = static_cast<int>(GetMusicTimePlayed(music) * 1000);
    }

    Rectangle panel_media{};
    Rectangle panel_horizontal_line{};
    Rectangle panel_duration{};
    Rectangle panel_left{};
    Rectangle panel_vertical_line{};
    Rectangle panel_main{};
    Rectangle panel_progress{};

    // BARU
    
    // FULLSCREEN
    if (p->fullscreen) {

        static float mouse_onscreen_timer = HUD_TIMER_SECS;

        if (mouse_onscreen_timer > 0.0F) {
            mouse_onscreen_timer -= GetFrameTime();
            p->mouse_onscreen = ON;
        }
        else {
            p->mouse_onscreen = OFF;
        }

        Vector2 delta = GetMouseDelta();
        bool moved = fabsf(delta.x) + fabsf(delta.y) > 0.0;
        if (moved) mouse_onscreen_timer = HUD_TIMER_SECS;

        float panel_progress_height = PANEL_PROGRESS_HEIGHT;
        if (p->mouse_onscreen == OFF) panel_progress_height = 3.0F;

        // PANEL MAIN
        panel_main = {
            0,
            0,
            screen.w,
            screen.h - panel_progress_height - PANEL_LINE_THICK
        };

        // PANEL PROGRESS
        panel_progress = {
            panel_main.x,
            screen.h - panel_progress_height,
            panel_main.width,
            panel_progress_height,
        };
        //DrawRectangleRec(panel_progress, PANEL_COLOR);
        DrawRectangleRec(panel_progress, PANEL_PROGRESS_BASE_COLOR);

        if (p->mouse_onscreen == ON) {
            float pad_time_domain = 7.5F;
            panel_progress = {
                panel_progress.x + (pad_time_domain * 1),
                panel_progress.y,
                panel_progress.width - (pad_time_domain * 2),
                panel_progress.height
            };
        }

        // PANEL HORIZONTAL LINE
        panel_horizontal_line = {
            0,
            panel_progress.y - PANEL_LINE_THICK,
            screen.w,
            PANEL_LINE_THICK
        };
        DrawRectangleRec(panel_horizontal_line, PANEL_LINE_COLOR);

        //// BLOCK DRAWING

        // DRAWING PANEL MAIN
        DrawMainDisplay(panel_main);

        // DRAWING PANEL MUSIC PROGRESS
        DrawMusicProgress(panel_progress, music_volume);

        // DRAWING POPUP RESET
        if (p->popup_on) {
            DrawPopUpReset(panel_main);
        }

    }


    // NOT FULLSCREEN
    if (!p->fullscreen) {

        // PANEL MEDIA
        panel_media = {
            0,
            screen.h - PANEL_MEDIA_HEIGHT,
            PANEL_MEIDA_WIDTH,
            PANEL_MEDIA_HEIGHT
        };
        DrawRectangleRec(panel_media, PANEL_COLOR);

        float pad_duration = 15.0F;
        Rectangle panel_media_draw = {
            panel_media.x + (pad_duration * 1),
            panel_media.y + (pad_duration * 0.25F * 1),
            panel_media.width - (pad_duration * 2),
            panel_media.height - (pad_duration * 0.25F * 2),
        };

        // PANEL HORIZONTAL LINE
        panel_horizontal_line = {
            0,
            panel_media.y - PANEL_LINE_THICK,
            screen.w,
            PANEL_LINE_THICK
        };
        DrawRectangleRec(panel_horizontal_line, PANEL_LINE_COLOR);

        // PANEL DURATION
        panel_duration = {
            0,
            panel_horizontal_line.y - PANEL_DURATION_HEIGHT,
            PANEL_DURATION_WIDTH,
            PANEL_DURATION_HEIGHT
        };
        DrawRectangleRec(panel_duration, PANEL_COLOR);

        // PANEL LEFT
        panel_left = {
            0,
            0,
            PANEL_LEFT_WIDTH,
            screen.h - panel_media.height - panel_horizontal_line.height - panel_duration.height
        };
        DrawRectangleRec(panel_left, PANEL_LEFT_COLOR);

        // PANEL MUSIC LIST
        float top_bottom_pad{ 53.0F * 0.10F };
        Rectangle panel_music_list{
            panel_left.x,
            panel_left.y + (top_bottom_pad * 1 + 1),
            panel_left.width,
            panel_left.height - (top_bottom_pad * 2 + 1)
        };
        DrawRectangleRec(panel_music_list, PANEL_LEFT_COLOR);

        // PANEL VERTICAL LINE
        panel_vertical_line = {
            panel_left.x + panel_left.width,
            panel_left.y,
            PANEL_LINE_THICK,
            screen.h
        };
        DrawRectangleRec(panel_vertical_line, PANEL_LINE_COLOR);

        // PANEL MAIN
        panel_main = {
            panel_vertical_line.x + panel_vertical_line.width,
            0,
            screen.w - (panel_vertical_line.x + panel_vertical_line.width),
            screen.h - PANEL_PROGRESS_HEIGHT - PANEL_LINE_THICK
        };

        // PANEL PROGRESS
        panel_progress = {
            panel_main.x,
            screen.h - PANEL_PROGRESS_HEIGHT,
            panel_main.width,
            PANEL_PROGRESS_HEIGHT,
        };
        //DrawRectangleRec(panel_progress, PANEL_COLOR);
        DrawRectangleRec(panel_progress, PANEL_PROGRESS_BASE_COLOR);

        float pad_time_domain = 7.5F;
        panel_progress = {
            panel_progress.x + (pad_time_domain * 1),
            panel_progress.y,
            panel_progress.width - (pad_time_domain * 2),
            panel_progress.height
        };

        //// BLOCK DRAWING
        
        // DRAWING PANEL MAIN
        DrawMainDisplay(panel_main);

        // DRAWING PANEL MUSIC LIST
        DrawMusicList(panel_music_list, retFlag);

        // DRAWING PANEL DURATION
        DrawDuration(panel_duration);

        // DRAWING PANEL MEDIA
        DrawMedia(panel_media_draw);

        // DRAWING PANEL MUSIC PROGRESS
        DrawMusicProgress(panel_progress, music_volume);

        // DRAWING POPUP RESET
        if (p->popup_on) {
            DrawPopUpReset(panel_main);
        }


    }


}

void DrawPopUpReset(Rectangle& panel_main)
{
    p->popup_title = "Reset Target";
    Color font_color{};
    Color icon_color{};
    float font_coef{};

    // DRAW POPUP RESET TARGET
    std::string name{ data.at(music_play).name };
    size_t size_name = name.size();
    std::string popup_name{ data.at(music_play).name.substr(0, 25) };
    std::string popup_old_target{ std::to_string(data.at(music_play).target) };

    float popup_w = 360.0F;
    float popup_h = popup_w / 2;
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
        const char* text = p->popup_title.c_str();
        float font_size = title_rect.height * 0.7F;
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
            p->popup_on = OFF;
            input.clear();
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
        float font_size = title_rect.height * 0.725F;
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
            popup_body.x + (popup_w / 10),
            popup_body.y + (popup_body.height - text_measure.y) / 2
        };
        DrawTextEx(*font, text, text_coor, font_size, font_space, font_color);
    }

    static bool mouse_on_card{ false };
    static int frames_counter{ 0 };
    int max_input_chars{ 4 };

    if (CheckCollisionPointRec(mouse_position, input_rect)) {
        p->mouse_cursor = MOUSE_CURSOR_IBEAM;
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            mouse_on_card = true;
        }
    }
    else {
        p->mouse_cursor = MOUSE_CURSOR_DEFAULT;
    }

    if (!CheckCollisionPointRec(mouse_position, input_rect)) {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            mouse_on_card = false;
        }
    }

    letter_size = input.size();
    if (mouse_on_card) {
        frames_counter++;
        int key = GetCharPressed();

        while (key > 0) {
            if ((key >= KEY_ZERO) && (key <= KEY_NINE) && (letter_size < max_input_chars)) {
                input += static_cast<char>(key);
            }
            key = GetCharPressed();
        }

        if (letter_size > 0) {
            if (IsKeyPressed(KEY_BACKSPACE)) {
                input.pop_back();
            }

            if (IsKeyPressed(KEY_ENTER)) {
                ApplyInputReset(input, p->popup_on, name);
            }
        }
        else {
            if (IsKeyPressed(KEY_ENTER)) {
                p->popup_on = OFF;
            }
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
                    ApplyInputReset(input, p->popup_on, name);
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
                p->popup_on = OFF;
                input.clear();
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

void DrawProgressTimeDomain(Rectangle& panel, float progress_w)
{
    float progress = progress_w + panel.x;
    Color color = DARKGRAY;
    float alpha = 0.5F;

    // DRAW GRAY LINE
    DrawLineEx(
        { panel.x, panel.y + (panel.height * 0.5F) },
        { panel.x + panel.width, panel.y + (panel.height * 0.5F) },
        2.0F,
        Fade(color, alpha)
    );

    // DRAW LINE
    DrawLineEx(
        { panel.x, panel.y + (panel.height * 0.5F) },
        { progress - 0.5F, panel.y + (panel.height * 0.5F) },
        2.0F,
        { 225, 225, 225, 255 }
    );

    float segments = panel.width / (float)time_domain_signal.size();
    float center = panel.y + (panel.height * 0.5F);
    for (size_t i = 0; i < time_domain_signal.size(); i++) {
    
        float x1 = panel.x + i * segments;
        float y1 = center - time_domain_signal.at(i) * panel.height * 0.5F;
        float x2 = panel.x + i * segments;
        float y2 = center - (-time_domain_signal.at(i)) * panel.height * 0.5F;
    
        if (x1 < progress) { 
            //color = SKYBLUE;
            color = { 225, 225, 225, 255 };
            alpha = 1.0F;
        }
        else {
            color = DARKGRAY;
            alpha = 0.8F;
        }
    
        DrawLineEx({ x1,y1 }, { x2,y2 }, 1.75F, Fade(color, alpha));
    }

    if (CheckCollisionPointRec(mouse_position, panel)) p->mouse_cursor = MOUSE_CURSOR_POINTING_HAND;
    else p->mouse_cursor = MOUSE_CURSOR_DEFAULT;

}

void DrawMedia(Rectangle& panel_media)
{
    // PLAY PAUSE BUTTON
    float button_panel = panel_media.height;
    float pad = 3.0F;
    Rectangle panel_playpause{
        panel_media.x,
        panel_media.y,
        button_panel,
        button_panel
    };
    Rectangle button_playpause{
        panel_playpause.x + (pad * 1),
        panel_playpause.y + (pad * 1),
        panel_playpause.width - (pad * 2),
        panel_playpause.height - (pad * 2)
    };
    DrawPlayPause(button_playpause, panel_playpause);

    // VOLUME BUTTON
    DrawVolume(panel_playpause, button_panel);
}

void DrawVolume(Rectangle& panel_playpause, float button_panel)
{
    //static bool HUD_toggle = false;
    static bool HUD_toggle = true;

    float volume_slider_length_base = 150.0F;
    Rectangle panel_volume_base{
        panel_playpause.x + panel_playpause.width,
        panel_playpause.y,
        panel_playpause.width + volume_slider_length_base,
        panel_playpause.height
    };
    //DrawRectangleRec(panel_volume_base, RED);

    Rectangle panel_volume{
        panel_volume_base.x,
        panel_volume_base.y,
        button_panel,
        button_panel
    };

    float pad = 5.0F;
    Rectangle button_volume{
        panel_volume.x + (pad * 1),
        panel_volume.y + (pad * 1),
        panel_volume.width - (pad * 2),
        panel_volume.height - (pad * 2),
    };

    float volume = GetMasterVolume();
    Color icon_color = GRAY;
    if (CheckCollisionPointRec(mouse_position, panel_volume)) {
        icon_color = RAYWHITE;
        HUD_toggle = true;
        if (CheckCollisionPointRec(mouse_position, panel_volume) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
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
        Rectangle dest = button_volume;
        Rectangle source{ icon_index * icon_size, 0, icon_size, icon_size };
        DrawTexturePro(VOLUME_TEX, source, dest, { 0,0 }, 0, icon_color);
    }

    // VOLUME SLIDER
    static bool drag_volume = false;
    if (HUD_toggle) {
        // make new rect
        Rectangle volume_slider_panel{
            panel_volume.x + panel_volume.width,
            panel_volume.y,
            volume_slider_length_base,
            panel_volume.height
        };
        //DrawRectangleRec(volume_slider_panel, DARKBROWN);

        // SLIDER DRAW - START
        float padding_slider = 20.0F;
        float volume_slider_w = volume_slider_length_base - padding_slider;
        float volume_slider_h = button_panel * 0.15F;
        float vol_ratio = static_cast<float>(volume_slider_w) / 1;
        float vol_length = vol_ratio * GetMasterVolume();

        // OUTLINE SLIDER
        Rectangle volume_slider_outer{
            volume_slider_panel.x + (padding_slider * 0.5F),
            volume_slider_panel.y + (volume_slider_panel.height - volume_slider_h) / 2,
            static_cast<float>(volume_slider_w),
            volume_slider_h
        };
        DrawRectangleRounded(volume_slider_outer, 0.7F, 5, BASE_COLOR);
        DrawRectangleRoundedLines(volume_slider_outer, 0.7F, 5, 3.0F, BASE_COLOR);

        Rectangle volume_slider{
            volume_slider_panel.x + (padding_slider * 0.5F),
            volume_slider_panel.y + (volume_slider_panel.height - volume_slider_h) / 2,
            vol_length,
            volume_slider_h
        };
        DrawRectangleRounded(volume_slider, 0.7F, 5, GRAY);

        // DRAW CIRCLE
        int circle_center_x = static_cast<int>(volume_slider.x) + static_cast<int>(volume_slider.width);
        int circle_center_y = static_cast<int>(volume_slider.y) + static_cast<int>(volume_slider.height / 2) + 1;
        float radius = 6.0F;
        DrawCircle(circle_center_x, circle_center_y, radius + 5, BASE_COLOR);
        DrawCircle(circle_center_x, circle_center_y, radius, LIGHTGRAY);
        // SLIDER DRAW - END


        // DRAG
        bool inSlider = (CheckCollisionPointRec(mouse_position, volume_slider_panel));
        if (inSlider) {

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                vol_length = mouse_position.x - volume_slider.x;
                if (vol_length < 0) vol_length = 0;
                if (vol_length > volume_slider_w) vol_length = static_cast<float>(volume_slider_w);
                volume = vol_length / vol_ratio;
                SetMasterVolume(volume);

                //std::cout << vol_length << " : " << volume << std::endl;

            }
            else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                if (p->dragging != DRAG_MUSIC_PROGRESS) {
                    p->dragging = DRAG_VOLUME;
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

        if (p->dragging == DRAG_VOLUME) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                vol_length = mouse_position.x - volume_slider.x;
                if (vol_length < 0) vol_length = 0;
                if (vol_length > volume_slider_w) vol_length = volume_slider_w;
                volume = vol_length / vol_ratio;
                SetMasterVolume(volume);

                //std::cout << vol_length << " : " << volume << std::endl;
            }
            else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                p->dragging = DRAG_RELEASE;
            }
        }

        // MASIH NGEBUG - PERLU PENYELIDIKAN LEBIH LANJUT - Mungkin perlu di taruh di mainPage function bukan disini
        //if (inSlider) p->mouse_cursor = MOUSE_CURSOR_POINTING_HAND;
        //else p->mouse_cursor = MOUSE_CURSOR_DEFAULT;
        

    }

    // Mungkin perlu di hapus, tidak diperlukan lagi di versi ini
    // OUTSIDE OF HUD
    bool outVolumeBase = !(CheckCollisionPointRec(mouse_position, panel_volume_base));
    if (outVolumeBase) {
        //HUD_toggle = false;
    }

    if (p->dragging == DRAG_VOLUME) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && outVolumeBase) {
            HUD_toggle = true;
        }
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && outVolumeBase) {
            HUD_toggle = false;
            p->dragging = DRAG_RELEASE;
        }
    }
}

void DrawMusicList(Rectangle& panel, int& retFlag)
{
    // INNER PANEL LEFT
    Rectangle panel_list_boundary{
        panel.x,
        panel.y,
        panel.width * 0.940F,
        panel.height
    };
    //DrawRectangleRec(inner_panel_left_boundary, GRAY);

    float content_h = 53.0F;
    float content_panel_pad = content_h * 0.075F;
    float visible_area_size = panel.height;
    float entire_scrollable_area = content_h * data.size();

    static float content_scroll = 0;
    static float content_velocity = 0;
    content_velocity *= 0.9F;

    if (panel_list_boundary.height > entire_scrollable_area) panel_list_boundary.width = panel.width * 0.980F;
    else panel_list_boundary.width = panel.width * 0.940F;

    Rectangle content_boundary{
        panel_list_boundary.x,
        panel_list_boundary.y,
        panel_list_boundary.width,
        content_h
    };

    if (CheckCollisionPointRec(mouse_position, panel)) {
        content_velocity += GetMouseWheelMove() * data.size() * (content_h * 3 / 4);
    }
    content_scroll -= content_velocity * GetFrameTime();

    static bool scrolling = false;
    static float scrolling_mouse_offset = 0.0F;
    if (scrolling) {
        content_scroll = (mouse_position.y - panel.y - scrolling_mouse_offset) / visible_area_size * entire_scrollable_area;
    }

    float min_scroll = 0;
    if (content_scroll < min_scroll) content_scroll = min_scroll;
    float max_scroll = entire_scrollable_area - visible_area_size;
    if (max_scroll < 0) max_scroll = 0;
    if (content_scroll > max_scroll) content_scroll = max_scroll;

    bool scrollable = entire_scrollable_area > visible_area_size;

    BeginScissorMode(
        static_cast<int>(panel_list_boundary.x),
        static_cast<int>(panel_list_boundary.y - 2),
        static_cast<int>(panel_list_boundary.width),
        static_cast<int>(panel_list_boundary.height + 4)
    );

    Rectangle moving_boundary{
        content_boundary.x,
        content_boundary.y,
        content_boundary.width,
        content_boundary.height * data.size()
    };

    float move_info_w = content_boundary.height;
    float move_info_h = move_info_w;
    Rectangle move_info{
        panel_list_boundary.x + panel_list_boundary.width - (move_info_w * 1.5F),
        panel_list_boundary.y + (content_panel_pad * 1),
        (move_info_w * 1.5F) - (content_panel_pad * 1),
        move_info_h - (content_panel_pad * 2)
    };

    if (p->moving_save) {
        if (Save()) TraceLog(LOG_INFO, "Success Rearrange Play List");
        p->moving_save = false;
    }
    
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

        if (CheckCollisionPointRec(mouse_position, panel_list_boundary)) {

            static bool clicked_in_moving_boundary{};
            static bool hold{ false };  // LITTLE BUG OF HOLD WHENEVER RELEASE THE BUTTON, NEEDS TO MAKE IT FALSE, TO MAKE COLOR HOVER WORKS
            static std::chrono::time_point<std::chrono::steady_clock> content_press_start{};
            int double_click_threshold = 200;

            if (p->dragging == DRAG_RELEASE) {
                if (CheckCollisionPointRec(mouse_position, content)) {
                    if (!hold) color_content = DARKGRAY;

                    static bool double_click_detected{ false };
                    static clock_t last_click_time{ 0 };

                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        clock_t current_time = clock();
                        double time_difference = difftime(current_time, last_click_time);

                        if (time_difference < double_click_threshold && !double_click_detected) {
                            double_click_detected = true;

                            DetachAudioStreamProcessor(music.stream, callback);
                            ResetVisualizerParameter();

                            music_play = i;
                            music = LoadMusicStream(data.at(music_play).path.c_str());
                            time_domain_signal = ExtractMusicData(data.at(music_play).path);

                            AttachAudioStreamProcessor(music.stream, callback);

                            p->reset_time = true;
                        }
                        else {
                            double_click_detected = false;
                            // Optional : Handle single click event if i want
                            content_preview = int(i);
                        }
                        last_click_time = current_time;
                    }
                    else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                        p->option_status = ON;
                        p->option_music_order = i;
                    }
                    else {
                        double_click_detected = false;
                    }
                }


                if (CheckCollisionPointRec(mouse_position, moving_boundary)) {

                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        if (CheckCollisionPointRec(mouse_position, content)) {
                            //color_content = BLACK;
                            selected_index = int(i);
                            selected_data = data.at(selected_index);
                            y_while_selected = content.y + (content.height * 0.5F) + content_scroll;
                            clicked_in_moving_boundary = true;

                            content_press_start = std::chrono::steady_clock::now();
                        }
                    }
                    else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && clicked_in_moving_boundary) {
                        if (hold == false) {
                            auto current_time = std::chrono::steady_clock::now();
                            auto elapsed_time = current_time - content_press_start;
                            auto miliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count();

                            if (miliseconds > double_click_threshold) hold = true;
                        }

                        if (hold == true) {

                            if (selected_index == i) {
                                color_content = ORANGE;
                            }

                            delta_y_mouse_down = (y_while_selected - content_scroll) - mouse_position.y;
                            int moving = int(delta_y_mouse_down / content_boundary.height);

                            std::string text = std::to_string(moving);
                            float font_size = content.height * 0.65F;
                            //Font font = font_number;
                            Font font = font_counter;
                            Vector2 text_measure = MeasureTextEx(font, text.c_str(), font_size, 0);
                            Vector2 text_coor = {
                                move_info.x + (move_info.width - text_measure.x) / 2,
                                move_info.y + (move_info.height - text_measure.y) / 2
                            };

                            float line_y{};
                            if (moving > 0) {
                                //DrawRectangleRec(move_info, Fade(RAYWHITE, 0.1F));
                                DrawRectangleRounded(move_info, 0.25, 10, Fade(RAYWHITE, 0.1F));
                                DrawTextEx(font, text.c_str(), text_coor, font_size, 0, BLACK);
                                line_y = (y_while_selected - content_scroll) - (content_boundary.height * 0.5F) - (moving * content_boundary.height);

                            }
                            else if (moving < 0) {
                                //DrawRectangleRec(move_info, Fade(RAYWHITE, 0.1F));
                                DrawRectangleRounded(move_info, 0.25, 10, Fade(RAYWHITE, 0.1F));
                                DrawTextEx(font, text.c_str(), text_coor, font_size, 0, BLACK);
                                line_y = (y_while_selected - content_scroll) + (content_boundary.height * 0.5F) - (moving * content_boundary.height);

                            }
                            Vector2 start = { content.x + (content.width * 0.05F), line_y };
                            Vector2 end = { content.x + content.width - (content.width * 0.05F), line_y };
                            DrawLineEx(start, end, 2.0, WHITE);

                            if (scrollable) {

                                float auto_scroll_coef = 0.075F;
                                if ((mouse_position.y < panel_list_boundary.height * (auto_scroll_coef)) && (content_scroll > min_scroll)) {
                                    content_velocity += 1;
                                }
                                else if ((mouse_position.y > panel_list_boundary.height * (1 - auto_scroll_coef)) && (content_scroll < max_scroll)) {
                                    content_velocity -= 1;
                                }

                            }

                        }

                    }
                    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        clicked_in_moving_boundary = false;
                        hold = false;
                        delta_y_while_released = delta_y_mouse_down;

                        if (std::abs(int(delta_y_while_released / content_boundary.height)) == 0) {
                            p->moving_save = false;
                        }
                        else if (delta_y_while_released > 0) {
                            moveup = int(delta_y_while_released / content_boundary.height);
                            if (moveup) {
                                data.insert(data.begin() + selected_index - (moveup), selected_data);
                                data.erase(data.begin() + selected_index + 1);
                            }
                            p->moving_save = true;

                            // RE SET the playing now order
                            if (selected_index > music_play && music_play >= (selected_index - moveup)) {
                                music_play += 1;
                            }
                            else if (music_play == selected_index) {
                                music_play = selected_index - moveup;
                            }
                        }
                        else if (delta_y_while_released < 0) {
                            movedown = std::abs(int(delta_y_while_released / content_boundary.height));
                            if (movedown) {
                                data.insert(data.begin() + selected_index + (movedown + 1), selected_data);
                                data.erase(data.begin() + selected_index);
                            }
                            p->moving_save = true;

                            if (selected_index < music_play && music_play <= (selected_index + movedown)) {
                                music_play -= 1;
                            }
                            else if (music_play == selected_index) {
                                music_play = selected_index + movedown;
                            }
                        }

                        y_while_selected = 0;
                        delta_y_mouse_down = 0;
                        delta_y_while_released = 0;

                        if (p->moving_save) if (Save()) TraceLog(LOG_INFO, "Saving New Arranged Playlists in data.txt");
                    
                    }

                }
            }

        }

        if (i == music_play) {
            color_content = CONTENT_CHOOSE_COLOR;
            color_font = BLACK;
        }

        if (p->option_status == ON) {
            if (i == p->option_music_order) {
                //color_content = ORANGE;
                color_content = { 190, 76, 45, 255 };
                //color_content = { 105, 220, 57, 127 };
            }
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


        if (p->option_status == ON) {

            static float time_down = {};
            float dt = GetFrameTime();

            if (i == p->option_music_order) {
                float option_panel_width = content.width * 0.325F;
                Rectangle option_panel{
                    content.x + content.width - option_panel_width,
                    content.y,
                    option_panel_width,
                    content.height
                };
                DrawRectangleRounded(option_panel, 0.2F, 10, LIGHTGRAY);
                float btn_base_width = option_panel_width * 0.85F;
                Rectangle btn_base{
                    option_panel.x + (option_panel.width - btn_base_width) / 2,
                    option_panel.y,
                    btn_base_width,
                    option_panel.height
                };
                //DrawRectangleRec(btn_base, GRAY);

                float btn_width = btn_base_width * 0.475F;
                float y_pos = btn_base.y;
                if (btn_width < btn_base.height) {
                    float pad = btn_base.height - btn_width;
                    y_pos = btn_base.y + (pad / 2);
                }
                else btn_width = btn_base.height;

                Rectangle setting_btn{
                    btn_base.x,
                    y_pos,
                    btn_width,
                    btn_width
                };
                Rectangle delete_btn{
                    btn_base.x + btn_base_width - btn_width,
                    y_pos,
                    btn_width,
                    btn_width
                };


                if (CheckCollisionPointRec(mouse_position, setting_btn)) {
                    DrawRectangleRounded(setting_btn, 0.2F, 10, Fade(WHITE, 0.75F));
                }

                if (CheckCollisionPointRec(mouse_position, delete_btn)) {
                    DrawRectangleRounded(delete_btn, 0.2F, 10, Fade(WHITE, 0.75F));
                }


                // DRAW SETTING ICON
                {
                    float icon_size = 100.0F;
                    Rectangle dest = setting_btn;
                    Rectangle source = { 0, 0, icon_size, icon_size };
                    DrawTexturePro(SETTING_TEX, source, dest, { 0,0 }, 0, BLACK);
                }
                
                // DRAW DELETE ICON
                {
                    float icon_size = 100.0F;
                    Rectangle dest = delete_btn;
                    Rectangle source = { 0, 0, icon_size, icon_size };
                    DrawTexturePro(DELETE_TEX, source, dest, { 0,0 }, 0, color_content);
                }


                if (CheckCollisionPointRec(mouse_position, setting_btn)) {
                    time_down = 0.5;
                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        p->popup_on = ON;
                    }
                } 

                if (CheckCollisionPointRec(mouse_position, delete_btn)) {
                    time_down = 0.5F;
                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {

                        DeleteMusic(retFlag, p->option_music_order);
                        p->option_status = OFF;


                    }
                }
            }

            if (time_down >= 0.0F) time_down -= dt;
            if (time_down <= 0.0F) if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) p->option_status = OFF;
            
        }

    }

    EndScissorMode();

    if (scrollable) {
        float t = visible_area_size / entire_scrollable_area;
        float q = content_scroll / entire_scrollable_area;
        float padding = PANEL_LINE_THICK;
        Rectangle scroll_bar_area{
            panel_list_boundary.x + panel_list_boundary.width + (padding * 0.5F),
            panel_list_boundary.y + (padding * 1),
            panel.width - panel_list_boundary.width - (padding * 2.0F),
            panel_list_boundary.height - (padding * 2)
        };
        //DrawRectangleRec(scroll_bar_area, DARKBLUE);

        Rectangle scroll_bar_boundary{
            scroll_bar_area.x,
            scroll_bar_area.y + scroll_bar_area.height * q,
            scroll_bar_area.width,
            scroll_bar_area.height * t
        };
        DrawRectangleRounded(scroll_bar_boundary, 0.7F, 10, CONTENT_COLOR);

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
}

void DeleteMusic(int& retFlag, size_t order)
{
    if (order >= 0 && order < data.size()) {
        data.erase(data.begin() + order);

        if (music_play > order) {
            order--;
            music_play--;
        }

        if (Save()) TraceLog(LOG_INFO, "[SUCCESS] Delete Music");

        if (data.size() == 0) {
            std::ofstream file(data_txt);
            if (file.is_open()) {
                file << "0";
                file.close();
                FileZeroDataCheck(data_txt);
            }
        }

    }
    if (data.size() == 0) {
        p->page = PAGE_DRAG_DROP;
        zero_data = true;
        { retFlag = 2; return; };
        //goto drag_drop_label;
    }

    if (music_play == order) {
        if (order != 0) music_play--;

        if (data.size() > 0) {
            // SAVE TO TXT AGAIN
            DetachAudioStreamProcessor(music.stream, callback);
            ResetVisualizerParameter();

            music = LoadMusicStream(data.at(music_play).path.c_str());
            time_domain_signal = ExtractMusicData(data.at(music_play).path);

            p->reset_time = true;
            //setting_on = OFF;

            AttachAudioStreamProcessor(music.stream, callback);
        }
    }

    
}

void ResetVisualizerParameter()
{
    std::fill(Peak.begin(), Peak.end(), PeakInfo{});
    std::fill(Spectrum.begin(), Spectrum.end(), 0.0F);
    std::fill(smoothedAmplitude.begin(), smoothedAmplitude.end(), 0.0F);
    for (size_t i = 0; i < BUCKETS; ++i) {
        std::fill(prevAmplitude[i].begin(), prevAmplitude[i].end(), 0.0f);
    }
}

void DrawMainDisplay(Rectangle& panel_main)
{
    float pad = 20.0F;
    font = &font_s_semibold;
    Rectangle panel_title_display{
        panel_main.x,
        panel_main.y + 30,
        panel_main.width,
        40
    };
    BeginScissorMode(
        static_cast<int>(panel_title_display.x + (pad * 1)),
        static_cast<int>(panel_title_display.y + (pad * 0)),
        static_cast<int>(panel_title_display.width - (pad * 2)),
        static_cast<int>(panel_title_display.height - (pad * 0))
    );
    DrawTitleMP3(panel_title_display);
    EndScissorMode();

    float panel_pad = panel_main.width * 0.01F;
    Rectangle panel_display{
        panel_main.x + (panel_pad * 1),
        panel_main.y,
        panel_main.width - (panel_pad * 2),
        panel_main.height,
    };

    font = &font_counter;
    Rectangle panel_counter_display{
        panel_title_display.x,
        panel_title_display.y + panel_title_display.height,
        panel_title_display.width,
        panel_title_display.height
    };
    DrawCounter(panel_counter_display);


    if (IsKeyPressed(KEY_ONE)) {
        p->mode = MODE_NATURAL;
    }
    else if (IsKeyPressed(KEY_TWO)) {
        p->mode = MODE_EXPONENTIAL;
    }
    else if (IsKeyPressed(KEY_THREE)) {
        p->mode = MODE_MULTI_PEAK;
    }
    else if (IsKeyPressed(KEY_FOUR)) {
        p->mode = MODE_MAX_PEAK;
    }
    else if (IsKeyPressed(KEY_G)) {
        p->glow = !p->glow;
    }

    if (p->music_playing) {
        dc_offset(in);
        hann_window(in, N);
    }
    fft_calculation(in, out, N);

    for (int i = 0; i < BUCKETS; i++) {
        Spectrum.at(i) = 0.0F;
    }

    float min_amp = std::numeric_limits<float>::max();  // Or a very large positive value
    float max_amp = std::numeric_limits<float>::min();  // Or a very large negative value
    for (int i = 0; i < N / 2; i++) {
        float real_num = (float)out[i][0];
        float imaginer = (float)out[i][1];

        float amplitude = std::sqrt((real_num * real_num) + (imaginer * imaginer));

        min_amp = std::min(min_amp, amplitude);
        max_amp = std::max(max_amp, amplitude);
    }

    for (int i = 0; i < N / 2; i++) {
        float real_num = (float)out[i][0];
        float imaginer = (float)out[i][1];

        float amplitude = std::sqrt((real_num * real_num) + (imaginer * imaginer));

        for (int j = 0; j < BUCKETS; j++) {
            float freq = min_frequency + i * bin_width;
            //float freq = i * 48000/N;
            //float freq = std::powf(10, log_f_min + i * delta_log);

            if (freq >= Freq_Bin.at(j) && freq <= Freq_Bin.at(j + 1)) {
                Spectrum.at(j) = std::max(Spectrum.at(j), amplitude);

                if (amplitude > Peak.at(j).amplitude) {
                    Peak.at(j).amplitude = amplitude;
                    Peak.at(j).frequency_index = i;
                }

            }

        }

    }

    float dt = GetFrameTime();
    
    for (int i = 0; i < BUCKETS; i++) {
        for (int j = SMOOTHING_BUFFER_SIZE - 1; j > 0; --j) {
            prevAmplitude.at(i).at(j) = prevAmplitude.at(i).at(j - 1);
        }

        float norm_amplitude = Spectrum.at(i);

        prevAmplitude.at(i).at(0) = norm_amplitude;

        smoothedAmplitude.at(i) = calculateMovingAverage(prevAmplitude.at(i), SMOOTHING_BUFFER_SIZE);

        maxAmplitude = std::max(maxAmplitude, smoothedAmplitude.at(i));
    }
    // SPLINE INITIALIZATION
    std::unique_ptr<Vector2[]> pointsArray_RealTime_smart(new Vector2[BUCKETS]);
    std::unique_ptr<Vector2[]> pointsArray_Norm_smart(new Vector2[BUCKETS]);

    // JUST FOR DRAWING
    for (int i = 0; i < BUCKETS; i++) {
        float final_amplitude = smoothedAmplitude.at(i);

        switch (p->mode)
        {
        case MODE_NATURAL:
            final_amplitude = natural_scale(final_amplitude, 0.02F);
            break;
        case MODE_EXPONENTIAL:
            final_amplitude = exponential_scale(final_amplitude, 0.5F);
            break;
        case MODE_MULTI_PEAK:
            final_amplitude = multi_peak_scale(final_amplitude, i, 1.0F, Peak);
            break;
        case MODE_MAX_PEAK:
            final_amplitude = max_peak_scale(final_amplitude, maxAmplitude, 0.2F);
            break;
        default:
            break;
        }

        Vector2 coor = { normalization(float(i), 0.0F, (BUCKETS - 1)), (1 - final_amplitude * 0.7F) };
        pointsArray_Norm_smart[i] = coor;

        float bar_h = final_amplitude * panel_display.height * 0.65F;
        //float bar_h = final_amplitude * panel_display.height * 0.9F;
        float bar_w = panel_display.width / BUCKETS;

        pad = bar_w * sqrtf(1 - final_amplitude) * 0.35F;
        float base_h = bar_w * 0.7F;
        Rectangle base = {
            panel_display.x + (i * bar_w),
            (panel_display.y + panel_display.height) - base_h * 1.5F,
            bar_w,
            base_h
        };

        Rectangle bar = {
            panel_display.x + (i * bar_w),
            base.y + (base.height * 0.5F) - bar_h,
            bar_w,
            bar_h
        };

        Color color{};
        float hue = (float)i / BUCKETS;
        float sat = 1.0F;
        float val = 1.0F;
        color = ColorFromHSV(hue * 360, sat, val);

        
        Vector2 startPos = { bar.x + bar.width / 2, (bar.y + bar.height) - bar_h };
        Vector2 endPos = { bar.x + bar.width / 2, (bar.y + bar.height) };
        float thick = 4.0F * sqrt(final_amplitude) * (bar_w * 0.10F);
        //DrawLineEx(startPos, endPos, thick, color);

        Vector2 center_bins = { bar.x + bar.width / 2, bar.y };
        float radius = bar_w * sqrt(final_amplitude) * 1.25F * 2;

        pointsArray_RealTime_smart[i] = center_bins;

        // Maybe can used for toggle glow or bubble effect. not as default 
        if (p->glow) {
            // DRAW BUBBLE USING SHADERS
            radius = bar_w * sqrt(final_amplitude) * 1.40F * 2;
            BeginShaderMode(p->bubble);
            Texture2D bubble_texture = { rlGetTextureIdDefault(), 1,1,1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
            Vector2 bubble_pos = {
                startPos.x - radius,
                startPos.y - radius
            };
            float bubble_rotation = { 0 };
            float bubble_scale = radius * 2;
            DrawTextureEx(bubble_texture, bubble_pos, bubble_rotation, bubble_scale, Fade(RAYWHITE, 0.25F));
            EndShaderMode();
        }

        // DRAW CIRCLE USING SHADERS
        radius = bar_w * sqrt(final_amplitude) * 1.20F * 2;
        BeginShaderMode(p->circle);
        Texture2D circle_texture = { rlGetTextureIdDefault(), 1,1,1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };

        // TOP CIRCLE
        Vector2 top_pos = {
            startPos.x - radius,
            startPos.y - radius
        };
        float rotation = { 0 };
        float scale = radius * 2;
        //DrawTextureEx(circle_texture, top_pos, rotation, scale, color);

        // BASE CIRCLE
        radius = radius * 0.70F;
        scale = radius * 2;
        Vector2 base_pos = {
            endPos.x - radius,
            endPos.y - radius
        };
        //DrawTextureEx(circle_texture, base_pos, rotation, scale, color);

        
        // NEW FFT ROTATION STYLE, USE LINE AND SHADERS
        color = ColorFromHSV(hue * 360 + GetFrameTime(), sat, val);
        Vector2 center_panel_main{
            panel_display.x + (panel_display.width * 0.5F),
            panel_display.y + (panel_display.height * 0.495F)
        };

        float value = sqrt(final_amplitude)*panel_display.height * 0.4F;
        float angle = (360.0F / 50.0F) * i;
        //float angle = 0.5F * (float)i;
        Vector2 startPos_fft_rotation = center_panel_main;

        Vector2 endPos_fft_rotation = {
            startPos_fft_rotation.x + (sin(angle) * value),
            startPos_fft_rotation.y + (cos(angle) * value)
        };

        Vector2 Pos_40 = {
            startPos_fft_rotation.x + (sin(angle) * value) * 0.4F,
            startPos_fft_rotation.y + (cos(angle) * value) * 0.4F
        };
        Vector2 Pos_70 = {
            startPos_fft_rotation.x + (sin(angle) * value) * 0.7F,
            startPos_fft_rotation.y + (cos(angle) * value) * 0.7F
        };
        Vector2 Pos_90 = {
            startPos_fft_rotation.x + (sin(angle) * value) * 0.9F,
            startPos_fft_rotation.y + (cos(angle) * value) * 0.9F
        };
        Vector2 Pos_100 = {
            startPos_fft_rotation.x + (sin(angle) * value) * 0.9F,
            startPos_fft_rotation.y + (cos(angle) * value) * 0.9F
        };
        //DrawLineEx(startPos_fft_rotation, endPos_fft_rotation, 2.0, color);
        {

            float radius_40 = sqrt(value) * 1.0F;
            float scale = radius_40 * 2;
            Vector2 base_40_pos = {
                Pos_40.x - radius_40,
                Pos_40.y - radius_40,
            };
            //DrawTextureEx(circle_texture, base_40_pos, rotation, scale, color);
            //DrawTextureEx(circle_texture, base_40_pos, rotation, scale, WHITE);

            //float radius_70 = value * 0.05F;
            float radius_70 = 2.50F;
            scale = radius_70 * 2;
            Vector2 base_70_pos = {
                Pos_70.x - radius_70,
                Pos_70.y - radius_70,
            };
            //DrawTextureEx(circle_texture, base_70_pos, rotation, scale, color);
            //DrawTextureEx(circle_texture, base_70_pos, rotation, scale, WHITE);

            float radius_90 = value * 0.2F;
            scale = radius_90 * 2;
            Vector2 base_90_pos = {
                Pos_90.x - radius_90,
                Pos_90.y - radius_90,
            };
            //DrawTextureEx(circle_texture, base_90_pos, rotation, scale, color);

            float radius_100 = sqrt(value) * 0.75F;
            scale = radius_100 * 2;
            Vector2 base_100_pos = {
                Pos_100.x - radius_100,
                Pos_100.y - radius_100,
            };
            //DrawTextureEx(circle_texture, base_100_pos, rotation, scale, color);
            //DrawTextureEx(circle_texture, base_100_pos, rotation, scale, WHITE);

        }

        EndShaderMode();

    }

    // Make Rectangle
    Rectangle base{ panel_display };
    int JUMLAH_RECT = 100;
    float coef_rect = 0.955F;
    std::deque<Rectangle> landscape_rects{};

    for (int i = 0; i < JUMLAH_RECT; i++) {
        Rectangle edited = {
            base.x + ((base.width - base.width * coef_rect) / 2),
            base.y + base.height * 0.025F * coef_rect,
            base.width * coef_rect,
            base.height * coef_rect
        };

        landscape_rects.push_back(edited);
        //landscape_rects.push_front(edited);
        coef_rect += 0.00015F;

        base = edited;

        //DrawRectangleLinesEx(edited, 1.0F, RED);
    }

    std::unique_ptr<Vector2[]> spline_pointer_smart(new Vector2[BUCKETS]);

    static float time_check{};
    time_check += dt;
    float frame_rate_capture = 40; // FPS
    float time_to_capture{ 1.0F / frame_rate_capture };
    if (time_check >= time_to_capture) {

        std::vector<Vector2> points{};
        for (int i = 0; i < BUCKETS; i++) {
            points.push_back(pointsArray_Norm_smart[i]);
        }

        if (landscape_splines.size() >= JUMLAH_RECT) {
            landscape_splines.pop_back();
        }
        landscape_splines.push_front(points);

        time_check = 0;
    }

    

    float thick = 1.0F;
    for (int i = 0; i < landscape_splines.size(); i++) {
        Rectangle rect = landscape_rects.at(i);

        for (int j = 0; j < BUCKETS; j++) {
            spline_pointer_smart[j] = {
                rect.x + rect.width * landscape_splines.at(i)[j].x,
                rect.y + rect.height * landscape_splines.at(i)[j].y
            };
        }
        DrawSplineLinear(spline_pointer_smart.get(), BUCKETS, 6.0F * thick, Fade(SKYBLUE, thick / 4));
        DrawSplineLinear(spline_pointer_smart.get(), BUCKETS, 1.25F * thick, LIGHTGRAY);

        //DrawSplineBasis(spline_pointer_smart.get(), BUCKETS, thick, LIGHTGRAY);
        //DrawRectangleLinesEx(rect, .4F, BLUE);

        thick *= 0.97F;
    }

    Color color = BLUE;

    DrawSplineCatmullRom(pointsArray_RealTime_smart.get(), BUCKETS, 13.0F, Fade(color, 0.1F));
    DrawSplineCatmullRom(pointsArray_RealTime_smart.get(), BUCKETS, 10.0F, Fade(color, 0.15F));
    DrawSplineCatmullRom(pointsArray_RealTime_smart.get(), BUCKETS, 7.0F, Fade(color, 0.2F));
    DrawSplineCatmullRom(pointsArray_RealTime_smart.get(), BUCKETS, 5.0F, Fade(color, 0.25F));
    DrawSplineCatmullRom(pointsArray_RealTime_smart.get(), BUCKETS, 2.5F, Fade(WHITE, 1.0F));


    // DRAW MUSIC MODE
    DrawMusicPlayMode(panel_main, dt);

    // DRAW FULLSCREEN BUTTON
    DrawFullscreenButton(panel_main, dt);

}

void DrawFullscreenButton(Rectangle& panel_main, float dt)
{
    float fullscreen_hover_size = panel_main.height / 4;
    Rectangle fullscreen_btn_area_hover{
        panel_main.x + panel_main.width - (fullscreen_hover_size + 10), // add 10 to make space to border, to minimize mouse_stuck.
        panel_main.y + panel_main.height - fullscreen_hover_size,
        fullscreen_hover_size,
        fullscreen_hover_size
    };
    static float alpha_coef{};

    if (CheckCollisionPointRec(mouse_position, fullscreen_btn_area_hover)) {
        if (alpha_coef <= 1.0F) {
            //alpha_coef += dt * 0.5F;
            alpha_coef += sqrtf(dt);
        }
    }
    else {
        if (alpha_coef >= 0.0F) {
            //alpha_coef -= dt * 0.5F;
            alpha_coef -= sqrtf(dt) / 4;
        }
    }

    bool draw_icon = alpha_coef > 0.0F;

    if (draw_icon) {
        float fullscreen_btn_size = 50.0F;
        float space = 5.0F;
        Rectangle fullscreen_panel{
            panel_main.x + panel_main.width - (fullscreen_btn_size + space),
            panel_main.y + panel_main.height - (fullscreen_btn_size + space - PANEL_LINE_THICK),
            fullscreen_btn_size,
            fullscreen_btn_size
        };
        float pad = 5.0F;
        Rectangle fullscreen_btn{
            fullscreen_panel.x + (pad * 1),
            fullscreen_panel.y + (pad * 1),
            fullscreen_panel.width - (pad * 2),
            fullscreen_panel.height - (pad * 2),
        };
        DrawRectangleRounded(fullscreen_btn, 0.25F, 10, Fade(LIGHTGRAY, 0.20F * alpha_coef));

        Color icon_color{};
        static float time_down{};
        if (p->fullscreen == OFF) {
            if (CheckCollisionPointRec(mouse_position, fullscreen_btn)) {
                p->icon_fullscreen_index = 1;
                icon_color = WHITE;
                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && (time_down <= 0.0F)) {
                    p->fullscreen = ON;
                    time_down = 0.1F;
                }
            }
            else {
                p->icon_fullscreen_index = 0;
                icon_color = LIGHTGRAY;
            }
        }
        else {
            if (CheckCollisionPointRec(mouse_position, fullscreen_btn)) {
                p->icon_fullscreen_index = 3;
                icon_color = WHITE;
                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && (time_down <= 0.0F)) {
                    p->fullscreen = OFF;
                    time_down = 0.1F;
                }
            }
            else {
                p->icon_fullscreen_index = 2;
                icon_color = LIGHTGRAY;
            }
        }

        if (time_down >= 0.0F) time_down -= dt;

        float icon_size = 100.0F;
        Rectangle dest{ fullscreen_btn };
        Rectangle source{ p->icon_fullscreen_index * icon_size, 0, icon_size, icon_size };
        DrawTexturePro(FULLSCREEN_TEX, source, dest, { 0,0 }, 0, Fade(icon_color, 1.0F * alpha_coef));
    }
}

void DrawMusicPlayMode(Rectangle& panel_main, float dt)
{

    float play_mode_hover_size = panel_main.height / 4;
    Rectangle play_mode_btn_area_hover{};
    if (p->fullscreen) {
        play_mode_btn_area_hover = {
            panel_main.x + 10,  // add 10 to make space to border, to minimize mouse_stuck.
            panel_main.y + panel_main.height - play_mode_hover_size,
            play_mode_hover_size,
            play_mode_hover_size
        };
    }
    else {
        play_mode_btn_area_hover = {
            panel_main.x,
            panel_main.y + panel_main.height - play_mode_hover_size,
            play_mode_hover_size,
            play_mode_hover_size
        };
    }


    static float alpha_coef{};

    if (CheckCollisionPointRec(mouse_position, play_mode_btn_area_hover)) {
        if (alpha_coef <= 1.0F) {
            alpha_coef += sqrtf(dt);
        }
    }
    else {
        if (alpha_coef >= 0.0F) {
            alpha_coef -= sqrtf(dt) / 4;
        }
    }

    bool draw_icon = alpha_coef > 0.0F;

    if (draw_icon) {
        float play_mode_btn_size = 50.0F;
        float space = 5.0F;

        Rectangle play_mode_panel = {
            panel_main.x + space - PANEL_LINE_THICK,
            panel_main.y + panel_main.height - (play_mode_btn_size + space - PANEL_LINE_THICK),
            play_mode_btn_size,
            play_mode_btn_size
        };
        float pad = 5.0F;
        Rectangle play_mode_btn{
            play_mode_panel.x + (pad * 1),
            play_mode_panel.y + (pad * 1),
            play_mode_panel.width - (pad * 2),
            play_mode_panel.height - (pad * 2),
        };
        DrawRectangleRounded(play_mode_btn, 0.25F, 10, Fade(LIGHTGRAY, 0.20F * alpha_coef));

        Color icon_color = LIGHTGRAY;
        if (CheckCollisionPointRec(mouse_position, play_mode_btn)) {
            icon_color = RAYWHITE;
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                p->repeat = !p->repeat;
            }
        }

        float icon_size = 100.0F;
        Rectangle dest = play_mode_btn;
        Rectangle source = { p->repeat * icon_size, 0, icon_size, icon_size };
        DrawTexturePro(MODE_TEX, source, dest, { 0,0 }, 0, Fade(icon_color, 1.0F * alpha_coef));
        
    }
}

void DrawMusicProgress(Rectangle& panel_progress, float& music_volume)
{
    // PROGRESS PANEL
    panel_progress = panel_progress;
    //DrawRectangleRec(panel_progress, PANEL_PROGRESS_BASE_COLOR);

    float progress_ratio = static_cast<float>(panel_progress.width) / duration;
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

    if (p->fullscreen && p->mouse_onscreen == OFF) DrawRectangleRec(progress_bar, progress_color_hover);
    else DrawProgressTimeDomain(panel_progress, progress_w);

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
        music_volume = 0.0F;
        progress_w = mouse_position.x - panel_progress.x;

        music_time = static_cast<int>(progress_w / progress_ratio);

        if (music_time < 0) {
            music_time = 1;
            progress_w = 0;
        }
        else if (music_time > duration) {
            music_time = duration;
            progress_w = progress_ratio * music_time;
        }
    }
    else if (p->dragging == DRAG_MUSIC_PROGRESS && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        p->dragging = DRAG_RELEASE;
        p->music_playing = true;

        float t = (mouse_position.x - panel_progress.x) / panel_progress.width;
        if (mouse_position.x < panel_progress.x) {
            t = 0.001F;
        }
        else if (mouse_position.x - panel_progress.x > panel_progress.width) {
            t = 0.999F;
        }
        SeekMusicStream(music, (t * duration / 1000));
    }

}

bool is_Draw_Icons()
{
    bool draw_setting_icon = true;
    if (p->mouse_onscreen == true && p->fullscreen == ON) {
        return draw_setting_icon = true;
    }
    else if (p->fullscreen == OFF) {
        return draw_setting_icon = true;
    }
    else {
        return draw_setting_icon = false;
    }
}

void LoadMP3()
{
    FilePathList dropped_files = LoadDroppedFiles();

    for (size_t i = 0; i < dropped_files.count; i++) {

        const char* c_file_path = dropped_files.paths[i];
        std::string cpp_file_path = std::string(c_file_path);
        std::string file_name = std::filesystem::path(cpp_file_path).stem().string();
        //std::string file_extension = std::filesystem::path(cpp_file_path).extension().string();

        if (IsFileExtension(c_file_path, ".mp3") || IsFileExtension(c_file_path, ".wav") || IsFileExtension(c_file_path, ".flac") || IsFileExtension(c_file_path, ".ogg")) {
            TraceLog(LOG_INFO, "SUCCESS: Adding new file [ %s ]", cpp_file_path.c_str());

            Data newData{};
            newData.path = cpp_file_path;
            newData.name = file_name;
            newData.duration = GetDuration(c_file_path);

            data.push_back(newData);

            if (Save()) {
                TraceLog(LOG_INFO, "[SUCCESS] Save [%s] to data.txt", file_name.c_str());
                ReloadVector();
                if (zero_data) {
                    music_play = 0;
                    music = LoadMusicStream(data.at(music_play).path.c_str());
                    while (!IsMusicReady(music)) {
                    }
                    PlayMusicStream(music);
                    AttachAudioStreamProcessor(music.stream, callback);
                    p->music_playing = true;
                }
                zero_data = false;
            }
            else {
                TraceLog(LOG_ERROR, "Failed to save [%s]", file_name.c_str());
            }

        }
        else {
            TraceLog(LOG_ERROR, "Failed adding new file, only support mp3/wav/flac/ogg files");
        }
    }

    UnloadDroppedFiles(dropped_files);
}

void ApplyInputReset(std::string& input, bool& popup_on, std::string& name)
{
    int new_target = std::stoi(input);
    int old_target = data.at(music_play).target;
    data.at(music_play).target = new_target;
    p->popup_on = OFF;

    if (Save()) TraceLog(LOG_INFO, "[SUCCESS] Reset Target of [%s] from : [%d] to : [%d]", name.c_str(), old_target, new_target);

    // CLEAR
    if (p->popup_on == OFF) input.clear();

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
    Color icon_color = GRAY;
    if (CheckCollisionPointRec(mouse_position, hover_panel)) {
        icon_color = RAYWHITE;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            p->music_playing = !p->music_playing;
        }
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
    font = &font_number;
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
    float font_size = panel_duration.height * 0.7F;
    float font_space = 0.5F;
    const char* text = cpp_text.c_str();
    Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
    Vector2 text_coor{
        panel_duration.x + (panel_duration.width - text_measure.x) / 2,
        panel_duration.y + (panel_duration.height - text_measure.y) / 2
    };
    DrawTextEx(*font, text, text_coor, font_size, font_space, RAYWHITE);
}

void DrawCounter(Rectangle& panel)
{
    std::string counter = std::to_string(data.at(music_play).counter);
    std::string target = std::to_string(data.at(music_play).target);
    std::string cpp_text = counter + " / " + target;

    const char* text = cpp_text.c_str();
    float font_size = 30.0F;
    float font_space = 0.0F;
    Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
    Vector2 text_coor{
        panel.x + (panel.width - text_measure.x) / 2,
        panel.y + (panel.height - text_measure.y) / 2
    };
    Color color = RAYWHITE;
    Data data_check = data.at(music_play);
    if (data_check.counter > data_check.target) {
        color = Fade(TARGET_DONE_COLOR, 0.9F);
    }
    //DrawTextEx(*font, text, text_coor, font_size, font_space, color);
}

void DrawTitleMP3(Rectangle& panel)
{
    std::string cpp_text = data.at(music_play).name;
    const char* text = cpp_text.c_str();
    float font_size = 32.0F;
    float font_space = 0.0F;
    Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
    Vector2 text_coor{
        panel.x + (panel.width - text_measure.x) / 2,
        panel.y + (panel.height - text_measure.y) / 2
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

    //for (const auto& i : data) {
    //    std::cout << i.name << std::endl;
    //    std::cout << i.duration << std::endl;
    //}
}

int GetDuration(const char* c_file_path)
{
    // Convert duration to milliseconds
    int durationInMilliseconds = static_cast<int>(GetMusicTimeLength(LoadMusicStream(c_file_path)) * 1000);

    // Print duration in milliseconds
    std::cout << "Duration: " << durationInMilliseconds << " milliseconds" << std::endl;

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

void InitFile(const std::filesystem::path& filename) 
{
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
