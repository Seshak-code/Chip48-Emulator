#include <iostream>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
#include <unordered_map>
#include "cpu.cpp"
#include <chrono>


using namespace std;

class Chip8Emulator 
{
public:
    Chip8Emulator() 
    {
        cout << "CHIP8 Started!" << endl;
        cout << "Initializing SDL!" << endl;

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            cerr << "SDL Initialization Failed!" << endl;
            exit(EXIT_FAILURE); // Handle the failure using exit
        }

        create_window();


        renderer = SDL_CreateRenderer(window, -1, 0);

        if (renderer == nullptr) {
            const char* err = SDL_GetError();
            cerr << err << endl;
            cerr << "SDL Renderer Initialization Failed!" << endl;
            exit(EXIT_FAILURE); // Handle the failure using exit
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
        if (texture == nullptr) {
            cerr << "SDL Texture Creation Failed!" << endl;
            exit(EXIT_FAILURE); // Handle the failure using exit
        }
    }

    ~Chip8Emulator() 
    {
        SDL_DestroyWindow(window);
        window = nullptr;

        cout << "Quitting SDL!" << endl;
        SDL_Quit();
        cout << "CHIP8 Exiting!" << endl;
    }

    void clear_window()
    {
        SDL_RenderClear(renderer);
    }

    void copy_render()
    {
        SDL_RenderCopy(renderer, texture, NULL, NULL);
    }

    void present_render()
    {
        SDL_RenderPresent(renderer);
    }

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;

    void create_window()
    {
        window = SDL_CreateWindow("CHIP8 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_SHOWN);
    }

};

int main() {
    Chip8Emulator emulator;
    Chip8 cpu;
    //cout << __cplusplus << endl;
    bool running = true;
    int duration_ms = 16;
    auto duration = chrono::milliseconds(duration_ms);

    unordered_map<int, int> keymap;
        keymap[SDLK_1, 0x1];
        keymap[SDLK_2, 0x2]; 
        keymap[SDLK_3, 0x3]; 
        keymap[SDLK_4, 0xC];
        keymap[SDLK_q, 0x4]; 
        keymap[SDLK_w, 0x5]; 
        keymap[SDLK_e, 0x6]; 
        keymap[SDLK_r, 0xD];
        keymap[SDLK_a, 0x7]; 
        keymap[SDLK_s, 0x8]; 
        keymap[SDLK_d, 0x9]; 
        keymap[SDLK_f, 0xE];
        keymap[SDLK_z, 0xA]; 
        keymap[SDLK_x, 0x0]; 
        keymap[SDLK_c, 0xB]; 
        keymap[SDLK_v, 0xF];
        keymap[SDLK_5, 0x5]; 
        keymap[SDLK_6, 0x6]; 
        keymap[SDLK_7, 0x7];
        keymap[SDLK_8, 0x8]; 
        keymap[SDLK_9, 0x9]; 
        keymap[SDLK_0, 0x0]; 
        keymap[SDLK_ESCAPE, -1]; 
    
    cpu.init();

    for(int i = 0; i < 8; i++) 
        cpu.rpl_user_flags[i] = rand() & 0x3F;

    while(running)
    {
        // Emulation Cycle

        SDL_Event event;
        while( SDL_PollEvent(&event) > 0 )
        {
            switch(event.type)
            {
                case SDL_QUIT:
                {
                    //cout << "quit";
                    running = false;
                }

                default:
                    //cout << "failure";
                
            }

            emulator.clear_window();

            // Build Texture

            emulator.copy_render();
            emulator.present_render();

            this_thread::sleep_for(duration);

                
        }
    }



    return 0;

}
