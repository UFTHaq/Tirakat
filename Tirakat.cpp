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
// 9. Add Label on button if hover functionality
//
// 10. Maybe consider to use JUCE? maybe not, maybe in the next project, maybe for MusMiBot?

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
// 
// 3. Tambah Splash Screen
// 
// 4. Tambah notification format file couldn't load.
// 
// 5. Edge case: jika ada simbol tidak umum di judul, biasanya tidak akan bisa load. perbaiki, atau tambah notifikasi perlu mengubah judul.
//


// MAXIMUM MIDI FREQ -> G#9/Ab9 = 13289.75Hz


// Features now:
// 1. Drag & Drop single or multiple musics.
// 2. Music format support : MP3, WAV, FLAC, & OGG.
// 3. Play Pause music by mouse and KEY_SPACE.
// 4. Volume control by mouse hold and drag, mouse wheel, press or hold down KEY_UP KEY_DOWN.
// 5. Mute Unmute by KEY_M.
// 6. Display playlists scrollable and can be re-arrange by mouse hold and drag.
// 7. Display time of play and duration of musics.
// 8. Music seek time in progress bar by mouse hold and drag, and press or hold down KEY_RIGHT KEY_LEFT.
// 9. Display time domain signal of music as progress bar, make it easy to jump and seek to the wanted time.
// 10.Display freq domain signal visualization (fft) in many modes and algorithms: Classic, Galaxy, Landscape.
// 11.2 mode of Playlist, repeat and loop.
// 12.Fullscreen display, by mouse and KEY_F.
// 13.Lock Time Domain Display, if you want it, by mouse and KEY_L.
//


// For next release try to:
// 1. Make Spectrogram.
// 2. Make custom title bar maybe.
// 3. Convert Image to Spectrogram, Spectrogram to audio? then i can see in my tirakat spectrogram mode.
// 

// buat sanbox untuk mengubah image ke grayscale. input dengan drag and drop, masukkan kiri panel, lalu sebelah kanan hasil grayscale, dan bisa disave. buat dengan texture.

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
#include <cmath>
#include <mutex>

#include <chrono>
#include <thread>

#include <raylib.h>
#include <rlgl.h>
#include <fftw3.h>
#include <SFML/Audio.hpp>

#define FONT_LOC_Roboto_Slab                    {"resources/Fonts/Roboto_Slab/static/RobotoSlab-Regular.ttf"}
#define FONT_LOC_Roboto_Mono                    {"resources/Fonts/Roboto_Mono/static/RobotoMono-SemiBold.ttf"}
#define FONT_LOC_Source_Sans_BOLD               {"resources/Fonts/Source_Sans_3/static/SourceSans3-Bold.ttf"}
#define FONT_LOC_Source_Sans_SEMIBOLD           {"resources/Fonts/Source_Sans_3/static/SourceSans3-SemiBold.ttf"}
#define FONT_LOC_Source_Sans_REG                {"resources/Fonts/Source_Sans_3/static/SourceSans3-Regular.ttf"}
#define FONT_LOC_Sofia_Sans_Condensed_BOLD      {"resources/Fonts/Sofia_Sans_Condensed/static/SofiaSansCondensed-Bold.ttf"}
//#define FONT_LOC_Sofia_Sans_Condensed_REG       {"resources/Fonts/Sofia_Sans_Condensed/static/SofiaSansCondensed-Regular.ttf"}
#define FONT_LOC_Sofia_Sans_Condensed_REG       {"resources/Fonts/Sofia_Sans_Condensed/static/SofiaSansCondensed-Medium.ttf"}

#define ICON_APP_LOC        {"resources/Icons/Tirakat-V4.png"}
#define ICON_PLAYPAUSE_LOC  {"resources/Icons/PlayPause.png"}
#define ICON_FULLSCREEN_LOC {"resources/Icons/Fullscreen.png"}
#define ICON_VOLUME_LOC     {"resources/Icons/Volume.png"}
#define ICON_SETTING_LOC    {"resources/Icons/Setting.png"}
#define ICON_X_LOC          {"resources/Icons/X.png"}
#define ICON_DELETE_LOC     {"resources/Icons/Trash.png"}
#define ICON_MODE_LOC       {"resources/Icons/Mode.png"}
#define ICON_POINTER_LOC    {"resources/Icons/Pointer.png"}
#define ICON_LOCK_LOC       {"resources/Icons/Lock.png"}

#define HUD_TIMER_SECS                              1.5F
#define PANEL_LEFT_WIDTH                            275.0F
#define PANEL_DURATION_HEIGHT                       40.0F
#define PANEL_DURATION_WIDTH                        PANEL_LEFT_WIDTH
#define PANEL_BOTTOM                                50.0F
#define PANEL_MEDIA_HEIGHT                          PANEL_BOTTOM
#define PANEL_MEIDA_WIDTH                           PANEL_LEFT_WIDTH
#define PANEL_PROGRESS_HEIGHT                       PANEL_BOTTOM
#define PANEL_PROGRESS_HEIGHT_FULLSCREEN_OFFSCREEN  5.0F
#define PANEL_LINE_THICK                            4.0F // 4.0F

#define BASE_COLOR                  Color{  20,  20,  20, 255 }
#define PANEL_COLOR                 Color{  30,  30,  30, 255 }
#define PANEL_LEFT_COLOR            Color{  40,  40,  40, 255 }
#define PANEL_COLOR2                Color{  30,  30,  30, 255 }
#define PANEL_LINE_COLOR            Color{  20,  20,  20, 255 }
#define PANEL_PROGRESS_BASE_COLOR   Color{  25,  25,  25, 255 }
#define PANEL_PROGRESS_COLOR        DARKGRAY

#define CONTENT_COLOR               Color{  60,  60,  60, 255 }
#define CONTENT_CHOOSE_COLOR        Color{ 150, 150, 150, 255 }

#define BLUE_BUTTON_COLOR           Color{  58,  76, 131, 255 }
#define POPUP_CARD_COLOR            Color{ 112, 141, 225, 255 }
#define POPUP_X_COLOR               Color{ 190,  60,  50, 255 }
#define POPUP_BODY_COLOR            Color{ 203, 209, 216, 255 }
#define POPUP_APPLY_COLOR           Color{ 75,  109, 214, 255 }
#define POPUP_CANCEL_COLOR          Color{ 142, 149, 178, 255 }
#define TARGET_DONE_COLOR           Color{  80, 180, 120, 255 }

#define KEY_TOGGLE_PLAY             KEY_SPACE
#define KEY_TOGGLE_MUTE             KEY_M
#define KEY_FULLSCREEN              KEY_F
#define KEY_VISUAL_MODE             KEY_V 
#define KEY_LOCK_TIME_DOMAIN        KEY_L

#define MIN_FREQ                    10.0F 
#define MAX_FREQ                    24000.0F
//#define MAX_FREQ                    22050.F
#define MAX_GRADIENT_COLORS         5

const int N{ 480 * 2 };

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

enum VisualModes {
    WAVE,
    CLASSIC,
    GALAXY,
    LANDSCAPE,
    SPECTROGRAM
};

enum TrimString {
    EASY,
    BOLD
};

struct VisualMode {
    std::string title{};
    std::string shortcut{};
    bool enable{};
};

VisualMode visualM1{ "Wave"         , "V + 1", ON };
VisualMode visualM2{ "Classic"      , "V + 2", ON };
VisualMode visualM3{ "Galaxy"       , "V + 3", ON };
VisualMode visualM4{ "Landscape"    , "V + 4", ON };
VisualMode visualM5{ "Spectogram"   , "V + 5", ON };

struct Notification {
    std::string g_info{};
    float g_info_timer{};
};

struct DragDropPopup {
    std::string name{};
    bool info{};
    float time{ 0.0F };
    float alpha{ 1.0F };
    float slide_up{ 0.0F };

    DragDropPopup(const std::string& name, const bool info) : name(name), info(info) {}

    void updateTime() {
        time += GetFrameTime();
    }

    void updateAlpha() {
        float threshold = 5.0F;
        if (time > threshold) {
            alpha -= 0.01F;
            if (alpha < 0.0F) alpha = 0.0F;
        }
    }

    void updateSlideUP() {
        if (slide_up < 1.0F) slide_up += 0.05F;
    }

    void updateAll() {
        updateTime();
        updateAlpha();
        updateSlideUP();
    }

    void resetSlideUp() {
        slide_up = 0.0F;
    }

    float getSlideUp() { return slide_up; }

    bool getInfo() { return info; }

    bool isExpired() const {
        return alpha <= 0.0F;
    }
};

struct Plug {
    int page{};
    int play{};
    int dragging{};
    bool music_playing{};
    uint8_t music_channel{ 2 };
    bool reset_time{};
    bool icon_pp_index{};
    bool volume_mute{ false };
    float last_volume{};
    size_t icon_fullscreen_index{};
    bool fullscreen{ false };
    bool popup_on = false;
    std::string popup_title{};
    bool mouse_onscreen{ true };
    bool repeat{ ON };
    int mouse_cursor{};
    bool glow{ false };
    int mode{ MODE_NATURAL };
    bool moving_save{ false };
    Shader circle{};
    Shader bubble{};
    int option_status{ OFF };
    size_t option_music_order{};
    bool visual_mode_expand{ OFF };
    std::vector<VisualMode> visualmode{ visualM1, visualM2, visualM3, visualM4, visualM5 };
    size_t visual_mode_active{ SPECTROGRAM };
    bool toggle_windowed_wave{ true };
    Notification notification{};
    float mouse_onscreen_timer{ HUD_TIMER_SECS };
    bool visual_time_domain_lock{ ON };
    size_t icon_lock_index{};
    //const int spectrogram_h{ static_cast<int>((1 << 9)) };
    const int spectrogram_h{ static_cast<int>((N / 2)) };
    //const int spectrogram_w{ (1 << 9) * 16 / 9 };
    const int spectrogram_w{ 860 };
    Image spectrogram_image{};
    Texture2D SPECTROGRAM_TEXTURE{};
    const int spectrogram_zone_out_w{ 255 };
    Image spectrogram_zone_out_image{};
    Texture2D SPECTROGRAM_ZONE_OUT_TEXTURE{};
    Image spectrogram_zone_in_image{};
    Texture2D SPECTROGRAM_ZONE_IN_TEXTURE{};
    std::deque<DragDropPopup> DragDropPopupTray{};
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

// Special cause mistake i made
struct SpecialToolTip {
    bool enable{};
    Rectangle rect{};
};

SpecialToolTip special_btn_delete{};
SpecialToolTip special_btn_setting{};

struct Frame {
    float left{};
    float right{};
};

//const int N{ 1 << 10 };

//fftw_complex* fftw_in  = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
//fftw_complex* fftw_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
std::vector<fftw_complex> fftw_in(N);
std::vector<fftw_complex> fftw_out(N);
std::vector<float> wave_live(N / 2, 0.0F);
std::mutex wave_mutex;

const int BUCKETS{ 1 << 6 };
//const int BUCKETS{ 80 };
std::array<float, BUCKETS> Spectrum{};
std::array<float, BUCKETS + 1> Freq_Bin{};

const int SMOOTHING_BUFFER_SIZE{ 15 };
std::array<std::array<float, SMOOTHING_BUFFER_SIZE>, BUCKETS> prevAmplitude{};
std::array<float, BUCKETS> smoothedAmplitude{};
std::array<float, BUCKETS> out_smear{};
float maxAmplitude = 0.0F;

struct PeakInfo {
    int frequency_index{};
    float amplitude{};
};
std::array<PeakInfo, BUCKETS> Peak{};



void callback(void* bufferData, unsigned int frames) {
    // Cast buffer data to float array
    float* samples = (float*)bufferData;

    // Lock the mutex to safely update the global buffer
    std::lock_guard<std::mutex> lock(wave_mutex);

    // Process the samples
    for (unsigned int i = 0; i < frames; i++) {

        float leftSample = samples[2 * i];       // Left channel sample
        float rightSample = samples[2 * i + 1];  // Right channel sample

        // You can choose to store only one channel or process both
        wave_live[i] = (leftSample + rightSample) / 2.0f; // Example: average of both channels

        fftw_in[i][0] = (leftSample + rightSample) / 2.0f;
    }

    if (IsKeyPressed(KEY_D)) {
        std::cout << "frame size: " << frames << std::endl;
    }
}

// Using std::vector, for now this not activated
void cleanup() {
    //if (fftw_in != nullptr) {
    //    fftw_free(fftw_in);
    //} 
    //if (fftw_out != nullptr) {
    //    fftw_free(fftw_out);
    //}
}

void dc_offset(fftw_complex in[]) {
    double dc_offset = 0.0F;
    for (size_t i = 0; i < N; i++) {
        dc_offset += in[i][0];
    }
    dc_offset = dc_offset / (float)N;

    for (size_t i = 0; i < N; i++) {
        in[i][0] -= dc_offset;
    }
}

// Low-pass filter (simple moving average filter)
void low_pass_filter(fftw_complex in[], size_t n) {
    std::vector<double> filtered(n, 0.0);

    int window_size = 5; // Adjust as needed

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < window_size && i >= j; j++) {
            filtered[i] += in[i - j][0]; // Directly access the real part
        }
        filtered[i] /= window_size;
    }

    for (size_t i = 0; i < n; i++) {
        in[i][0] = filtered[i]; // Update the real part
    }
}

// Basic FIR Low-Pass Filter
void fir_low_pass_filter(fftw_complex in[], size_t n, double cutoff) {
    // Define filter coefficients for a basic FIR filter
    // Here, we create a simple low-pass filter using sinc function
    const int filter_size = 21; // Number of filter coefficients
    std::vector<double> h(filter_size);
    double fc = cutoff / (0.5 * N); // Normalize cutoff frequency by Nyquist frequency

    for (size_t i = 0; i < filter_size; i++) {
        if (i == (filter_size - 1) / 2) {
            h[i] = 2.0 * fc;
        }
        else {
            h[i] = sin(2.0 * PI * fc * (i - (filter_size - 1) / 2)) / (PI * (i - (filter_size - 1) / 2));
            // Apply Hamming window
            h[i] *= 0.54 - 0.46 * cos(2.0 * PI * i / (filter_size - 1));
        }
    }

    std::vector<double> filtered(n, 0.0);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < filter_size; j++) {
            if (i >= j) {
                filtered[i] += h[j] * in[i - j][0]; // Apply filter to the real part
            }
        }
    }

    for (size_t i = 0; i < n; i++) {
        in[i][0] = filtered[i]; // Update the real part
    }
}

void hann_window(fftw_complex in[], size_t N) {
    for (size_t i = 0; i < N; i++) {
        float w = 0.5F * (1.0F - cosf(2.0F * PI * i / (N - 1)));
        in[i][0] *= w;
    }
}

void hann_window(float in[], size_t N) {
    for (size_t i = 0; i < N; i++) {
        float w = 0.5F * (1.0F - cosf(2.0F * PI * i / (N - 1)));
        in[i] *= w;
    }
}

void hamming_window(fftw_complex in[], size_t N) {
    for (size_t i = 0; i < N; i++) {
        float w = 0.54F - 0.46F * cos(2 * PI * i / (N - 1));
        in[i][0] *= w;
    }
}

void gaussian_window(fftw_complex in[], size_t N) {
    float sigma = 0.5F;
    float center = (N - 1) / 2.0F; // Center of the window
    for (size_t i = 0; i < N; i++) {
        float exponent = -0.5F * std::powf((i - center) / (sigma * center), 2);
        float w = std::exp(exponent);
        in[i][0] *= w;
    }
}

void fftw_calculation(fftw_complex in[], fftw_complex out[], size_t N) {
    assert(N > 0);

    //fftw_plan plan{ fftw_plan_dft_1d(static_cast<int>(N), in, out, FFTW_FORWARD, FFTW_ESTIMATE) };
    fftw_plan plan{ fftw_plan_dft_1d(static_cast<int>(N), in, out, FFTW_FORWARD, FFTW_MEASURE) };

    //plan = fftw_plan_dft_1d(static_cast<int>(N), in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    fftw_execute(plan);

    fftw_destroy_plan(plan);
}

float natural_scale(float amplitude, float Fit_factor) {
    return amplitude * Fit_factor;
}

float exponential_scale(float amplitude, float Fit_factor) {
    //return std::log10(amplitude * Fit_factor) * Fit_factor;
    return std::log10(amplitude) * Fit_factor;
    //return sqrtf(std::log10(amplitude) * Fit_factor);
}

float multi_peak_scale(float amplitude, int i, float Fit_factor, const std::array<PeakInfo, BUCKETS>& Peak) {
    if (Peak.at(i).frequency_index >= 0) {
        if (i < 5) return amplitude / Peak.at(i).amplitude * Fit_factor * 1.3F; // untuk membuat drum bass pada awal bins lebih naik.
        else return amplitude / Peak.at(i).amplitude * Fit_factor;
        //else return sqrtf(amplitude / Peak.at(i).amplitude * Fit_factor) * 0.7F;
        //else return (amplitude / Peak.at(i).amplitude * Fit_factor) * (amplitude / Peak.at(i).amplitude * Fit_factor);
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

float min_frequency = MIN_FREQ;
float max_frequency = MAX_FREQ;
float bin_width = (max_frequency - min_frequency) / BUCKETS;

float log_f_min = std::log10(min_frequency);
float log_f_max = std::log10(max_frequency);
float delta_log = (log_f_max - log_f_min) / BUCKETS;

void make_bins() {
    std::cout << std::fixed << std::setprecision(2);
    for (size_t i = 0; i <= BUCKETS; i++) {
        Freq_Bin.at(i) = min_frequency + i * bin_width;
        //Freq_Bin.at(i) = std::powf(10, log_f_min + i * delta_log);
        std::cout << Freq_Bin[i] << std::endl;
    }
}

float normalization(float val, float min_val, float max_val) {
    if (min_val == max_val) return val;

    return (val - min_val) / (max_val - min_val);
}

float millisecondsToSeconds(int milliseconds) {
    return static_cast<float>(milliseconds) / 1000;
}

void Tooltip(const Rectangle& boundary, const Font& font, const ScreenSize& screen, const std::string& information) {
    float rect_h{ 35.0F };
    float rect_w{};
    float font_size = rect_h * 0.8F;
    float font_space = 0.5F;
    float space = 10;
    Vector2 text_measure = MeasureTextEx(font, information.c_str(), font_size, font_space);
    rect_w = text_measure.x + (space * 3.F);
    float center = boundary.x + boundary.width / 2;

    Rectangle tip_panel{
        0,
        boundary.y - space - rect_h,
        rect_w,
        rect_h
    };

    tip_panel.x = center - (tip_panel.width / 2);

    if (tip_panel.x < space) tip_panel.x = space;
    else if (tip_panel.x + tip_panel.width > screen.w) {
        tip_panel.x = screen.w - (tip_panel.width + space);
    }
    else if (tip_panel.y < space) tip_panel.y = boundary.y + boundary.height + space;

    Vector2 text_coor{
        tip_panel.x + (tip_panel.width - text_measure.x) / 2,
        tip_panel.y + (tip_panel.height - text_measure.y) / 2,
    };

    Color color = { 20, 20, 20, 240 };
    DrawRectangleRounded(tip_panel, 0.25F, 10, color);
    DrawTextEx(font, information.c_str(), text_coor, font_size, font_space, RAYWHITE);

}

void NotificationTool(const Rectangle& base_boundary, const Font& font, const std::string& info, float& info_timer, float dt) {
    float rect_h{ 40.0F };
    float rect_w{};
    float font_size{ rect_h * 0.8F };
    float font_space{ 0.5F };
    float space{ 10 };
    Vector2 text_measure = MeasureTextEx(font, info.c_str(), font_size, font_space);
    float center = base_boundary.x + base_boundary.width / 2;
    rect_w = text_measure.x + (space * 3.F);

    Rectangle notification_panel{
        0,
        base_boundary.y + 100,
        rect_w,
        rect_h
    };

    notification_panel.x = center - (notification_panel.width / 2);
    Vector2 text_coor{
        notification_panel.x + (notification_panel.width - text_measure.x) / 2,
        notification_panel.y + (notification_panel.height - text_measure.y) / 2,
    };
    Color color = { 50, 50, 50, 255 };

    if (info_timer > 0) {
        float alpha = 1.0F;
        if (info_timer < 1) alpha = (info_timer * info_timer);
        DrawRectangleRounded(notification_panel, 0.25F, 10, Fade(color, alpha * 0.9F));
        DrawTextEx(font, info.c_str(), text_coor, font_size, font_space, Fade(RAYWHITE, alpha));
    }

    info_timer -= dt;
}

Color GrayscaleColor(float normalizedValue) {
    Color startColor = { 20, 20, 25, 255 }; // Black
    Color endColor = { 255, 245, 245, 255 }; // White

    float value = normalizedValue;
    //float value = normalizedValue * normalizedValue;
    //float value = normalizedValue * normalizedValue * normalizedValue;
    //float value = sqrtf(normalizedValue);

    // Interpolate between the start and end color based on the normalized value
    Color resultColor;
    resultColor.r = static_cast<unsigned char>(startColor.r + value * (endColor.r - startColor.r));
    resultColor.g = static_cast<unsigned char>(startColor.g + value * (endColor.g - startColor.g));
    resultColor.b = static_cast<unsigned char>(startColor.b + value * (endColor.b - startColor.b));
    //resultColor.a = startColor.a + normalizedValue * (endColor.a - startColor.a);
    resultColor.a = 200;

    return resultColor;
}

Color SpectrogramColor(float normalizedValue) {
    // Clamp value to 0-1 range
    normalizedValue = std::clamp(normalizedValue, 0.0f, 1.0f);

    // Initialize color channels
    float red = 0.0f, green = 0.0f, blue = 0.0f, alpha = 240.0F;

    if (normalizedValue > 0.8f) {
        // White
        red = 255.0f;
        green = 255.0f;
        blue = 255.0f;
        alpha = 255.0f;
    }
    else if (normalizedValue > 0.65f) {
        // ALMOST WHITE
        red = 245.0f;
        green = 235.0f;
        blue = 235.0f;
        alpha = 255.0f;
    }
    else if (normalizedValue > 0.55f) {
        // ALMOST WHITE
        red = 235.0f;
        green = 225.0f;
        blue = 225.0f;
        alpha = 255.0f;
    }
    else if (normalizedValue > 0.45f) {
        // Yellow
        red = 225.0f;
        green = 235.0f;
        blue = 140.0f;
        alpha = 200.0f;
    }
    //else if (normalizedValue > 0.35f) {
    //    // Orange
    //    red = 218.0f;
    //    green = 165.0f;
    //    blue = 90.0f;
    //    alpha = 150.0F;
    //}
    else if (normalizedValue > 0.35f) {
        // Orange
        red = 180.0f;
        green = 160.0f;
        blue = 130.0f;
        alpha = 180.0F;
    }
    else if (normalizedValue > 0.25f) {
        // Pink
        red = 110.0f;
        green = 110.0f;
        blue = 135.0f;
        alpha = 180.0F;
    }
    else if (normalizedValue > 0.15f) {
        // Light Purple
        red = 70.0f;
        green = 60.0f;
        blue = 110.0f;
        alpha = 180.0F;
    }
    else if (normalizedValue > 0.1f) {
        // Dark Purple
        red = 45.0f;
        green = 40.0f;
        blue = 80.0f;
        alpha = 150.0F;
    }
    else if (normalizedValue > 0.05f) {
        // Dark Purple 35, 22, 59
        red = 35.0f;
        green = 20.0f;
        blue = 70.0f;
        alpha = 120.0F;
    }
    else {
        // Zero Value
        red = 21;
        green = 21;
        blue = 22;
    }

    // Return the color as raylib Color
    return Color{ (unsigned char)red, (unsigned char)green, (unsigned char)blue, (unsigned char)alpha };
}

// Define gradient colors
Color gradientColors[MAX_GRADIENT_COLORS] = {
    Color {  0,   0, 255, 255},   // Blue
    Color {  0, 255, 255, 255},   // Cyan
    Color {  0, 255,   0, 255},   // Green
    Color {255, 255,   0, 255},   // Yellow
    Color {255,   0,   0, 255}    // Red
};

// Function to linearly interpolate between two colors
Color ColorLerp(Color color1, Color color2, float amount) {
    return Color {
        (unsigned char)((color1.r * (1 - amount)) + (color2.r * amount)),
        (unsigned char)((color1.g * (1 - amount)) + (color2.g * amount)),
        (unsigned char)((color1.b * (1 - amount)) + (color2.b * amount)),
        (unsigned char)((color1.a * (1 - amount)) + (color2.a * amount))
    };
}

// Function to map a value between 0 and 1 to a color in the gradient
Color getColorFromValue(float value) {
    if (value <= 0) return gradientColors[0];
    if (value >= 1) return gradientColors[MAX_GRADIENT_COLORS - 1];

    int colorIndex = static_cast<int>(value) * (MAX_GRADIENT_COLORS - 1);
    float colorPercentage = (value * (MAX_GRADIENT_COLORS - 1)) - colorIndex;

    Color color1 = gradientColors[colorIndex];
    Color color2 = gradientColors[colorIndex + 1];

    return ColorLerp(color1, color2, colorPercentage);
}

Color getColorFromAmplitude(float normalizedAmplitude) {
    // Map normalized amplitude to brightness (0 to 255)
    unsigned char brightness = (unsigned char)(normalizedAmplitude * 255);

    return Color { brightness, brightness, brightness, 255 }; // RGB components set to brightness
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

void DrawVisualTimeDomainProgress(Rectangle& panel, float progress_w);

void DrawMedia(Rectangle& panel_media);

void DrawVolume(Rectangle& panel_playpause, float button_panel);

void DrawMusicList(Rectangle& panel_left, int& retFlag);

std::string TrimDisplayString(std::string& cpp_text, float text_width_limit, float font_size, float font_space, int TrimString);

void DeleteMusic(int& retFlag, size_t order);

void ResetVisualizerParameter();

void DrawMainDisplay(Rectangle& panel_main);

void DrawLockButton(Rectangle& panel_main, float dt);

void DrawVisualModeButton(Rectangle& panel_main, float dt);

void DrawFullscreenButton(Rectangle& panel_main, float dt);

void DrawMusicPlayModeButton(Rectangle& panel_main, float dt);

void DrawMusicProgress(Rectangle& panel_progress, float& music_volume);

bool is_Draw_Icons();

void DrawDragDropPage(ScreenSize screen);

bool Check_StartUp_Page();

void InitializedSpectrogram();

void DrawSplashScreen();

void DrawDragDropPopupTray();

//void InitializedSpectrogram(std::unique_ptr<Color[], std::default_delete<Color[]>>& spectrogram_data);

void InitializedSpectrogramZoneOut();

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
    int downsampling_rate = int(total_frames / 10000);
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
Font font_visual_mode{};
Font font_visual_mode_child{};

Texture2D TIRAKAT_ICON_TEX{};
Texture2D PLAYPAUSE_TEX{};
Texture2D FULLSCREEN_TEX{};
Texture2D VOLUME_TEX{};
Texture2D SETTING_TEX{};
Texture2D DELETE_TEX{};
Texture2D X_TEX{};
Texture2D MODE_TEX{};
Texture2D POINTER_TEX{};
Texture2D LOCK_TEX{};

std::ostringstream formatted_duration{};
std::ostringstream formatted_progress{};

int music_time_now{};
int music_duration{};
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

ScreenSize screen{};

float volume{};

auto spectrogram_data = std::make_unique<Color[]>(p->spectrogram_w * p->spectrogram_h);
std::vector<unsigned char> spectrogram_gray_color(p->spectrogram_w * p->spectrogram_h);
std::vector<Vector2> pointsArray_RealTime_smart(BUCKETS);
std::vector<Vector2> pointsArray_Norm_smart(BUCKETS);
std::vector<Vector2> splines_pointer_smart(BUCKETS);

std::vector<Color> spectrogram_zone_out(p->spectrogram_zone_out_w * p->spectrogram_h);
std::vector<Vector2> audio_wave_live(wave_live.size());

int main()
{
    //landscape_splines.reserve(BUCKETS * 60);

    std::cout << std::setprecision(3);
    std::cout << "Hello World!\n";
    std::cout << "RAYLIB VERSION: " << RAYLIB_VERSION << std::endl;

    screen = { 1000, 600 };

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    //SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    //SetConfigFlags(FLAG_VSYNC_HINT);
    //SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    //SetConfigFlags(FLAG_WINDOW_MOUSE_PASSTHROUGH);

    InitWindow((int)screen.w, (int)screen.h, "Tirakat");
    InitAudioDevice();
    SetTargetFPS(96);
    //SetTargetFPS(60);
    SetWindowIcon(LoadImage(ICON_APP_LOC));
    //ToggleBorderlessWindowed();
    //SetWindowOpacity(0.75F);

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

    font_visual_mode = LoadFontEx(FONT_LOC_Sofia_Sans_Condensed_BOLD, 60, 0, 0);
    SetTextureFilter(font_visual_mode.texture, RL_TEXTURE_FILTER_BILINEAR);

    font_visual_mode_child = LoadFontEx(FONT_LOC_Sofia_Sans_Condensed_REG, 60, 0, 0);
    SetTextureFilter(font_visual_mode_child.texture, RL_TEXTURE_FILTER_BILINEAR);

    Image tirakat_icon = LoadImage(ICON_APP_LOC);
    TIRAKAT_ICON_TEX = LoadTextureFromImage(tirakat_icon);
    SetTextureFilter(TIRAKAT_ICON_TEX, TEXTURE_FILTER_BILINEAR);

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

    Image pointer_icon = LoadImage(ICON_POINTER_LOC);
    POINTER_TEX = LoadTextureFromImage(pointer_icon);
    SetTextureFilter(POINTER_TEX, TEXTURE_FILTER_BILINEAR);

    Image lock_icon = LoadImage(ICON_LOCK_LOC);
    LOCK_TEX = LoadTextureFromImage(lock_icon);
    SetTextureFilter(LOCK_TEX, TEXTURE_FILTER_BILINEAR);

    p->circle = LoadShader(NULL, "resources/shaders/circle.fs");
    p->bubble = LoadShader(NULL, "resources/shaders/bubble.fs");

    InitializedSpectrogram();
    InitializedSpectrogramZoneOut();

    FileCheck(data_txt);

    if (Check_StartUp_Page()) {
        ReloadVector();

        music = LoadMusicStream(data.at(music_play).path.c_str());
        p->music_channel = music.stream.channels;
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
        SetWindowMinSize(1000, 600);

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
        ClearBackground(Fade(BASE_COLOR, 0.5F));

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

        //DrawFPS(screen.w - 83, 10);
        DrawFPS(screen.w / 2 - 38, 10);

        DrawDragDropPopupTray();

        DrawSplashScreen();

        EndDrawing();
    }

    UnloadShader(p->circle);
    UnloadShader(p->bubble);
    
    cleanup();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

void DrawDragDropPopupTray()
{
    if (!p->DragDropPopupTray.empty()) {

        for (size_t i = 0; i < p->DragDropPopupTray.size(); i++) {
            DragDropPopup& tray = p->DragDropPopupTray.at(i);

            tray.updateAll();

            float popup_w = 225;
            float popup_h = 60;
            float space = 15;
            float alpha = tray.alpha;
            float coef_moving = sqrtf(tray.getSlideUp());
            Rectangle popup_rect{
                screen.w - popup_w - space,
                screen.h - 0 - space - (coef_moving * popup_h) - (i * (popup_h + space)),
                popup_w,
                popup_h
            };
            Color color_bg = (tray.getInfo() == true) ? DARKGREEN : RED;
            DrawRectangleRounded(popup_rect, 0.2F, 10, Fade(color_bg, alpha));

            if (tray.getInfo())
            {
                // Draw Text
                font = &font_s_reg;
                Color font_color = WHITE;
                std::string text_cpp = "Success load \"" + tray.name + "\"";
                // trim text_cpp sesuai dengan space
                float font_size = popup_rect.height * 0.5F;
                float font_space = 0.0F;
                float width_text = popup_rect.width * 0.9F;
                text_cpp = TrimDisplayString(text_cpp, width_text, font_size, font_space, EASY);
                const char* text = text_cpp.c_str();
                Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
                Vector2 text_coor{
                    popup_rect.x + (popup_rect.width - text_measure.x) / 2,
                    popup_rect.y + (popup_rect.height - text_measure.y) / 2
                };
                DrawTextEx(*font, text, text_coor, font_size, font_space, Fade(font_color, alpha));
            }
            else
            {
                // Draw Text
                font = &font_s_reg;
                Color font_color = WHITE;
                std::string text_cpp = "Could not load \"" + tray.name + "\"";
                // trim text_cpp sesuai dengan space
                float font_size = popup_rect.height * 0.5F;
                float font_space = 0.0F;
                float width_text = popup_rect.width * 0.9F;
                text_cpp = TrimDisplayString(text_cpp, width_text, font_size, font_space, EASY);
                const char* text = text_cpp.c_str();
                Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
                Vector2 text_coor{
                    popup_rect.x + (popup_rect.width - text_measure.x) / 2,
                    popup_rect.y + (popup_rect.height - text_measure.y) / 2
                };
                DrawTextEx(*font, text, text_coor, font_size, font_space, Fade(font_color, alpha));
            }

        }

        if (p->DragDropPopupTray.back().isExpired()) p->DragDropPopupTray.pop_back();

    }
}

void DrawSplashScreen()
{
    // Draw Splash Screen
    double splash_screen_time = 7.0;
    static bool draw_splash_screen = true;
    if (draw_splash_screen) {
        float splash_screen_w = 250;
        float splash_screen_h = 90;
        float space = 20;
        static float alpha = 1.0F;
        Rectangle splash_screen_rect{
            screen.w - splash_screen_w - space,
            screen.h - splash_screen_h - space,
            splash_screen_w,
            splash_screen_h
        };
        Color color_bg = { 16, 16, 16, 255 };
        Color color_line = { 224,229,124,255 };

        if (GetTime() > splash_screen_time) alpha -= 0.005F;

        if (alpha <= 0.0F) draw_splash_screen = false;

        DrawRectangleRounded(splash_screen_rect, 0.225F, 10, Fade(color_bg, 1.0F * alpha));
        DrawRectangleRoundedLines(splash_screen_rect, 0.225F, 10, 3.0F, Fade(color_line, 1.0F * alpha));

        // Draw Content
        {
            float space = 10.F;
            Rectangle dest{
                splash_screen_rect.x + space,
                splash_screen_rect.y + space,
                splash_screen_rect.height - (space * 2),
                splash_screen_rect.height - (space * 2)
            };
            DrawRectangleRoundedLines(dest, 0.15F, 10, 1.0F, Fade(LIGHTGRAY, alpha));
            Rectangle source{ 0,0,200,200 };
            DrawTexturePro(TIRAKAT_ICON_TEX, source, dest, { 0,0 }, 0, Fade(WHITE, alpha));

            // Draw Made By
            Rectangle text_rect{
                dest.x + dest.width + space,
                dest.y,
                (splash_screen_rect.x + splash_screen_rect.width) - (dest.x + dest.width) - (space * 2),
                dest.height
            };
            //DrawRectangleRoundedLines(text_rect, 0.1F, 10, 1.0F, Fade(LIGHTGRAY, alpha));

            float rect_space = -0.025F;
            float rect_space_top = 0.3F;
            float rect_space_bot = 1 - rect_space - rect_space_top;
            Rectangle text_rect_top{
                text_rect.x,
                text_rect.y,
                text_rect.width,
                text_rect.height * rect_space_top
            };
            //DrawRectangleRoundedLines(text_rect_top, 0.1F, 10, 1.0F, Fade(LIGHTGRAY, alpha));

            {
                font = &font_s_reg;
                Color font_color = WHITE;
                const char* text = "MADE BY";
                float font_size = text_rect_top.height * 1.4F;
                float font_space = 0.0F;
                Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
                Vector2 text_coor{
                    text_rect_top.x + (text_rect_top.width - text_measure.x) / 2,
                    text_rect_top.y + (text_rect_top.height - text_measure.y) / 2
                };
                DrawTextEx(*font, text, text_coor, font_size, font_space, Fade(font_color, alpha));
            }


            // Draw UFTHaq
            Rectangle text_rect_bot{
                text_rect.x,
                text_rect.y + (text_rect.height * rect_space_top) + (text_rect.height * rect_space),
                text_rect.width,
                text_rect.height * rect_space_bot
            };
            //DrawRectangleRoundedLines(text_rect_bot, 0.1F, 10, 1.0F, Fade(LIGHTGRAY, alpha));

            {
                font = &font_s_bold;
                Color font_color = WHITE;
                const char* text = "UFTHaq";
                float font_size = text_rect_bot.height * 1.2F;
                float font_space = 0.0F;
                Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
                Vector2 text_coor{
                    text_rect_bot.x + (text_rect_bot.width - text_measure.x) / 2,
                    text_rect_bot.y + (text_rect_bot.height - text_measure.y) / 2
                };
                DrawTextEx(*font, text, text_coor, font_size, font_space, Fade(font_color, alpha));
            }
        }
    }
}


void InitializedSpectrogram()
{
    for (size_t i = 0; i < p->spectrogram_h; i++) {
        for (size_t j = 0; j < p->spectrogram_w; j++) {
            spectrogram_data[i * p->spectrogram_w + j] = Color{
                static_cast<unsigned char>(j * 255 / p->spectrogram_w),
                static_cast<unsigned char>(i * 255 / p->spectrogram_h),
                static_cast<unsigned char>(i * 255 / p->spectrogram_h),
                //255 // Full color initialized
                0 // Full transparent
            };
        }
    }

    p->spectrogram_image = {
        spectrogram_data.get(),
        p->spectrogram_w,
        p->spectrogram_h,
        1,
        PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    p->SPECTROGRAM_TEXTURE = LoadTextureFromImage(p->spectrogram_image);
}

void InitializedSpectrogramZoneOut() 
{
    std::vector<uint8_t> alpha_space(p->spectrogram_zone_out_w);

    for (size_t i = 0; i < p->spectrogram_zone_out_w; i++) {
        alpha_space.at(i) = static_cast<uint8_t>(i);
    }

    for (size_t i = 0; i < p->spectrogram_h; i++) {
        for (size_t j = 0; j < p->spectrogram_zone_out_w; j++) {
            spectrogram_zone_out.at(i * p->spectrogram_zone_out_w + j) =
            {
                10,
                10,
                10,
                //static_cast<uint8_t>(256 - alpha_space.at(j))
                static_cast<uint8_t>(256 - alpha_space.at(j))
            };
        }
    }

    p->spectrogram_zone_out_image = {
        spectrogram_zone_out.data(),
        p->spectrogram_zone_out_w,
        p->spectrogram_h,
        1,
        PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    p->SPECTROGRAM_ZONE_OUT_TEXTURE = LoadTextureFromImage(p->spectrogram_zone_out_image);

    p->spectrogram_zone_in_image = ImageCopy(p->spectrogram_zone_out_image);
    ImageFlipHorizontal(&p->spectrogram_zone_in_image);

    p->SPECTROGRAM_ZONE_IN_TEXTURE = LoadTextureFromImage(p->spectrogram_zone_in_image);
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

    volume = GetMasterVolume();

    if (p->mouse_cursor == MOUSE_CURSOR_POINTING_HAND) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }
    else if (p->mouse_cursor == MOUSE_CURSOR_IBEAM) {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
    }
    else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    music_duration = data.at(music_play).duration;
    //music_duration = static_cast<int>(GetMusicTimeLength(music) * 1000);

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
            float music_move_to = millisecondsToSeconds(music_time_now) - 5.0F;
            if (music_move_to < 0.1F) music_move_to = 0.1F;
            SeekMusicStream(music, music_move_to);
            //std::cout << "LEFT ARROW : -5 SECONDS" << std::endl;
            interval_time = INTERVAL;

            std::string info = "-5 seconds";
            p->notification.g_info = info;
            p->notification.g_info_timer = HUD_TIMER_SECS;
        }

    }
    else if (IsKeyDown(KEY_RIGHT)) {

        if (interval_time > 0.0F) {
            interval_time -= GetFrameTime();
        }
        else {
            float music_move_to = millisecondsToSeconds(music_time_now) + 5.0F;
            if (music_move_to > millisecondsToSeconds(music_duration)) music_move_to = millisecondsToSeconds(music_duration) - 0.1F;
            SeekMusicStream(music, music_move_to);
            //std::cout << "RIGHT ARROW : +5 SECONDS" << std::endl;
            interval_time = INTERVAL;

            std::string info{ "+5 seconds" };
            p->notification.g_info = info;
            p->notification.g_info_timer = HUD_TIMER_SECS;
        }
    }
    else if (IsKeyDown(KEY_UP)) {
        if (interval_time > 0.0F) {
            interval_time -= GetFrameTime();
        }
        else {
            volume += 0.05F;
            if (volume > 1) volume = 1;
            SetMasterVolume(volume);

            std::string info{ std::to_string(static_cast<int>(volume * 100)) + "%" };
            p->notification.g_info = info;
            p->notification.g_info_timer = HUD_TIMER_SECS;
            interval_time = INTERVAL;
        }
    }
    else if (IsKeyDown(KEY_DOWN)) {
        if (interval_time > 0.0F) {
            interval_time -= GetFrameTime();
        }
        else {
            volume -= 0.05F;
            if (volume < 0) volume = 0;
            SetMasterVolume(volume);

            std::string info{ std::to_string(static_cast<int>(volume * 100)) + "%" };
            p->notification.g_info = info;
            p->notification.g_info_timer = HUD_TIMER_SECS;
            interval_time = INTERVAL;
        }
    }
    else {
        interval_time = 0;
    }

    // Important to re rise after each moving the progress time to play the sound smooth with no sudden change in buffer sound.
    float set_music_volume = 1.0F;
    static float music_volume = 0.5F;
    if (music_volume < set_music_volume) music_volume += 0.02F;

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

        int threshold_80 = static_cast<int>(music_duration * 0.8F);
        //std::cout << time_played << " : " << music_time << std::endl;

        // COUNTING REPETITION
        static bool repetition_saved = false;
        if (repetition_saved == false) {

            if (music_time_now >= (music_duration - 10)) {
            //if (GetMusicTimePlayed(music) >= (millisecondsToSeconds(music_duration) - 0.05F)) {

                PauseMusicStream(music);

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
                    p->music_channel = music.stream.channels;
                    AttachAudioStreamProcessor(music.stream, callback);

                    time_domain_signal = ExtractMusicData(data.at(music_play).path);
                }

                ResumeMusicStream(music);

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
        music_time_now = static_cast<int>(GetMusicTimePlayed(music) * 1000);
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

        if (p->mouse_onscreen_timer > 0.0F) {
            p->mouse_onscreen_timer -= GetFrameTime();
            p->mouse_onscreen = ON;
        }
        else {
            p->mouse_onscreen = OFF;
        }

        Vector2 delta = GetMouseDelta();
        bool moved = fabsf(delta.x) + fabsf(delta.y) > 0.0;
        if (moved) p->mouse_onscreen_timer = HUD_TIMER_SECS;

        float panel_progress_height = PANEL_PROGRESS_HEIGHT;
        if (p->mouse_onscreen == OFF && p->visual_time_domain_lock == OFF) panel_progress_height = 3.0F;
        

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

        if (p->mouse_onscreen == ON || p->visual_time_domain_lock == ON) {
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
        
        // DRAWING PANEL MUSIC LIST
        DrawMusicList(panel_music_list, retFlag);

        // DRAWING PANEL MAIN
        DrawMainDisplay(panel_main);

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


        // SPECIAL CASE TOOLTIP
        if (special_btn_setting.enable == ON && CheckCollisionPointRec(mouse_position, special_btn_setting.rect)) {
            std::string info{ "Reset Counter" };
            Tooltip(special_btn_setting.rect, font_visual_mode_child, screen, info);
        } 

        if (special_btn_delete.enable == ON && CheckCollisionPointRec(mouse_position, special_btn_delete.rect)) {
            std::string info{ "Delete Music" };
            Tooltip(special_btn_delete.rect, font_visual_mode_child, screen, info);
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

void DrawVisualTimeDomainProgress(Rectangle& panel, float progress_w)
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
    
        if (x1 <= progress) { 
            //color = SKYBLUE;
            color = { 225, 225, 225, 255 };
            alpha = 1.0F;
        }
        else {
            color = DARKGRAY;
            alpha = 0.8F;
        }
    
        DrawLineEx({ x1,y1 }, { x2,y2 }, 1.5F, Fade(color, alpha));
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
    volume = { GetMasterVolume() };
    bool volume_btn_clicked{ false };
    //static bool HUD_toggle = false;
    static bool HUD_toggle = true;

    float volume_slider_length_base = 170.0F;
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

    Color icon_color = GRAY;
    if (CheckCollisionPointRec(mouse_position, panel_volume)) {
        icon_color = RAYWHITE;
        HUD_toggle = true;

        std::string info{};
        if (volume > 0) info = "Mute [M]";
        else info = "Unmute [M]";
        Tooltip(panel_volume, font_visual_mode_child, screen, info);

        if (CheckCollisionPointRec(mouse_position, panel_volume) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            volume_btn_clicked = true;
        }
    }


    if (IsKeyPressed(KEY_TOGGLE_MUTE) || volume_btn_clicked) {
        if (volume > 0) {
            p->last_volume = volume;
            volume = 0;
        }
        else {
            volume = p->last_volume;
        }
        SetMasterVolume(volume);
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
        //DrawRectangleRec(volume_slider_panel, RED);

        // SLIDER DRAW - START
        float padding_slider = 50.0F;
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

            std::string info{ "Volume " + std::to_string(static_cast<int>(volume * 100)) + "%"};
            Tooltip(volume_slider_panel, font_visual_mode_child, screen, info);

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
    //bool outVolumeBase = !(CheckCollisionPointRec(mouse_position, panel_volume_base));
    //if (outVolumeBase) {
    //    //HUD_toggle = false;
    //}

    //if (p->dragging == DRAG_VOLUME) {
    //    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && outVolumeBase) {
    //        HUD_toggle = true;
    //    }
    //    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && outVolumeBase) {
    //        HUD_toggle = false;
    //        p->dragging = DRAG_RELEASE;
    //    }
    //}
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

                            // Reset music_time_now sebelum pindah lagu biar tidak overflow ke lagu selanjutnya
                            music_time_now = 1; // milliseconds

                            music_play = i;
                            music = LoadMusicStream(data.at(music_play).path.c_str());
                            p->music_channel = music.stream.channels;
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

        //font = &font_s_semibold;
        font = &font_visual_mode_child;
        float text_width_limit = content.width - 30.0F;
        //float font_size = content_h * 0.475F;
        float font_size = content_h * 0.5F;
        float font_space = -0.25F;
        //float font_space = 0.0F;

        std::string cpp_text = data.at(i).name;

        std::string first_10 = TrimDisplayString(cpp_text, text_width_limit, font_size, font_space, BOLD);
        const char* text = first_10.c_str();
        Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
        Vector2 text_coor = {
            content.x + 12.0F,
            content.y + (content.height - text_measure.y) / 2,
        };
        DrawTextEx(*font, text, text_coor, font_size, font_space, color_font);


        if (p->option_status == ON) 
        {

            static float time_down = {};
            float dt = GetFrameTime();

            if (i == p->option_music_order) {
                bool special_author = true;
                std::string author_lower = "author";
                std::string subtring_name_lower = cpp_text;
                std::transform(subtring_name_lower.begin(), subtring_name_lower.end(), subtring_name_lower.begin(), tolower);
                size_t found = subtring_name_lower.find(author_lower);

                if (found == std::string::npos) {

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

                    special_btn_setting = { ON, setting_btn };
                    special_btn_delete = { ON, delete_btn };
                }

            }

            if (time_down >= 0.0F) time_down -= dt;
            if (time_down <= 0.0F) if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) p->option_status = OFF;
            
            
        }
        else {
            special_btn_setting = { OFF, {} };
            special_btn_delete = { OFF, {} };
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

std::string TrimDisplayString(std::string& cpp_text, float text_width_limit, float font_size, float font_space, int TrimString)
{
    float text_width = 0.0F;
    int max_chars = 0;
    while (text_width < text_width_limit && max_chars < cpp_text.length()) {
        text_width = MeasureTextEx(*font, cpp_text.substr(0, max_chars + 1).c_str(), font_size, font_space).x;
        max_chars++;
    }

    std::string result{};
    if (TrimString == EASY) {
        if (text_width > text_width_limit) {
            std::string titik_titik = "..";
            return  result = cpp_text.substr(0, (max_chars - titik_titik.size())) + titik_titik;
        }
        else {
            return result = cpp_text.substr(0, max_chars);
        }
    }
    else if (TrimString == BOLD) {
        return result = cpp_text.substr(0, max_chars);
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
    }

    if (music_play == order) {
        if (order != 0) music_play--;

        if (data.size() > 0) {
            // SAVE TO TXT AGAIN
            DetachAudioStreamProcessor(music.stream, callback);
            ResetVisualizerParameter();

            music = LoadMusicStream(data.at(music_play).path.c_str());
            p->music_channel = music.stream.channels;
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
        //dc_offset(fftw_in.data());
        //hann_window(fftw_in.data(), N);
        hamming_window(fftw_in.data(), N);
        //gaussian_window(fftw_in.data(), N);
        //low_pass_filter(in, N);
        //fir_low_pass_filter(in, N, 1000);
    }
    fftw_calculation(fftw_in.data(), fftw_out.data(), N);

    for (size_t i = 0; i < BUCKETS; i++) {
        Spectrum.at(i) = 0.0F;
    }

    float min_amp = std::numeric_limits<float>::max();  // Or a very large positive value
    float max_amp = std::numeric_limits<float>::min();  // Or a very large negative value
    for (size_t i = 0; i < N / 2; i++) {
        float real_num = static_cast<float>(fftw_out[i][0]);
        float imaginer = static_cast<float>(fftw_out[i][1]);

        float amplitude = std::sqrt((real_num * real_num) + (imaginer * imaginer));

        min_amp = std::min(min_amp, amplitude);
        max_amp = std::max(max_amp, amplitude);
    }

    for (size_t i = 0; i < N / 2; i++) {
        float real_num = static_cast<float>(fftw_out[i][0]);
        float imaginer = static_cast<float>(fftw_out[i][1]);

        float amplitude = std::sqrt((real_num * real_num) + (imaginer * imaginer));

        for (size_t j = 0; j < BUCKETS; j++) {
            float freq = min_frequency + i * bin_width;

            if (freq >= Freq_Bin.at(j) && freq <= Freq_Bin.at(j + 1)) {
                Spectrum.at(j) = std::max(Spectrum.at(j), amplitude);

                if (amplitude > Peak.at(j).amplitude) {
                    Peak.at(j).amplitude = amplitude;
                    Peak.at(j).frequency_index = static_cast<int>(i);
                }

            }

        }

    }

    float dt = GetFrameTime();
    
    for (size_t i = 0; i < BUCKETS; i++) {
        for (size_t j = SMOOTHING_BUFFER_SIZE - 1; j > 0; --j) {
            prevAmplitude.at(i).at(j) = prevAmplitude.at(i).at(j - 1);
        }

        float norm_amplitude = Spectrum.at(i);

        prevAmplitude.at(i).at(0) = norm_amplitude;

        smoothedAmplitude.at(i) = calculateMovingAverage(prevAmplitude.at(i), SMOOTHING_BUFFER_SIZE);

        maxAmplitude = std::max(maxAmplitude, smoothedAmplitude.at(i));
    }

    // SPLINE INITIALIZATION
    /*std::unique_ptr<Vector2[]> pointsArray_RealTime_smart(new Vector2[BUCKETS]);
    std::unique_ptr<Vector2[]> pointsArray_Norm_smart(new Vector2[BUCKETS]);*/

    for (size_t i = 0; i < BUCKETS; i++) {
        float final_amplitude = smoothedAmplitude.at(i);

        switch (p->mode)
        {
        case MODE_NATURAL:
            smoothedAmplitude.at(i) = natural_scale(final_amplitude, 0.02F);
            break;
        case MODE_EXPONENTIAL:
            smoothedAmplitude.at(i) = exponential_scale(final_amplitude, 0.5F);
            break;
        case MODE_MULTI_PEAK:
            smoothedAmplitude.at(i) = multi_peak_scale(final_amplitude, static_cast<int>(i), 1.0F, Peak);
            break;
        case MODE_MAX_PEAK:
            smoothedAmplitude.at(i) = max_peak_scale(final_amplitude, maxAmplitude, 0.2F);
            break;
        default:
            break;
        }

        float smearness = 8;
        out_smear.at(i) += (smoothedAmplitude.at(i) - out_smear.at(i)) * smearness * GetFrameTime();
    }

    if (p->visual_mode_active == CLASSIC || p->visual_mode_active == GALAXY || p->visual_mode_active == LANDSCAPE) 
    {
        // JUST FOR DRAWING
        for (size_t i = 0; i < BUCKETS; i++) {
            float final_amplitude = smoothedAmplitude.at(i);
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
            float hue = (float)i / BUCKETS * 0.9F;
            float sat = 0.85F;
            float val = 1.0F;
            color = ColorFromHSV(hue * 360, sat, val);


            Vector2 startPos = { bar.x + bar.width / 2, (bar.y + bar.height) - bar_h };
            Vector2 endPos = { bar.x + bar.width / 2, (bar.y + bar.height) };
            float thick = 4.0F * sqrt(final_amplitude) * (bar_w * 0.10F);


            Vector2 center_bins = { bar.x + bar.width / 2, bar.y };
            float radius = bar_w * sqrt(final_amplitude) * 1.25F * 2;
            pointsArray_RealTime_smart[i] = center_bins;


            if (p->visual_mode_active == CLASSIC) {
                p->mode = MODE_MULTI_PEAK;

                bar_h *= 1.25F;
                startPos = { bar.x + bar.width / 2, (bar.y + bar.height) - bar_h };
                endPos = { bar.x + bar.width / 2, (bar.y + bar.height) };
                DrawLineEx(startPos, endPos, thick, color);

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

                {
                    // TRY SMEAR EFFECT
                    Texture2D smear_texture = { rlGetTextureIdDefault(), 1,1,1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };

                    BeginShaderMode(p->circle);
                    float start = out_smear.at(i) * panel_display.height * 0.65F;;
                    float end = bar_h;

                    Vector2 start_pos = startPos;
                    Vector2 end_pos = endPos;

                    float radius = bar_w * sqrt(final_amplitude) * 2.F;
                    Vector2 origin{};
                    if (end_pos.y >= start_pos.y) {
                        Rectangle dest{
                            start_pos.x - radius / 2,
                            start_pos.y,
                            radius,
                            end_pos.y - start_pos.y
                        };
                        Rectangle source{ 0, 0, 1, 0.5 };
                        DrawTexturePro(smear_texture, source, dest, origin, 0, color);
                    }
                    else {
                        Rectangle dest{
                            end_pos.x - radius / 2,
                            end_pos.y,
                            radius,
                            start_pos.y - end_pos.y
                        };
                        Rectangle source{ 0, 0.5, 1, 0.5 };
                        DrawTexturePro(smear_texture, source, dest, origin, 0, color);
                    }

                    EndShaderMode();
                }

                // DRAW CIRCLE USING SHADERS
                BeginShaderMode(p->circle);
                Texture2D circle_texture = { rlGetTextureIdDefault(), 1,1,1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
                radius = bar_w * sqrt(final_amplitude) * 1.50F * 2;

                // TOP CIRCLE
                Vector2 top_pos = {
                    startPos.x - radius,
                    startPos.y - radius
                };
                float rotation = { 0 };
                float scale = radius * 2;
                DrawTextureEx(circle_texture, top_pos, rotation, scale, color);

                // BASE CIRCLE
                radius = radius * 0.70F;
                scale = radius * 2;
                Vector2 base_pos = {
                    endPos.x - radius,
                    endPos.y - radius
                };
                DrawTextureEx(circle_texture, base_pos, rotation, scale, color);
                EndShaderMode();
            }


            if (p->visual_mode_active == GALAXY) {
                p->mode = MODE_EXPONENTIAL;

                BeginShaderMode(p->circle);
                Texture2D circle_texture = { rlGetTextureIdDefault(), 1,1,1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };

                float rotation = { 0 };

                // NEW FFT ROTATION STYLE, USE LINE AND SHADERS
                color = ColorFromHSV(hue * 360 + GetFrameTime(), sat, val);
                Vector2 center_panel_main{
                    panel_display.x + (panel_display.width * 0.5F),
                    panel_display.y + (panel_display.height * 0.58F)
                };

                float value = sqrt(final_amplitude) * panel_display.height * 0.42F;
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
                    float radius_40 = sqrt(value) * 1.25F;
                    float scale = radius_40 * 2;
                    Vector2 base_40_pos = {
                        Pos_40.x - radius_40,
                        Pos_40.y - radius_40,
                    };
                    DrawTextureEx(circle_texture, base_40_pos, rotation, scale, color);
                    //DrawTextureEx(circle_texture, base_40_pos, rotation, scale, WHITE);

                    //float radius_70 = value * 0.05F;
                    float radius_70 = 2.50F;
                    scale = radius_70 * 2;
                    Vector2 base_70_pos = {
                        Pos_70.x - radius_70,
                        Pos_70.y - radius_70,
                    };
                    //DrawTextureEx(circle_texture, base_70_pos, rotation, scale, color);
                    DrawTextureEx(circle_texture, base_70_pos, rotation, scale, WHITE);

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
                    DrawTextureEx(circle_texture, base_100_pos, rotation, scale, color);
                    //DrawTextureEx(circle_texture, base_100_pos, rotation, scale, WHITE);

                }
                EndShaderMode();
            }

        }
    }

    if (p->visual_mode_active == LANDSCAPE) {
        p->mode = MODE_NATURAL;

        // Make Rectangle
        Rectangle base{ panel_display };
        int JUMLAH_RECT = 100;
        //int JUMLAH_RECT = 150;
        //float coef_rect = 0.955F;
        float coef_rect = 0.96F;
        std::deque<Rectangle> landscape_rects{};

        for (size_t i = 0; i < JUMLAH_RECT; i++) {
            Rectangle edited = {
                base.x + ((base.width - base.width * coef_rect) / 2),
                base.y + base.height * 0.025F * coef_rect,
                base.width * coef_rect,
                base.height * coef_rect
            };

            landscape_rects.push_back(edited);
            
            // Inverse
            //landscape_rects.push_front(edited);
            //coef_rect += 0.00015F;
            coef_rect += 0.00009F;

            base = edited;

            //DrawRectangleLinesEx(edited, 1.0F, RED);
        }

        //std::unique_ptr<Vector2[]> spline_pointer_smart(new Vector2[BUCKETS]);

        static float time_check{};
        time_check += dt;
        float frame_rate_capture = 40; // FPS
        float time_to_capture{ 1.0F / frame_rate_capture };
        if (time_check >= time_to_capture) {

            std::vector<Vector2> points{};
            for (size_t i = 0; i < BUCKETS; i++) {
                points.push_back(pointsArray_Norm_smart[i]);
            }

            if (landscape_splines.size() >= JUMLAH_RECT) {
                landscape_splines.pop_back();
            }
            landscape_splines.push_front(points);

            time_check = 0;
        }

        float thick = 1.0F;
        for (size_t i = 0; i < landscape_splines.size(); i++) {
            Rectangle rect = landscape_rects.at(i);

            for (size_t j = 0; j < BUCKETS; j++) {
                splines_pointer_smart.at(j) = {
                    rect.x + rect.width * landscape_splines.at(i)[j].x,
                    rect.y + rect.height * landscape_splines.at(i)[j].y
                };
            }
            DrawSplineLinear(splines_pointer_smart.data(), BUCKETS, 1.5F * thick, LIGHTGRAY);
            DrawSplineLinear(splines_pointer_smart.data(), BUCKETS, 6.0F * thick, Fade(SKYBLUE, thick / 4));

            // Inverse
            //DrawSplineLinear(spline_pointer_smart.data(), BUCKETS, 4.0F * (1-thick), Fade(SKYBLUE, (1 - thick) / 4));
            //DrawSplineLinear(spline_pointer_smart.data(), BUCKETS, 2.F * (1 - thick), LIGHTGRAY);

            //DrawSplineBasis(spline_pointer_smart.data(), BUCKETS, thick, LIGHTGRAY);
            //DrawRectangleLinesEx(rect, .4F, BLUE);

            thick *= 0.97F;
        }

        Color color = BLUE;

        DrawSplineCatmullRom(pointsArray_RealTime_smart.data(), BUCKETS, 15.0F, Fade(color, 0.1F));
        DrawSplineCatmullRom(pointsArray_RealTime_smart.data(), BUCKETS, 13.0F, Fade(color, 0.15F));
        DrawSplineCatmullRom(pointsArray_RealTime_smart.data(), BUCKETS, 9.0F, Fade(color, 0.2F));
        DrawSplineCatmullRom(pointsArray_RealTime_smart.data(), BUCKETS, 7.0F, Fade(color, 0.25F));
        DrawSplineCatmullRom(pointsArray_RealTime_smart.data(), BUCKETS, 3.5F, Fade(WHITE, 1.0F));
    }

    if (p->visual_mode_active == SPECTROGRAM) {
        float space_from_top = panel_display.height * 0.15F;
        Rectangle spectrogram_base_panel{
            panel_display.x,
            panel_display.y + space_from_top,
            panel_display.width,
            panel_display.height - space_from_top
        };

        static float dest_w{};
        static float dest_h{};
        float pad = 35.0F;
        if (spectrogram_base_panel.width < spectrogram_base_panel.height) {
            dest_w = std::round(spectrogram_base_panel.width - (pad * 2));
            dest_h = std::round(dest_w * (9 / 16.0F));

        }
        else {
            dest_h = spectrogram_base_panel.height - (pad * 2);
            dest_w = dest_h * (16.0F / 9);
            if (dest_w > spectrogram_base_panel.width) {
                dest_w = std::round(spectrogram_base_panel.width - (pad * 2));
                dest_h = std::round(dest_w * (9 / 16.0F));
            }
        }

        Rectangle dest{
            spectrogram_base_panel.x + (spectrogram_base_panel.width - dest_w) / 2,
            spectrogram_base_panel.y + (spectrogram_base_panel.height - dest_h) / 2,
            dest_w,
            dest_h
        };
        
        Rectangle source{
            0,
                0,
                static_cast<float>(p->spectrogram_w),
                static_cast<float>(p->spectrogram_h)
        };

        static int time_skip = 0;
        time_skip += (int)dt;
        //if (time_skip % 40 == 0)
        {
            // 
            float min_amp_spec = std::numeric_limits<float>::max();  // Or a very large positive value
            float max_amp_spec = std::numeric_limits<float>::min();  // Or a very large negative value

            for (size_t i = 0; i < p->spectrogram_h; i++) {
                float real_num_spec = static_cast<float>(fftw_out[i][0]);
                float imaginer_spec = static_cast<float>(fftw_out[i][1]);
                float amplitude_spc = std::sqrt((real_num_spec * real_num_spec) + (imaginer_spec * imaginer_spec));
                min_amp_spec = std::min(min_amp_spec, amplitude_spc);
                max_amp_spec = std::max(max_amp_spec, amplitude_spc);
            }


            int speed = 1;
            if (p->music_playing) {

                for (size_t y = 0; y < p->spectrogram_h; y++) {
                    std::memmove(&spectrogram_data[y * p->spectrogram_w], &spectrogram_data[y * p->spectrogram_w + speed], (p->spectrogram_w - speed) * sizeof(Color));

                    float real_num_spec = static_cast<float>(fftw_out[y][0]);
                    float imaginer_spec = static_cast<float>(fftw_out[y][1]);
                    float amplitude_spc = std::sqrt((real_num_spec * real_num_spec) + (imaginer_spec * imaginer_spec));


                    amplitude_spc = normalization(amplitude_spc, min_amp_spec, max_amp_spec);
                    if (amplitude_spc < 0.1F) amplitude_spc = 0.0F;
                    int inverse = p->spectrogram_h - static_cast<int>(y);
                    //spectrogram_data[y * p->spectrogram_w + (p->spectrogram_w - speed)] = interpolateColor(amplitude_spc); // Terbalik
                    spectrogram_data[inverse * p->spectrogram_w + (p->spectrogram_w - speed)] = SpectrogramColor(amplitude_spc);
                }

            }

        }

        Color tint = WHITE;
        UpdateTexture(p->SPECTROGRAM_TEXTURE, spectrogram_data.get());
        DrawTexturePro(p->SPECTROGRAM_TEXTURE, source, dest, { 0,0 }, 0, tint);

        // DRAW ZONE OUT
        {
            Rectangle dest_zone_out{
                dest.x - 1,
                dest.y,
                dest.width * 0.025F,
                dest.height
            };

            Rectangle source_zone_out{
                0,
                0,
                static_cast<float>(p->spectrogram_zone_out_w),
                static_cast<float>(p->spectrogram_h)
            };

            Color tint = WHITE;
            DrawTexturePro(p->SPECTROGRAM_ZONE_OUT_TEXTURE, source_zone_out, dest_zone_out, { 0,0 }, 0, Fade(tint, 0.5F));
        }

        // DRAW ZONE IN
        {

            Rectangle dest_zone_in{
                dest.x + dest.width - (dest.width * 0.025F) + 1,
                dest.y,
                (dest.width * 0.025F),
                dest.height
            };

            Rectangle source_zone_out{
                0,
                0,
                static_cast<float>(p->spectrogram_zone_out_w),
                static_cast<float>(p->spectrogram_h)
            };

            Color tint = WHITE;
            DrawTexturePro(p->SPECTROGRAM_ZONE_IN_TEXTURE, source_zone_out, dest_zone_in, { 0,0 }, 0, Fade(tint, 0.5F));
        }


    }

    if (p->visual_mode_active == WAVE) 
    {
        if (p->toggle_windowed_wave && p->music_playing) {
            hann_window(wave_live.data(), wave_live.size());
        }

        float pad = panel_display.height * 0.2F;
        Rectangle wave_live_signal_base
        {
            panel_display.x + (pad * 0.5F),
            panel_display.y + (pad * 1.25F),
            panel_display.width - (pad * 2 * 0.5F),
            panel_display.height - (pad * 2 * 1.25F) + (pad * 0.5F)
        };
        //DrawRectangleLinesEx(wave_live_signal_base, 1.0F, YELLOW);

        pad = wave_live_signal_base.height * 0.05F;
        Rectangle wave_live_signal_rect
        {
            wave_live_signal_base.x,
            wave_live_signal_base.y + (pad * 1),
            wave_live_signal_base.width,
            wave_live_signal_base.height - (pad * 2),
        };
        //DrawRectangleRec(wave_live_signal_rect, RAYWHITE);

        float center_hor = wave_live_signal_rect.y + (wave_live_signal_rect.height / 2);
        float point_width = wave_live_signal_rect.width / wave_live.size();
        for (size_t i = 0; i < audio_wave_live.size(); i++) {
            audio_wave_live.at(i) = {
                wave_live_signal_rect.x + (point_width * i),
                center_hor + (wave_live_signal_rect.height * wave_live.at(i)) * 0.4F,
            };
        }

        DrawSplineLinear(audio_wave_live.data(), (int)wave_live.size(), 3.F, RAYWHITE);

        static float alpha_coef{1.0f};
        bool draw_icon = alpha_coef > 0.0F;
        static float time_down{};

        float toggle_btn_size = 50.0F;
        Rectangle toggle_btn
        {
            wave_live_signal_base.x + (wave_live_signal_base.width - toggle_btn_size) / 2,
            wave_live_signal_base.y,
            toggle_btn_size,
            toggle_btn_size
        };

        float pad_button = 35.0F;
        Rectangle hover_area
        {
            toggle_btn.x - (pad_button * 1),
            toggle_btn.y - (pad_button * 1),
            toggle_btn.width + (pad_button * 2),
            toggle_btn.height + (pad_button * 2),
        };

        if (CheckCollisionPointRec(mouse_position, hover_area)) {
            if (alpha_coef <= 1.0F) {
                alpha_coef += sqrtf(dt);
                }
            }
        else {
            if (alpha_coef >= 0.0F) {
                alpha_coef -= sqrtf(dt) / 4;
            }
        }

        DrawRectangleRounded(toggle_btn, 0.25F, 10, Fade(LIGHTGRAY, 0.10F * alpha_coef));

        if (CheckCollisionPointRec(mouse_position, toggle_btn)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                p->toggle_windowed_wave = !p->toggle_windowed_wave;
            }

            std::string info{};
            if (p->toggle_windowed_wave) info = "Windowed Wave [W]";
            else info = "Natural Wave [W]";
            Tooltip(toggle_btn, font_visual_mode_child, screen, info);
        }

        Color color{};
        if (p->toggle_windowed_wave) color = GREEN;
        else color = MAROON;

        Vector2 center{ toggle_btn.x + toggle_btn.width / 2, toggle_btn.y + toggle_btn.height / 2 };
        DrawCircleLinesV(center, toggle_btn_size * 0.250F, Fade(color, alpha_coef));
        DrawCircleLinesV(center, toggle_btn_size * 0.230F, Fade(color, alpha_coef));
        DrawCircleLinesV(center, toggle_btn_size * 0.220F, Fade(color, alpha_coef));
        DrawCircleLinesV(center, toggle_btn_size * 0.210F, Fade(color, alpha_coef));

        if (IsKeyPressed(KEY_W)) p->toggle_windowed_wave = !p->toggle_windowed_wave;
    }

    if (p->visual_mode_active != WAVE)
    {
        if (p->music_playing) {
            hann_window(wave_live.data(), wave_live.size());
        }

        // AUDIO WAVE LIVE
        float width = panel_display.height * 0.3F;
        float height = width * 0.6F;
        Rectangle wave_live_signal_base{
            panel_display.x + (panel_display.width - width) / 2,
            panel_display.y + 50,
            width,
            height
        };
        //DrawRectangleRec(wave_live_signal_base, RAYWHITE);

        float pad = wave_live_signal_base.height * 0.05F;
        Rectangle wave_live_signal_rect{
            wave_live_signal_base.x,
            wave_live_signal_base.y + (pad * 1),
            wave_live_signal_base.width,
            wave_live_signal_base.height - (pad * 2),
        };
        //DrawRectangleRec(wave_live_signal_rect, RAYWHITE);

        float center_hor = wave_live_signal_rect.y + (wave_live_signal_rect.height / 2);
        float point_width = wave_live_signal_rect.width / wave_live.size();
        for (size_t i = 0; i < audio_wave_live.size(); i++) {
            audio_wave_live.at(i) = {
                wave_live_signal_rect.x + (point_width * i),
                center_hor + (wave_live_signal_rect.height * wave_live.at(i)) * 0.4F,
                //center_hor + (wave_live_signal_rect.height * static_cast<float>(fftw_in[i][0])) * 0.5F,
            };
        }

        DrawSplineLinear(audio_wave_live.data(), (int)wave_live.size(), 2.F, RAYWHITE);

        if (IsKeyPressed(KEY_P)) {
            for (auto& i : wave_live) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
            std::cout << wave_live.size() << std::endl;
        }

    }


    // PANEL LOCK VISUAL TIME DOMAIN
    DrawLockButton(panel_main, dt);
    
    // DRAW PLAY MODE BUTTON
    DrawMusicPlayModeButton(panel_main, dt);

    // DRAW FULLSCREEN BUTTON
    DrawFullscreenButton(panel_main, dt);

    // DRAW VISUAL MODE BUTTON
    DrawVisualModeButton(panel_main, dt);

    // DRAW NOTIFICATION
    NotificationTool(panel_main, font_visual_mode_child, p->notification.g_info, p->notification.g_info_timer, dt);


}

void DrawLockButton(Rectangle& panel_main, float dt)
{
    float panel_lock_area_hover_w = panel_main.height / 4;
    Rectangle panel_lock_area_hover{
        panel_main.x + (panel_main.width - panel_lock_area_hover_w) / 2,
        panel_main.y + panel_main.height - panel_lock_area_hover_w,
        panel_lock_area_hover_w,
        panel_lock_area_hover_w
    };
    //DrawRectangleRec(panel_lock_area_hover, RED);
    //if (p->mouse_onscreen == ON && (CheckCollisionPointRec(mouse_position, panel_lock_area_hover))) {
    //    Rectangle 
    //}
    static float alpha_coef{};
    if (CheckCollisionPointRec(mouse_position, panel_lock_area_hover)) {
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
    static float time_down{};

    if (draw_icon && p->fullscreen) {
        p->mouse_onscreen_timer = HUD_TIMER_SECS;
        float lock_btn_size = 50.0F;
        float space = 5.0F;
        Rectangle lock_panel{
            panel_main.x + (panel_main.width - lock_btn_size) / 2,
            panel_main.y + panel_main.height - (lock_btn_size + space - PANEL_LINE_THICK),
            lock_btn_size,
            lock_btn_size
        };
        float pad = 5.0F;
        Rectangle lock_btn{
            lock_panel.x + (pad * 1),
            lock_panel.y + (pad * 1),
            lock_panel.width - (pad * 2),
            lock_panel.height - (pad * 2),
        };
        DrawRectangleRounded(lock_btn, 0.25F, 10, Fade(LIGHTGRAY, 0.20F * alpha_coef));

        Color icon_color{};
        if (p->visual_time_domain_lock == OFF) {
            if (CheckCollisionPointRec(mouse_position, lock_btn)) {
                icon_color = WHITE;

                std::string info = "Lock Visual Time Domain [L]";
                Tooltip(lock_btn, font_visual_mode_child, screen, info);

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && (time_down <= 0.0F)) {
                    p->visual_time_domain_lock = ON;
                    time_down = 0.1F;
                }
            }
            else {
                icon_color = LIGHTGRAY;
            }
        }
        else {
            if (CheckCollisionPointRec(mouse_position, lock_btn)) {
                icon_color = WHITE;

                std::string info = "Unlock Visual Time Domain [L]";
                Tooltip(lock_btn, font_visual_mode_child, screen, info);

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && (time_down <= 0.0F)) {
                    p->visual_time_domain_lock = OFF;
                    time_down = 0.1F;
                }
            }
            else {
                icon_color = LIGHTGRAY;
            }
        }

        p->icon_lock_index = p->visual_time_domain_lock;

        float icon_size = 100.0F;
        Rectangle dest{ lock_btn };
        Rectangle source{ p->icon_lock_index * icon_size, 0, icon_size, icon_size };
        DrawTexturePro(LOCK_TEX, source, dest, { 0,0 }, 0, Fade(icon_color, 1.0F * alpha_coef));
    }

    if (IsKeyPressed(KEY_L) && time_down <= 0.0F) {
        p->visual_time_domain_lock = !p->visual_time_domain_lock;
        time_down = 0.1F;
    }

    if (time_down > 0.0F) time_down -= dt;
}

void DrawVisualModeButton(Rectangle& panel_main, float dt)
{
    float visual_mode_hover_size = panel_main.height / 4;
    float space = 10;
    Rectangle visual_mode_button_area_hover{
        panel_main.x + panel_main.width - (visual_mode_hover_size + space),
        panel_main.y + space,
        visual_mode_hover_size,
        visual_mode_hover_size
    };
    static float alpha_coef{};
    if (p->fullscreen) {
        if (CheckCollisionPointRec(mouse_position, visual_mode_button_area_hover)) {
            if (alpha_coef <= 1.0F) {
                alpha_coef += sqrtf(dt);
            }
        }
        else {
            if (alpha_coef >= 0.0F) {
                alpha_coef -= sqrtf(dt) / 4;
            }
        }
    }
    else alpha_coef = 1.0F;

    bool draw_button = alpha_coef > 0.0F;

    if (draw_button) {
        font = &font_visual_mode;
        float visual_mode_btn_width = 150;
        float visual_mode_btn_height = visual_mode_btn_width * 0.275F;
        float space = 10.0F;
        Color button_color = { 69, 69, 69, 255 };
        Rectangle visual_mode_panel{
            panel_main.x + panel_main.width - (visual_mode_btn_width + space),
            panel_main.y + space + 2,
            visual_mode_btn_width,
            visual_mode_btn_height
        };
    
        if ((p->visual_mode_expand == ON) || (CheckCollisionPointRec(mouse_position, visual_mode_panel))) {
            DrawRectangleRounded(visual_mode_panel, 0.2F, 10, Fade(button_color, 0.4F * alpha_coef));
        }
    
        const char* text = "Visual Mode";
        float font_coef = 0.65F;
        float font_size = visual_mode_panel.height * font_coef;
        float font_space = 0.75F;
        Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
        Vector2 text_coor = {
            visual_mode_panel.x + space * 1.25F,
            visual_mode_panel.y + (visual_mode_panel.height - text_measure.y) / 2
        };
        if (CheckCollisionPointRec(mouse_position, visual_mode_panel) || p->visual_mode_expand == ON) {
            DrawTextEx(*font, text, text_coor, font_size, font_space, Fade(WHITE, alpha_coef));
        }
    
        float icon_width = visual_mode_panel.height;
        Rectangle icon_panel{
            visual_mode_panel.x + visual_mode_panel.width - icon_width,
            visual_mode_panel.y,
            icon_width,
            icon_width
        };
        float pad = 7.0F;
        Rectangle icon_rect{
            icon_panel.x + (pad * 1),
            icon_panel.y + (pad * 1),
            icon_panel.width - (pad * 2),
            icon_panel.height - (pad * 2),
        };
    
        Color icon_color{ LIGHTGRAY };
        size_t visual_mode_icon_index{};
    
        if (p->visual_mode_expand == OFF) visual_mode_icon_index = 0;
        else visual_mode_icon_index = 1;

        float icon_size = 100.0F;
        Rectangle dest{ icon_rect };
        Rectangle source{ visual_mode_icon_index * icon_size, 0, icon_size, icon_size };
        DrawTexturePro(POINTER_TEX, source, dest, { 0,0 }, 0, Fade(icon_color, 1.0F * alpha_coef));

        // Toggle visual_mode_expand state
        if (CheckCollisionPointRec(mouse_position, visual_mode_panel)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                p->visual_mode_expand = !p->visual_mode_expand;
            }
        }
        
        float visual_mode_expand_base_w = visual_mode_panel.width * 1.0F;
        float visual_mode_expand_base_h = visual_mode_panel.height * 0.8F;
        if (p->visual_mode_expand == ON) {
            alpha_coef = 1.0F;
            Rectangle visual_mode_expand_base{
                visual_mode_panel.x + (visual_mode_panel.width - visual_mode_expand_base_w) / 2,
                visual_mode_panel.y + visual_mode_panel.height + space,
                visual_mode_expand_base_w,
                visual_mode_expand_base_h * p->visualmode.size()
            };
            DrawRectangleRounded(visual_mode_expand_base, 0.1F, 10, Fade(button_color, 0.15F));

            // DRAWING ALL VISUAL MODE CHILD
            for (size_t i = 0; i < p->visualmode.size(); i++) {
                VisualMode visual_mode_child = p->visualmode.at(i);
                Color active_color{ 105, 220, 57, 255 };

                Rectangle visual_mode_child_panel{
                    visual_mode_expand_base.x,
                    visual_mode_expand_base.y + (i * visual_mode_expand_base_h),
                    visual_mode_expand_base.width,
                    visual_mode_expand_base_h,
                };

                float pad = 3.0F;
                Rectangle visual_mode_active{
                    visual_mode_child_panel.x,
                    visual_mode_child_panel.y + (pad * 1),
                    visual_mode_child_panel.width,
                    visual_mode_child_panel.height - (pad * 2)
                };

                // HOVER && ACTIVE MODE
                if (p->visual_mode_active == i) {
                    DrawRectangleRounded(visual_mode_active, 0.1F, 10, Fade(active_color, 0.5F));
                }
                else if (CheckCollisionPointRec(mouse_position, visual_mode_active) && (visual_mode_child.enable == ON)) {
                    DrawRectangleRounded(visual_mode_active, 0.1F, 10, Fade(active_color, 0.5F));

                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        p->visual_mode_active = i;
                        p->visual_mode_expand = OFF;
                    }
                }

                Color font_color = WHITE;
                if (visual_mode_child.enable == OFF) font_color = DARKGRAY;

                font = &font_visual_mode_child;

                // VISUAL MODE TITLE
                const char* text = visual_mode_child.title.c_str();
                float font_size = visual_mode_active.height * 0.9F;
                float font_space = 0.0F;
                space = 8.0F;
                Vector2 text_measure = MeasureTextEx(*font, text, font_size, font_space);
                Vector2 text_coor = {
                    visual_mode_active.x + space,
                    visual_mode_active.y + (visual_mode_active.height - text_measure.y) / 2,
                };
                DrawTextEx(*font, text, text_coor, font_size, font_space, font_color);

                // VISUAL MODE SHORTCUT
                text = visual_mode_child.shortcut.c_str();
                text_measure = MeasureTextEx(*font, text, font_size, font_space);
                text_coor = {
                    visual_mode_active.x + visual_mode_active.width - (text_measure.x + space),
                    visual_mode_active.y + (visual_mode_active.height - text_measure.y) / 2,
                };
                DrawTextEx(*font, text, text_coor, font_size, font_space, font_color);
    
            }
    
        }

        Rectangle visual_mode_panel_base{
            visual_mode_panel.x,
            visual_mode_panel.y,
            visual_mode_expand_base_w,
            visual_mode_panel.height + space + visual_mode_expand_base_h * 4
        };
        //DrawRectangleRounded(visual_mode_panel_base, 0.1F, 10, Fade(LIGHTGRAY, 0.05F));
        
        // Close expanded panel if click outside visual_mode_panel_base
        if (!CheckCollisionPointRec(mouse_position, visual_mode_panel_base)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                p->visual_mode_expand = OFF;
            }
        }

    }
    
    // VISUAL MODE SHORTCUT KEY
    if (IsKeyDown(KEY_VISUAL_MODE) && IsKeyPressed(KEY_ONE)) {
        if (p->visualmode.at(WAVE).enable == ON) {
            p->visual_mode_active = WAVE;
        }
    }
    else if (IsKeyDown(KEY_VISUAL_MODE) && IsKeyPressed(KEY_TWO)) {
        if (p->visualmode.at(CLASSIC).enable == ON) {
            p->visual_mode_active = CLASSIC;
        }
    }
    else if (IsKeyDown(KEY_VISUAL_MODE) && IsKeyPressed(KEY_THREE)) {
        if (p->visualmode.at(GALAXY).enable == ON) {
            p->visual_mode_active = GALAXY;
        }
    }
    else if (IsKeyDown(KEY_VISUAL_MODE) && IsKeyPressed(KEY_FOUR)) {
        if (p->visualmode.at(LANDSCAPE).enable == ON) {
            p->visual_mode_active = LANDSCAPE;
        }
    }
    else if (IsKeyDown(KEY_VISUAL_MODE) && IsKeyPressed(KEY_FOUR)) {
        if (p->visualmode.at(SPECTROGRAM).enable == ON) {
            p->visual_mode_active = SPECTROGRAM;
        }
    }

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
            alpha_coef += sqrtf(dt);
        }
    }
    else {
        if (alpha_coef >= 0.0F) {
            alpha_coef -= sqrtf(dt) / 4;
        }
    }

    bool draw_icon = alpha_coef > 0.0F;
    static float time_down{};

    if (draw_icon) {
        p->mouse_onscreen_timer = HUD_TIMER_SECS;
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
        if (p->fullscreen == OFF) {
            if (CheckCollisionPointRec(mouse_position, fullscreen_btn)) {
                p->icon_fullscreen_index = 1;
                icon_color = WHITE;

                std::string info = "Expand [F]";
                Tooltip(fullscreen_btn, font_visual_mode_child, screen, info);

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

                std::string info = "Collapse [F]";
                Tooltip(fullscreen_btn, font_visual_mode_child, screen, info);

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


        float icon_size = 100.0F;
        Rectangle dest{ fullscreen_btn };
        Rectangle source{ p->icon_fullscreen_index * icon_size, 0, icon_size, icon_size };
        DrawTexturePro(FULLSCREEN_TEX, source, dest, { 0,0 }, 0, Fade(icon_color, 1.0F * alpha_coef));
    }

    if (IsKeyPressed(KEY_FULLSCREEN) && time_down <= 0.0F) {
        p->fullscreen = !p->fullscreen;
        time_down = 0.1F;
    }

    if (time_down > 0.0F) time_down -= dt;


}

void DrawMusicPlayModeButton(Rectangle& panel_main, float dt)
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
        p->mouse_onscreen_timer = HUD_TIMER_SECS;
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

            std::string info{};
            if (p->repeat == ON) info = "Repeat";
            else info = "Loop";
            Tooltip(play_mode_btn, font_visual_mode_child, screen, info);

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

    float progress_ratio = static_cast<float>(panel_progress.width) / music_duration;
    static float progress_w = 0;
    if (p->dragging != DRAG_MUSIC_PROGRESS) {
        progress_w = progress_ratio * music_time_now;
    }

    Rectangle progress_bar{
        panel_progress.x,
        panel_progress.y,
        progress_w,
        panel_progress.height
    };
    Color progress_color_hover = PANEL_PROGRESS_COLOR;

    // DRAW TIME DOMAIN VISUAL OR DRAW TIME BAR ONLY
    if (p->fullscreen) {
        if (p->mouse_onscreen == OFF && p->visual_time_domain_lock == OFF) DrawRectangleRec(progress_bar, progress_color_hover);
        else DrawVisualTimeDomainProgress(panel_progress, progress_w);
    }
    else DrawVisualTimeDomainProgress(panel_progress, progress_w);

    if (p->dragging != DRAG_VOLUME) {
        if (CheckCollisionPointRec(mouse_position, panel_progress)) {

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                float t = (mouse_position.x - panel_progress.x) / panel_progress.width;
                SeekMusicStream(music, (t * music_duration / 1000));
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

        music_time_now = static_cast<int>(progress_w / progress_ratio);

        if (music_time_now < 0) {
            music_time_now = 1;
            progress_w = 0;
        }
        else if (music_time_now > music_duration) {
            music_time_now = music_duration;
            progress_w = progress_ratio * music_time_now;
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
        SeekMusicStream(music, (t * music_duration / 1000));
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
            p->DragDropPopupTray.emplace_front(file_name, true);
            for (auto& i : p->DragDropPopupTray) i.resetSlideUp();

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
                    p->music_channel = music.stream.channels;
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
            p->DragDropPopupTray.emplace_front(file_name, false); // use emplace front not push front because it construct the object directly, while push is contruct then move or copy, double step.
            for (auto& i : p->DragDropPopupTray) i.resetSlideUp();
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

        std::string info{};
        if (p->music_playing == ON) info = "Pause [Space]";
        else info = "Play [Space]";
        Tooltip(hover_panel, font_visual_mode_child, screen, info);

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

    if (music_duration < 3600 * 1000) {
        int minutes_dur = music_duration / (60 * 1000);
        int seconds_dur = (music_duration / 1000) % 60;

        int minutes_pro = music_time_now / (60 * 1000);
        int seconds_pro = (music_time_now / 1000) % 60;

        formatted_duration << std::setw(2) << std::setfill('0') << minutes_dur << ":" << std::setw(2) << std::setfill('0') << seconds_dur;
        formatted_progress << std::setw(2) << std::setfill('0') << minutes_pro << ":" << std::setw(2) << std::setfill('0') << seconds_pro;
    }
    else {
        int hour_dur = music_duration / (3600 * 1000);
        int minutes_dur = (music_duration / (60 * 1000)) % 60;
        int seconds_dur = (music_duration / 1000) % 60;

        int hour_pro = music_time_now / (3600 * 1000);
        int minutes_pro = (music_time_now / (60 * 1000)) % 60;
        int seconds_pro = (music_time_now / 1000) % 60;

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
    if (!data.empty()) {

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
}

void DrawTitleMP3(Rectangle& panel)
{
    if (!data.empty()) {

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
