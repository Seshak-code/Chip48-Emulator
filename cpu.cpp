#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

//#define EXTENDED (128 * 64)
//#define STANDARD (64 * 32)


#include <iostream>
#include <stdlib.h>
#include <time.h>



class Graphics {
public:

    
    uint8_t graphics[64 * 32];
    uint8_t graphics_extended[128 * 64];
    
    bool extendedScreenMode;

    Graphics() : extendedScreenMode(false)
    {  };

    void clear()
    {
        for (auto &g : graphics) 
            g = 0x00;
        for (auto &g : graphics_extended) 
            g = 0x00;
    }
    
};

class Chip8 {
public:
    // Current Operation Code
    uint16_t current_opcode;

    // Memory Map
    uint8_t memory[4096];

    // Graphics
    Graphics grhandlr;

    // 16 Registers V0-VF
    uint8_t registers[16];

    uint16_t index;
    uint16_t program_counter;
 
    // Timer registers
    uint8_t delay_timer;
    uint8_t sound_timer;

    // Stack and Stack Pointer
    uint16_t stack[32];
    uint16_t sp;

    // Keys
    uint8_t keys[16];

    uint8_t chip8_fontset[80] = 
    {
        0x60, 0xA0, 0xA0, 0xA0, 0xC0, // 0
        0x40, 0xC0, 0x40, 0x40, 0xE0, // 1
        0xC0, 0x20, 0x40, 0x80, 0xE0, // 2
        0xC0, 0x20, 0x40, 0x20, 0xC0, // 3
        0x20, 0xA0, 0xE0, 0x20, 0x20, // 4
        0xE0, 0x80, 0xC0, 0x20, 0xC0, // 5
        0x40, 0x80, 0xC0, 0xA0, 0x40, // 6
        0xE0, 0x20, 0x60, 0x40, 0x40, // 7
        0x40, 0xA0, 0x40, 0xA0, 0x40, // 8
        0x40, 0xA0, 0x60, 0x20, 0x40, // 9
        0x40, 0xA0, 0xE0, 0xA0, 0xA0, // A
        0xC0, 0xA0, 0xC0, 0xA0, 0xC0, // B
        0x60, 0x80, 0x80, 0x80, 0x60, // C
        0xC0, 0xA0, 0xA0, 0xA0, 0xC0, // D
        0xE0, 0x80, 0xC0, 0x80, 0xE0, // E
        0xE0, 0x80, 0xC0, 0x80, 0x80  // F
    };

    uint16_t chip48_fontset[80] = 
    {
        0xC67C, 0xDECE, 0xF6D6, 0xC6E6, 0x007C, // 0
        0x3010, 0x30F0, 0x3030, 0x3030, 0x00FC, // 1
        0xCC78, 0x0CCC, 0x3018, 0xCC60, 0x00FC, // 2
        0xCC78, 0x0C0C, 0x0C38, 0xCC0C, 0x0078, // 3
        0x1C0C, 0x6C3C, 0xFECC, 0x0C0C, 0x001E, // 4
        0xC0FC, 0xC0C0, 0x0CF8, 0xCC0C, 0x0078, // 5
        0x6038, 0xC0C0, 0xCCF8, 0xCCCC, 0x0078, // 6
        0xC6FE, 0x06C6, 0x180C, 0x3030, 0x0030, // 7
        0xCC78, 0xECCC, 0xDC78, 0xCCCC, 0x0078, // 8
        0xC67C, 0xC6C6, 0x0C7E, 0x3018, 0x0070, // 9
        0x7830, 0xCCCC, 0xFCCC, 0xCCCC, 0x00CC, // A
        0x66FC, 0x6666, 0x667C, 0x6666, 0x00FC, // B
        0x663C, 0xC0C6, 0xC0C0, 0x66C6, 0x003C, // C
        0x6CF8, 0x6666, 0x6666, 0x6C66, 0x00F8, // D
        0x62FE, 0x6460, 0x647C, 0x6260, 0x00FE, // E
        0x66FE, 0x6462, 0x647C, 0x6060, 0x00F0  // F
    };

    

    void init() {
        srand(time(0));

        program_counter = 0x200;
        current_opcode = 0x00;
        index = 0x00;
        sp = 0x00;

        // Clear display
        grhandlr.clear();

        // Clear stack
        for (auto &s : stack) {
            s = 0x00;
        }

        // Clear registers
        for (auto &r : registers) {
            r = 0x00;
        }

        // Clear memory
        for (auto &v : memory) {
            v = 0x00;
        }

        // Clear key
        for (auto &k : keys) {
            k = 0x00;
        }

        // Set fonts
        for (int i = 0; i < 80; i++) {
            if(grhandlr.extendedScreenMode)
                memory[i] = chip48_fontset[i];
            else
                memory[i] = chip8_fontset[i];
        }

        delay_timer = 0;
        sound_timer = 0;
    }

    void increment_pc() {
        program_counter += 2;
    }

    void cycle() {
        if (program_counter > 0xFFF) {
            throw "OPcode out of range! Your program has an error!";
        }

        current_opcode = static_cast<uint16_t>(memory[program_counter]) << 8 | memory[program_counter + 1];

        // X000
        uint16_t first = current_opcode >> 12;
        switch (first)
        {
            case 0x0:
                //DEBUG_MSG("SYS INSTR!\n" << first);
                switch(current_opcode)
                {
                    case 0x00C:
                    {
                        // 00CN: Scroll display N lines down
                        uint8_t N = current_opcode & 0x000F;
                        if(grhandlr.extendedScreenMode)
                        {
                            for (int y = 31; y >= N; --y)
                                for (int x = 0; x < 128; ++x)  // Adjusted for extended display mode
                                    grhandlr.graphics_extended[y * 128 + x] = grhandlr.graphics_extended[(y - N) * 128 + x];
                            for (int y = 0; y < N; ++y)
                                for (int x = 0; x < 128; ++x)  // Adjusted for extended display mode
                                    grhandlr.graphics_extended[y * 128 + x] = 0;
                        }
                        else
                        {
                            for (int y = 31; y >= N; --y)
                                for (int x = 0; x < 64; ++x)
                                    grhandlr.graphics[y * 64 + x] = grhandlr.graphics[(y - N) * 64 + x];
                            for (int y = 0; y < N; ++y)
                                for (int x = 0; x < 64; ++x)
                                    grhandlr.graphics[y * 64 + x] = 0;
                        }
                        break;
                    }

                    case 0x00E0:
                        //Clear the display.
                        grhandlr.clear();                        
                        break;

                    case 0x000E:
                    {
                        // 000E: Return from a subroutine
                        if (sp > 0) {
                            --sp;
                            program_counter = stack[sp];
                        } else {
                            throw "Stack underflow! Your program has an error!";
                        }
                        break;
                    }

                    case 0x00FB:
                    {
                        // 00FB: Scroll display 4 pixels right
                        const int scroll = 4;
                        if(grhandlr.graphics_extended)
                        {
                            for (int y = 0; y < 64; ++y)  
                            {
                                for (int x = 127; x >= scroll; --x)  
                                    grhandlr.graphics_extended[y * 128 + x] = grhandlr.graphics_extended[y * 128 + x - scroll];
                                for (int x = 0; x < scroll; ++x)  
                                    grhandlr.graphics_extended[y * 128 + x] = 0;
                            }
                        }
                        else{
                            for (int y = 0; y < 32; ++y) 
                            {
                                for (int x = 63; x >= scroll; --x) 
                                    grhandlr.graphics[y * 64 + x] = grhandlr.graphics[y * 64 + x - scroll];
                                for (int x = 0; x < scroll; ++x) 
                                    grhandlr.graphics[y * 64 + x] = 0;
                            }
                        }
                        break;
                    }

                    case 0x00FC:
                    {
                        // 00FC: Scroll display 4 pixels left
                        const int scroll = 4;
                        if(grhandlr.graphics_extended)
                        {
                            for (int y = 0; y < 64; ++y)  
                            {
                                for (int x = 0; x <= 127 - scroll; ++x)  
                                    grhandlr.graphics_extended[y * 128 + x] = grhandlr.graphics_extended[y * 128 + x + scroll];

                                for (int x = 128 - scroll; x < 128; ++x)  
                                    grhandlr.graphics_extended[y * 128 + x] = 0;
                            }
                        }
                        else{
                            for (int y = 0; y < 32; ++y) 
                            {
                                for (int x = 0; x < 64 - scroll; ++x) 
                                    grhandlr.graphics[y * 64 + x] = grhandlr.graphics[y * 64 + x + scroll];
                                
                                for (int x = 64 - scroll; x < 64; ++x) 
                                    grhandlr.graphics[y * 64 + x] = 0;
                            }
                        }
                        break;
                    }

                    case 0x00FD:
                    {
                        // 00FD: Exit the emulator
                        exit(0);
                    }

                    case 0x00FE:
                    {
                        // 00FE: Disable extended screen mode
                        grhandlr.extendedScreenMode = false;
                        break;
                    }

                    case 0x00FF:
                    {
                        // 00FF: Enable extended screen mode
                        grhandlr.extendedScreenMode = true;
                        break;
                    }

                    default:
                        throw "OPcode is unknown, Your program has an error!";
                        break;
                    
                }
                increment_pc();
                break;

            case 0x1:
                // Jump to location nnn.
                program_counter = current_opcode & 0x0FFF;
                increment_pc();
                break;

            case 0x2:
                // Call subroutine at nnn.
                stack[sp] = program_counter;
                sp++;
                program_counter = current_opcode & 0x0FFF;
                break;

            case 0x3:
                // Skip next instruction if Vx = kk.
                if (registers[(current_opcode & 0x0F00) >> 8] == (current_opcode & 0x00FF))
                    increment_pc();  
                increment_pc();
                break;

            case 0x4:
                // Skip next instruction if Vx != kk.
                if((registers[(current_opcode & 0x0F00) >> 8] != (current_opcode & 0x00FF)))
                    increment_pc();  
                increment_pc();
                break;

            case 0x5:
                // Skip next instruction if Vx = Vy.
                if((registers[(current_opcode & 0x0F00) >> 8] != registers[(current_opcode & 0x00FF) >> 4]))
                    increment_pc();  
                increment_pc();
                break;
            
            case 0x6:
                // Set Vx = kk.
                registers[(current_opcode& 0x0F00) >> 8] = static_cast<uint8_t>(current_opcode & 0x00FF);
                increment_pc();
                break;
            
            case 0x7:
                // Set Vx = Vx + kk.
                registers[(current_opcode& 0x0F00) >> 8] += static_cast<uint8_t>(current_opcode & 0x00FF);
                increment_pc();
                break;

            case 0x8:
            {
                uint8_t x = (current_opcode & 0x0F00) >> 8;
                uint8_t y = (current_opcode & 0x00F0) >> 4;

                switch (current_opcode & 0x000F)
                {
                    case 0x0:
                        // 8XY0: Set Vx = Vy
                        registers[x] = registers[y];
                        break;

                    case 0x1:
                        // 8XY1: Set Vx = Vx OR Vy
                        registers[x] |= registers[y];
                        break;

                    case 0x2:
                        // 8XY2: Set Vx = Vx AND Vy
                        registers[x] &= registers[y];
                        break;

                    case 0x3:
                        // 8XY3: Set Vx = Vx XOR Vy
                        registers[x] ^= registers[y];
                        break;

                    case 0x4:
                        // 8XY4: Set Vx = Vx + Vy, set VF = carry
                        if (static_cast<uint8_t>(registers[x]) + static_cast<uint8_t>(registers[y]) > 255)
                            registers[0xF] = 1;
                        else
                            registers[0xF] = 0;
                        registers[x] += registers[y];
                        break;

                    case 0x5:
                        // 8XY5: Set Vx = Vx - Vy, set VF = NOT borrow
                        if (registers[x] > registers[y])
                            registers[0xF] = 0x1;
                        else
                            registers[0xF] = 0x0;
                        registers[x] -= registers[y];
                        break;

                    case 0x6:
                        // 8XY6: Set Vx = Vx >> 1, set VF = LSB of Vx
                        registers[0xF] = registers[x] & 0x1;
                        registers[x] >>= 1;
                        break;

                    case 0x7:
                        // 8XY7: Set Vx = Vy - Vx, set VF = NOT borrow
                        if (registers[y] > registers[x])
                            registers[0xF] = 0x1;
                        else
                            registers[0xF] = 0x0;
                        registers[x] = registers[y] - registers[x];
                        break;

                    case 0xE:
                        // 8XYE: Set Vx = Vx << 1, set VF = MSB of Vx
                        registers[0xF] = (registers[x] >> 7) & 0x1; // Set VF = MSB of Vx
                        registers[x] <<= 1; // multiplied by 2
                        break;

                    default:
                        throw "Unknown opcode within 0x8 cases in ALU!";
                }

                increment_pc();
                break;
            }

            case 0x9:



        }



    }
};



