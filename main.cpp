#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include </Users/seshak/Desktop/chip8/include/SDL2/SDL.h>
#include <unordered_map>
#include "cpu.cpp"
#include <vector>


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
        SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        window = nullptr;
        cout << "Quitting SDL!" << endl;
        SDL_Quit();
        cout << "CHIP8 Exiting! 😵" << endl;
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

    SDL_Texture* getSDL_Texture ()
    {
        return texture;
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

void loadROM(const char* filename, Chip8 &cpu)  // Implement file opener
{
    std::ifstream rom(filename, std::ios::binary);

    if (rom.is_open()) {
        rom.seekg(0, std::ios::end);
        std::streampos size = rom.tellg();
        rom.seekg(0, std::ios::beg);

        cout << "Loading ROM: " << filename << endl;    
        if (size > 0 && size <= 0xFFFE00) {
            // Read the ROM directly into Chip-8 memory starting from 0x200
            rom.read(reinterpret_cast<char*>(&cpu.memory[0x200]), size);
        }

        rom.close();
    }
}

void buildTexture(Chip8Emulator &emulator, Chip8 &cpu, Graphics &grhandlr)
{
    uint32_t* bytes = nullptr;
    int pitch = 0;

    SDL_LockTexture(emulator.getSDL_Texture(), nullptr, reinterpret_cast<void**>(&bytes), &pitch);

        //cout << "is it locked in" << endl;

        if(!(grhandlr.extendedScreenMode))
        {
            //cout << "chip8" << endl;
            for (size_t y = 0; y < 32; ++y) {  // Update for CHIP-8 resolution
                for (size_t x = 0; x < 64; ++x) {  // Update for CHIP-8 resolution
                    bytes[y * 64 + x] = (grhandlr.graphics[y * 64 + x] == 1) ? 0xFFFFFFFF : 0x000000FF;
                }
            }
        }
        else
        {
            cout << "chip48" << endl;
            for (size_t y = 0; y < 64; ++y) {  // Update for Super CHIP-48 resolution
                for (size_t x = 0; x < 128; ++x) {  // Update for Super CHIP-48 resolution
                    bytes[y * 128 + x] = (grhandlr.graphics_extended[y * 128 + x] == 1) ? 0xFFFFFFFF : 0x000000FF;
                }
            }
        }


    SDL_UnlockTexture(emulator.getSDL_Texture());

    //cout << "does this work" << endl;

}

const array<int, 16> keymap = {{
    SDL_SCANCODE_X,
    SDL_SCANCODE_1,
    SDL_SCANCODE_2,
    SDL_SCANCODE_3,
    SDL_SCANCODE_Q,
    SDL_SCANCODE_W,
    SDL_SCANCODE_E,
    SDL_SCANCODE_A,
    SDL_SCANCODE_S,
    SDL_SCANCODE_D,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_C,
    SDL_SCANCODE_4,
    SDL_SCANCODE_R,
    SDL_SCANCODE_F,
    SDL_SCANCODE_V
}};

int main() 
{
    // unordered_map<int, int> keymap;
    //     keymap[SDLK_1, 0x1];
    //     keymap[SDLK_2, 0x2]; 
    //     keymap[SDLK_3, 0x3]; 
    //     keymap[SDLK_4, 0xC];
    //     keymap[SDLK_q, 0x4]; 
    //     keymap[SDLK_w, 0x5]; 
    //     keymap[SDLK_e, 0x6]; 
    //     keymap[SDLK_r, 0xD];
    //     keymap[SDLK_a, 0x7]; 
    //     keymap[SDLK_s, 0x8]; 
    //     keymap[SDLK_d, 0x9]; 
    //     keymap[SDLK_f, 0xE];
    //     keymap[SDLK_z, 0xA]; 
    //     keymap[SDLK_x, 0x0]; 
    //     keymap[SDLK_c, 0xB]; 
    //     keymap[SDLK_v, 0xF];
    //     keymap[SDLK_5, 0x5]; 
    //     keymap[SDLK_6, 0x6]; 
    //     keymap[SDLK_7, 0x7];
    //     keymap[SDLK_8, 0x8]; 
    //     keymap[SDLK_9, 0x9]; 
    //     keymap[SDLK_0, 0x0]; 
    //     keymap[SDLK_ESCAPE, -1]; 

    Chip8Emulator emulator;
    Chip8 cpu;
    Graphics grhandlr;
    //cout << __cplusplus << endl;
    bool running = true;
    int duration_ms = 16;
    auto duration = chrono::milliseconds(duration_ms);

    
    cpu.init(grhandlr);
    cout << "init functions ran" << endl;

    for(int i = 0; i < 8; i++) 
        cpu.rpl_user_flags[i] = rand() & 0x3F;



    loadROM("/Users/seshak/Desktop/chip8/test_opcode.ch8", cpu);
    cout << "Emulator cycle begins" << endl;
    while(running)
    {
        auto x = cpu.program_counter; // debugging

        // Emulation Cycle
        SDL_Event event;
        cpu.cycle();

        char hex_string[20];
        if(x != cpu.program_counter)
        {
            sprintf(hex_string, "%X", cpu.current_opcode); //convert number to hex
            cout << "debugging message: 0x" << hex_string << endl; 
        }
        
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

            buildTexture(emulator, cpu, grhandlr); //lol broken

            

            emulator.copy_render();
            emulator.present_render();

            this_thread::sleep_for(duration);

                
        }
    }



    return 0;

}
