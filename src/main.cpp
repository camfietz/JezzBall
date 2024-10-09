#include "SDL/SDL.h"
#include "input.hpp"
#include "window.hpp"
#include "game.hpp"
#include <iostream>
#include <vector>

int main (int argc, char* argv[]) {

    // init arguments
    options parameters;
    arguments_init(argc, argv, parameters);

    // load game state
    state game_state {
        .current_level = parameters.LEVEL_SELECT,
        .current_lives = parameters.STARTING_LIVES,
        .current_percentage = 0,
        .quit = false,
    };

    // init timers
    timer fps;
    timer ball_timer;
    timer level_timer;
    timer quit_timer;
    unsigned int start_time;

    // initialize SDL window
    window_init();

    // load files
    load_files(parameters);
    digits_init();

    // load buttons
    std::vector<std::vector<button>> grid;
    button_init(grid);

    // load walls
    std::vector<wall> walls_list;
    orientation wall_orientation = orientation::vertical;
    std::vector<button> walls_to_build_black;
    std::vector<button> walls_to_build_white;
    std::vector<button> walls_black_buffer;
    std::vector<button> walls_white_buffer;
    bool walls_black_building = false;
    bool walls_white_building = false;

    // load balls
    std::vector<ball> balls_list;
    ball_init(parameters, game_state, balls_list);

    // GAME LOOP
    try {
        while (!game_state.quit) {
            
            // GAME RUNNING
            if (!fps.is_paused()) {
                
                // INITIALIZATION
                fps.start();
                start_time = SDL_GetTicks();

                // clear frame
                if (SDL_FillRect(screen, NULL, 0x000000) == -1) { throw std::runtime_error("SDL failed"); };
                if (SDL_BlitSurface(background_surface, NULL, screen, NULL) == -1) { throw std::runtime_error("SDL failed"); }

                // set cursor
                SDL_SetCursor((wall_orientation == orientation::vertical) ? cursor_vertical : cursor_horizontal);

                // set HUD
                hud_handle(game_state);

                // EVENTS LOOP
                while (SDL_PollEvent(&event)) {

                    // check for pause
                    pause_handle(fps, ball_timer, walls_to_build_black, walls_to_build_white);
                    if (!fps.is_paused()) { 

                        // handle button
                        button_handle(grid, walls_list, walls_to_build_black, walls_to_build_white, wall_orientation);

                        // handle orientation
                        orientation_handle(wall_orientation);
                        
                    }

                    // check for quit
                    if (event.type == SDL_QUIT) { game_state.quit = true; }

                }

                // LOGIC
                if (!fps.is_paused()) {
                    
                    // build black and white walls
                    build_walls(parameters, grid, walls_list, walls_to_build_black, walls_black_buffer, walls_black_building);
                    build_walls(parameters, grid, walls_list, walls_to_build_white, walls_white_buffer, walls_white_building);

                    // handle walls
                    wall_handle(walls_list);
                    
                    // handle balls
                    ball_handle(game_state, balls_list, ball_timer, walls_list, walls_to_build_black, walls_to_build_white, walls_black_buffer, walls_white_buffer);

                    // update game state
                    update_game_state(parameters, game_state, fps, level_timer, quit_timer, grid, walls_list, walls_to_build_black, walls_to_build_white, walls_black_buffer, walls_white_buffer, walls_black_building, walls_white_building, balls_list, ball_timer);
                }

            // GAME PAUSED
            } else {
                
                // EVENTS LOOP
                while (SDL_PollEvent(&event)) {
                    
                    // check for unpause
                    pause_handle(fps, ball_timer, walls_to_build_black, walls_to_build_white);

                    // check for quit
                    if (event.type == SDL_QUIT) { game_state.quit = true; }
                    
                }
            }

            // RENDERING
            if (!level_timer.is_started()) {
                if (SDL_Flip(screen) == -1 ) { throw std::runtime_error("SDL failed"); }
            }

            // display and cap fps
            fps_handle(fps, start_time);

        }

    // catch errors and attempt to exit
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << SDL_GetError() << std::endl;
        window_exit();
        std::exit(1);
    }

    // clean up and quit
    window_exit();

    return 0;
}
