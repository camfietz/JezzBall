#pragma once
#include "SDL/SDL.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <iterator>
#include <utility>
#include <algorithm> 
#include <numeric>
#include <cstdlib>
#include <stdexcept>

// SDL GLOBAL VARIABLES
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

const int GRID_DIM = 25;
const int GRID_X_OFFSET = 50;
const int GRID_Y_OFFSET = 100;

const int DIGITS_OFFSET = 32;
const int LIVES_DIGIT_1_OFFSET = 205;
const int LIVES_DIGIT_2_OFFSET = 240;
const int LEVEL_DIGIT_1_OFFSET = 490;
const int LEVEL_DIGIT_2_OFFSET = 525;
const int PERCENTAGE_DIGIT_1_OFFSET = 625;
const int PERCENTAGE_DIGIT_2_OFFSET = 660;

const int FPS_CAP = 60;

SDL_Event event;

SDL_Surface* screen = NULL;
SDL_Surface* background_surface = NULL;
SDL_Surface* pause_surface = NULL;
SDL_Surface* level_complete_surface = NULL;
SDL_Surface* game_over_surface = NULL;
SDL_Surface* game_over_animation_surface = NULL;
SDL_Surface* game_winner_surface = NULL;
SDL_Surface* game_winner_animation_surface = NULL;
SDL_Surface* balls_surface = NULL;
SDL_Surface* wall_black = NULL;
SDL_Surface* wall_white = NULL;
SDL_Surface* digits_surface = NULL;
SDL_Rect digits_clip[10];
SDL_Cursor* cursor_horizontal = NULL;
SDL_Cursor* cursor_vertical = NULL;
std::unordered_set<SDL_Surface*> surface_array{background_surface, pause_surface, level_complete_surface, game_over_surface, game_over_animation_surface, game_winner_surface, game_winner_animation_surface, balls_surface, wall_black, wall_white, digits_surface};
std::unordered_set<SDL_Cursor*> cursor_array{cursor_horizontal, cursor_vertical};

// CLASS FORWARD DECLARATIONS
class timer; class button; class wall; class ball;

struct options {
    unsigned int LEVEL_SELECT = 1;
    unsigned int STARTING_LIVES = 5;
    float BALL_SPEED = 0.5;
    const unsigned int BALL_SPEED_MODIFIER = 150; // in pixels per second
    std::string BALL_COLOUR = "red";
    float BUILD_SPEED = 0.5;
    const unsigned int BUILD_SPEED_MODIFIER = 400; // in pixels per second
    const unsigned int PERCENTAGE_TARGET = 75;
    std::pair<int, int> RESOLUTION = {800, 600}; // {width, height} in pixels 
};

struct state {
    unsigned int current_level = 0;
    unsigned int current_lives = 0;
    float current_percentage = 0;
    bool quit = false;
};

enum class orientation : bool {
    vertical = true,
    horizontal = false,
};

class timer {
    // timer class based on Lazy Foo' Productions (https://lazyfoo.net/SDL_tutorials/)
    private:
        int start_ticks;
        int paused_ticks;

        bool paused;
        bool started;

    public:
        timer();

        void start();
        void stop();

        void pause();
        void unpause();

        int get_ticks() const;
        bool is_started() const;
        bool is_paused() const;
};

class button {
    public:
        // collision box
        SDL_Rect hitbox;

        // vector containing collision box
        std::vector<SDL_Rect> hitbox_vector;

        // coordinates of button
        std::pair<int, int> pos;

        // attributes
        bool colour;
        bool active;
        bool built;
        bool filled;
        bool complete;

        // building delay
        timer delay_timer;
        int delay_counter;
        
    public:
        button(int x, int y, int w, int h, std::pair<int, int> p);
        
        // handle wall building
        void handle(std::vector<std::vector<button>> &grid, std::vector<wall> &walls_list, std::vector<button> &walls_to_build_black, std::vector<button> &walls_to_build_white, orientation &wall_orientation);

        // check if adjacent wall can be built
        void check_adjacent_wall(std::vector<std::vector<button>> &grid, std::vector<wall> &walls_list, std::vector<button> &walls_to_build_black, std::vector<button> &walls_to_build_white, std::pair<int, int> pos, bool orig_flag, bool next_flag, int counter, orientation wall_orientation);

        // return index on grid
        int col() const;
        int row() const;

        // reset wall attributes
        void reset();
};

class wall {
    public:
        // collision box
        SDL_Rect hitbox;

        // collision detected
        bool collision;

        // wall colour
        bool colour;

    public:
        explicit wall(SDL_Rect wall, bool collision, bool colour);

        explicit wall(SDL_Rect wall, bool collision);

        explicit wall(bool collision);

        // treat as bool
        explicit operator bool() const;
};

class ball {  
    public:
        // dimensions
        const int rad = balls_surface->w;

        // position
        float x_pos, y_pos;

        // speed
        float x_speed, y_speed;

    public:   
        // collision box
        std::vector<SDL_Rect> hitbox;

        ball(int x, int y, int speed);

        // update position with respect to speed
        void update(timer &ball_timer);

        // update hitbox with respect to position
        void shift_boxes();
       
        // offset slightly to ensure balls don't get stuck in each other
        void set_direction(float &pos, float &other_pos);

        // set position and hitbox
        void set_position(float x, float y);
};

static const char *cursor_horizontal_image[] = {
  // cursor format based on SDL Library Documentation (www.libsdl.org/release/SDL-1.2.15/docs/html/sdlcreatecursor.html)
  // width height num_colors chars_per_pixel
  "    32    32        3            1",
  // colours (white, black, transparent)
  "X c #000000",
  ". c #ffffff",
  "  c None",
  // pixels
  "               .X               ",
  "              ..XX              ",
  "             ...XXX             ",
  "            ....XXXX            ",
  "           .....XXXXX           ",
  "          ......XXXXXX          ",
  "         .......XXXXXXX         ",
  "        ........XXXXXXXX        ",
  "       .........XXXXXXXXX       ",
  "      ..........XXXXXXXXXX      ",
  "     ...........XXXXXXXXXXX     ",
  "    ............XXXXXXXXXXXX    ",
  "   .............XXXXXXXXXXXXX   ",
  "  ..............XXXXXXXXXXXXXX  ",
  " ...............XXXXXXXXXXXXXXX ",
  "................XXXXXXXXXXXXXXXX",
  "................XXXXXXXXXXXXXXXX",
  " ...............XXXXXXXXXXXXXXX ",
  "  ..............XXXXXXXXXXXXXX  ",
  "   .............XXXXXXXXXXXXX   ",
  "    ............XXXXXXXXXXXX    ",
  "     ...........XXXXXXXXXXX     ",
  "      ..........XXXXXXXXXX      ",
  "       .........XXXXXXXXX       ",
  "        ........XXXXXXXX        ",
  "         .......XXXXXXX         ",
  "          ......XXXXXX          ",
  "           .....XXXXX           ",
  "            ....XXXX            ",
  "             ...XXX             ",
  "              ..XX              ",
  "               .X               ",
  "0,0"
};

static const char *cursor_vertical_image[] = {
  // cursor format based on SDL Library Documentation (www.libsdl.org/release/SDL-1.2.15/docs/html/sdlcreatecursor.html)
  // width height num_colors chars_per_pixel
  "    32    32        3            1",
  // colours (white, black, transparent)
  "X c #000000",
  ". c #ffffff",
  "  c None",
  // pixels
  "               ..               ",
  "              ....              ",
  "             ......             ",
  "            ........            ",
  "           ..........           ",
  "          ............          ",
  "         ..............         ",
  "        ................        ",
  "       ..................       ",
  "      ....................      ",
  "     ......................     ",
  "    ........................    ",
  "   ..........................   ",
  "  ............................  ",
  " .............................. ",
  "................................",
  "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
  " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ",
  "  XXXXXXXXXXXXXXXXXXXXXXXXXXXX  ",
  "   XXXXXXXXXXXXXXXXXXXXXXXXXX   ",
  "    XXXXXXXXXXXXXXXXXXXXXXXX    ",
  "     XXXXXXXXXXXXXXXXXXXXXX     ",
  "      XXXXXXXXXXXXXXXXXXXX      ",
  "       XXXXXXXXXXXXXXXXXX       ",
  "        XXXXXXXXXXXXXXXX        ",
  "         XXXXXXXXXXXXXX         ",
  "          XXXXXXXXXXXX          ",
  "           XXXXXXXXXX           ",
  "            XXXXXXXX            ",
  "             XXXXXX             ",
  "              XXXX              ",
  "               XX               ",
  "0,0"
};

static SDL_Cursor *init_system_cursor(const char *image[]) {
    // cursor init based on SDL Library Documentation (www.libsdl.org/release/SDL-1.2.15/docs/html/sdlcreatecursor.html)
    int i, row, col;
    Uint8 data[4*32];
    Uint8 mask[4*32];

    i = -1;
    for (row = 0; row < 32; ++row) {
        for (col = 0; col < 32; ++col) {
            if (col % 8) {
                data[i] <<= 1;
                mask[i] <<= 1;
            } else {
                ++i;
                data[i] = mask[i] = 0;
            }
            switch (image[4 + row][col]) {
            case 'X':
                data[i] |= 0x01;
                mask[i] |= 0x01;
                break;
            case '.':
                mask[i] |= 0x01;
                break;
            case ' ':
                break;
            }
        }
    }
  return SDL_CreateCursor(data, mask, 32, 32, 16, 16);
}

void print_command_line_arguments() { 
    std::cout << "COMMAND LINE ARGUMENTS:" << std::endl;
    std::cout << "-ls $levelselect (=1)" << std::endl;
    std::cout << "     Set the starting level in $levelselect | range [1, 50]." << std::endl;
    std::cout << "-sl $startinglives (=5)" << std::endl;
    std::cout << "     Set the starting lives in $startinglives | range [1, 99]." << std::endl;
    std::cout << "-bs $ballspeed (=0.5)" << std::endl;
    std::cout << "     Set the speed of the balls in $ballspeed | range (0.0, 1.0]." << std::endl;
    std::cout << "-bc $ballcolour (=red)" << std::endl;
    std::cout << "     Set the colour of the balls in $ballcolour | range [red, blue, green]." << std::endl;
    std::cout << "-res $resolution (=800x600)" << std::endl;
    std::cout << "     Set the resolution of the game window in $resolution | range [4:3 aspect ratio]" << std::endl;
}

void parse_command_line_arguments(int argc, char* argv[], options &parameters) {
    
    const std::unordered_set<std::string> ball_colour_LUT({"red", "blue", "green"});

    // parse arguments
    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];

        try {

            // LEVEL SELECT
            if (arg.substr(0,3) == "-ls") {
                try {
                    int level = std::stoi(arg.substr(arg.find_first_of("0123456789")));
                    if (level >= 1 && level <= 50) {
                        parameters.LEVEL_SELECT = level;
                    } else {
                        throw std::invalid_argument("error: starting level must be in range [1, 50]");
                    }
                } catch (const std::exception& e) {
                    throw std::invalid_argument("error: starting level must be in range [1, 50]");
                }

            // STARTING LIVES
            } else if (arg.substr(0,3) == "-sl") {
                try {
                    int lives = std::stoi(arg.substr(arg.find_first_of("0123456789")));
                    if (lives >= 1 && lives <= 99) {
                        parameters.STARTING_LIVES = lives;
                    } else {
                        throw std::invalid_argument("error: starting lives must be in range [1, 99]");
                    }
                } catch (const std::exception& e) {
                    throw std::invalid_argument("error: starting lives must be in range [1, 99]");
                }

            // BALL SPEED
            } else if (arg.substr(0,3) == "-bs") {
                try {
                    float speed = std::stof(arg.substr(arg.find_first_of("0123456789")));
                    if (speed > 0.0 && speed <= 1.0) {
                        parameters.BALL_SPEED = speed;
                    } else {
                        throw std::invalid_argument("error: ball speed must be in range (0.0, 1.0]");
                    }
                } catch (const std::exception& e) {
                    throw std::invalid_argument("error: ball speed must be in range (0.0, 1.0]");
                }

            // BALL COLOUR
            } else if (arg.substr(0,3) == "-bc") {
                try {
                    std::string colour = arg.substr(3);
                    if (ball_colour_LUT.find(colour) != ball_colour_LUT.end()) {
                        parameters.BALL_COLOUR = colour;
                    } else {
                        throw std::invalid_argument("error: ball colour must be red, blue, or green");
                    }
                } catch (const std::exception& e) {
                    throw std::invalid_argument("error: ball colour must be red, blue, or green");
                }

            // RESOLUTION
            } else if (arg.substr(0,4) == "-res") {
                try {
                    std::istringstream ss(arg.substr(4));

                    std::string w;
                    getline(ss, w, 'x');

                    std::string h;
                    getline(ss, h, 'x');

                    std::string dummy;
                    int width = std::stoi(w);
                    int height = std::stoi(h);
                    int gcd = std::gcd(height, width);
                    if ((width/gcd == 4 && height/gcd == 3) && (!getline(ss, dummy, ' '))) {
                        parameters.RESOLUTION.first = std::stoi(w);
                        parameters.RESOLUTION.second = std::stoi(h);
                    } else {
                        throw std::invalid_argument("error: resolution must be 4:3 aspect ratio");
                    }
                } catch (const std::exception& e) {
                    throw std::invalid_argument("error: resolution must be 4:3 aspect ratio");
                }

            // HELP
            } else if (arg.substr(0,6) == "--help") {
                print_command_line_arguments();
                std::exit(0);
            }


        } catch (const std::invalid_argument& e) {
            std::cerr << e.what() << std::endl;
            std::exit(1);
        }
    }


    // std::cout << "starting level: " << parameters.LEVEL_SELECT << std::endl;
    // std::cout << "starting lives: " << parameters.STARTING_LIVES << std::endl;
    // std::cout << "ball speed: " << parameters.BALL_SPEED << std::endl;
    // std::cout << "ball colour: " << parameters.BALL_COLOUR << std::endl;
    // std::cout << "resolution: " << parameters.RESOLUTION.first << "x" << parameters.RESOLUTION.second << std::endl;
}
