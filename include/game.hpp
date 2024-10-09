#pragma once
#include "SDL/SDL.h"
#include <vector>
#include <cstdlib>
#include <utility>
#include <sstream>
#include <cmath>
#include <algorithm>

// INITIALIZATION
void arguments_init(int argc, char* argv[], options &parameters){
    // parse the command line arguments
    parse_command_line_arguments(argc, argv, parameters);
}

void button_init(std::vector<std::vector<button>> &grid) {
    // for each cell in the grid, construct button and add to 2D vector 
    for (unsigned int x = GRID_X_OFFSET; x < (SCREEN_WIDTH - GRID_X_OFFSET); x += GRID_DIM) {
        std::vector<button> row;
        for (unsigned int y = GRID_Y_OFFSET; y < (SCREEN_HEIGHT - GRID_Y_OFFSET); y += GRID_DIM) {
            button button_tmp(x, y, GRID_DIM, GRID_DIM, std::make_pair((x - GRID_X_OFFSET) / GRID_DIM, (y - GRID_Y_OFFSET) / GRID_DIM));
            row.emplace_back(button_tmp);
        }
        grid.emplace_back(row);
    }
}

void ball_init(const options &parameters, const state &game_state, std::vector<ball> &balls_list) {
    // construct n balls, where n = value of current level
    for (unsigned int n = 0; n < game_state.current_level; ++n) {
        // generate ball in random location in playfield that while not overlapping with another ball
        ball ball_tmp(0, 0, parameters.BALL_SPEED*parameters.BALL_SPEED_MODIFIER);
        bool valid_position = false;
        while (!valid_position) {
            int x_tmp = std::rand() % ((SCREEN_WIDTH-GRID_X_OFFSET-ball_tmp.rad)-(GRID_X_OFFSET+ball_tmp.rad) + 1) + GRID_X_OFFSET+ball_tmp.rad;
            int y_tmp = std::rand() % ((SCREEN_HEIGHT-GRID_Y_OFFSET-ball_tmp.rad)-(GRID_Y_OFFSET+ball_tmp.rad) + 1) + GRID_Y_OFFSET+ball_tmp.rad;
            ball_tmp.set_position(x_tmp, y_tmp);

            valid_position = true;
            for (ball &other_ball : balls_list) {
                if (check_collision(ball_tmp.hitbox, other_ball.hitbox)) { 
                    valid_position = false;
                    break;
                }
            }
        }
        balls_list.emplace_back(ball_tmp);
    }
}

// EVENT HANDLING
void pause_handle(timer &fps, timer &ball_timer, std::vector<button> &walls_to_build_black, std::vector<button> &walls_to_build_white) {
    if (((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE)) || ((event.type == SDL_ACTIVEEVENT) && (event.active.gain == 0))){
        // pause if esc key is pressed or app loses focus
        if (fps.is_paused() && (SDL_GetAppState() & SDL_APPMOUSEFOCUS)) {
            // unpause timers
            fps.unpause();
            ball_timer.unpause();
            // unpause wall delay timers
            for (button &current_wall : walls_to_build_black ) { current_wall.delay_timer.unpause(); }
            for (button &current_wall : walls_to_build_white ) { current_wall.delay_timer.unpause(); }
        }
        // unpause if esc key is pressed and app is in focus
        else {
            // display pause overlay
            apply_surface((SCREEN_WIDTH-pause_surface->w)/2, (SCREEN_HEIGHT-pause_surface->h)/2, pause_surface, screen);
            // pause timers
            fps.pause();
            ball_timer.pause();
            // pause wall delay timers
            for (button &current_wall : walls_to_build_black ) { current_wall.delay_timer.pause(); }
            for (button &current_wall : walls_to_build_white ) { current_wall.delay_timer.pause(); }
        }
    }
}

void orientation_handle(orientation &wall_orientation) {
    // rotate wall direction with right click 
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_RIGHT) {
            wall_orientation = (wall_orientation == orientation::vertical) ? orientation::horizontal : orientation::vertical;
        }
    }
}

// RENDERING
void render_digits(unsigned int number, int digit_1_offset, int digit_2_offset, bool invert) {
    // split number 0-99 into two digits
    if (number > 99) { return; }
    const int digit_1 = number / 10;
    const int digit_2 = number % 10;

    if (digit_1 == 0) {
        if (!invert) {
            // digit 1 = digit 2
            apply_surface(digit_1_offset, 0, digits_surface, screen, &digits_clip[digit_2]);
        } else {
            apply_surface(digit_2_offset, 0, digits_surface, screen, &digits_clip[digit_2]);
        }
    } else {
        apply_surface(digit_1_offset, 0, digits_surface, screen, &digits_clip[digit_1]);
        apply_surface(digit_2_offset, 0, digits_surface, screen, &digits_clip[digit_2]);
    }
}

void hud_handle(const state &game_state) {
    // render level, lives, captured percentage
    render_digits(game_state.current_level, LEVEL_DIGIT_1_OFFSET, LEVEL_DIGIT_2_OFFSET, false);
    render_digits(game_state.current_lives, LIVES_DIGIT_1_OFFSET, LIVES_DIGIT_2_OFFSET, false);
    render_digits(game_state.current_percentage, PERCENTAGE_DIGIT_1_OFFSET, PERCENTAGE_DIGIT_2_OFFSET, true);
}

void fps_handle(timer &fps, unsigned int start_time) {
    // cap fps
    if (fps.get_ticks() < 1000 / FPS_CAP) { SDL_Delay((1000/FPS_CAP) - fps.get_ticks()); }
    unsigned int frame_time = SDL_GetTicks() - start_time;
    float fps_calculated = (frame_time > 0) ? 1000.0f / frame_time : 0.0f;
    // display fps
    std::stringstream caption;
    caption << "JezzBall - FPS: " << std::round(fps_calculated);
    SDL_WM_SetCaption(caption.str().c_str(), NULL);
}

// GAME LOGIC
void handle_ball_collisions(ball &current_ball, std::vector<ball> &balls_list, timer &ball_timer) {
    // detect ball collisions
    for (ball &other_ball : balls_list) {
        if (&current_ball != &other_ball) {
            if (check_collision(current_ball.hitbox, other_ball.hitbox)) {
                // if x-directions are different
                if ((current_ball.x_speed > 0 && other_ball.x_speed < 0) || (current_ball.x_speed < 0 && other_ball.x_speed > 0)) {
                    current_ball.x_speed *= -1;
                    other_ball.x_speed *= -1;
                    current_ball.set_direction(current_ball.x_pos, other_ball.x_pos);
                }                
                // if y-directions are different
                else if ((current_ball.y_speed > 0 && other_ball.y_speed < 0) || (current_ball.y_speed < 0 && other_ball.y_speed > 0)) {
                    current_ball.y_speed *= -1;
                    other_ball.y_speed *= -1;
                    current_ball.set_direction(current_ball.y_pos, other_ball.y_pos);
                }
                // else
                else {
                    current_ball.x_speed *= -1;
                    current_ball.y_speed *= -1;
                    other_ball.x_speed *= -1;
                    other_ball.y_speed *= -1;
                    current_ball.set_direction(current_ball.x_pos, other_ball.x_pos);
                    current_ball.set_direction(current_ball.y_pos, other_ball.y_pos);
                }
                other_ball.update(ball_timer);
            }
        }
    }
}

void handle_wall_collisions(state &game_state, ball &current_ball, std::vector<wall> &walls_list, std::vector<button> &walls_to_build_black, std::vector<button> &walls_to_build_white, std::vector<button> &walls_black_buffer, std::vector<button> &walls_white_buffer) {            

    //  check for collision with a wall in buffers
    if (check_collision(current_ball.hitbox, walls_black_buffer)) {
        // subtract life, ensure only one live removed per wall and lives do not go below 0
        if (!walls_to_build_black.empty() || !walls_black_buffer.empty()) {
            game_state.current_lives = game_state.current_lives > 0 ? game_state.current_lives - 1 : 0;
            walls_to_build_black.clear();
            walls_black_buffer.clear();
        }
    } else if (check_collision(current_ball.hitbox, walls_white_buffer)) {
        // subtract life, ensure only one live removed per wall and lives do not go below 0
        if (!walls_to_build_white.empty() || !walls_white_buffer.empty()) {
            game_state.current_lives = game_state.current_lives > 0 ? game_state.current_lives - 1 : 0;
            walls_to_build_white.clear();
            walls_white_buffer.clear();
        }
    }

    // check for collision with an active wall
    if (wall w = check_collision(current_ball.hitbox, walls_list)) {
        
        float dx = (current_ball.x_pos + current_ball.rad)/2 - (w.hitbox.x + w.hitbox.w)/2;
        float dy = (current_ball.y_pos + current_ball.rad)/2 - (w.hitbox.y + w.hitbox.h)/2;
        float offset = current_ball.rad / 10.0;

        // if dx and dy are very close, assume equal collision 
        if (std::hypot(dx, dy) < 1.0) {
            current_ball.x_speed *= -1;
            current_ball.y_speed *= -1;
            current_ball.shift_boxes();
            return;
        }

        // x-collision
        if (std::abs(dx) > std::abs(dy)) {
            // wall <- ball
            if (dx >= 0) {
                // std::cout << "wall <- ball " << current_ball.x_pos << " " << w.hitbox.x << std::endl;
                current_ball.x_speed *= -1;
                current_ball.x_pos = std::clamp(current_ball.x_pos, float(w.hitbox.x + w.hitbox.w), float(SCREEN_WIDTH - GRID_X_OFFSET - current_ball.rad));
                current_ball.x_pos += offset;
            }
            // ball -> wall
            else  {
                // std::cout << "ball -> wall " << current_ball.x_pos << " " << w.hitbox.x << std::endl;
                current_ball.x_speed *= -1;
                current_ball.x_pos = std::clamp(current_ball.x_pos, float(GRID_X_OFFSET), float(w.hitbox.x));
                current_ball.x_pos -= offset;
            }
        // y-collision
        } else {
            // ball ^ wall
            if (dy >= 0) {
                // std::cout << "ball ^ wall " << current_ball.y_pos << " " << w.hitbox.y << std::endl;
                current_ball.y_speed *= -1;
                current_ball.y_pos = std::clamp(current_ball.y_pos, float(w.hitbox.y + w.hitbox.h), float(SCREEN_HEIGHT - GRID_Y_OFFSET - current_ball.rad));
                current_ball.y_pos += offset;
            }
            // ball v wall
            else {
                // std::cout << "ball v wall " << current_ball.y_pos << " " << w.hitbox.y << std::endl; 
                current_ball.y_speed *= -1;
                current_ball.y_pos = std::clamp(current_ball.y_pos, float(GRID_Y_OFFSET), float(w.hitbox.y));
                current_ball.y_pos -= offset;
            }
        }
        current_ball.shift_boxes();
    }
}

void ball_handle(state &game_state, std::vector<ball> &balls_list, timer &ball_timer, std::vector<wall> &walls_list, std::vector<button> &walls_to_build_black, std::vector<button> &walls_to_build_white, std::vector<button> &walls_black_buffer, std::vector<button> &walls_white_buffer) {
    // for each ball on screen
    for (ball &current_ball : balls_list) {
        // apply texture
        apply_surface(current_ball.x_pos, current_ball.y_pos, balls_surface, screen);
 
        // handle wall collisions
        handle_wall_collisions(game_state, current_ball, walls_list, walls_to_build_black, walls_to_build_white, walls_black_buffer, walls_white_buffer);
        current_ball.update(ball_timer);

        // handle ball collisions
        handle_ball_collisions(current_ball, balls_list, ball_timer);
        current_ball.update(ball_timer);
    }
    // restart ball timer
    ball_timer.start();
}

void button_handle(std::vector<std::vector<button>> &grid, std::vector<wall> &walls_list, std::vector<button> &walls_to_build_black, std::vector<button> &walls_to_build_white, orientation &wall_orientation) {
    // mouse button pressed
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        // left click
        if (event.button.button == SDL_BUTTON_LEFT) {
            // get mouse position
            int mouse_x = event.button.x;
            int mouse_y = event.button.y;
            
            // if mouse is within gameplay area
            if ((mouse_x >= GRID_X_OFFSET && mouse_x <= SCREEN_WIDTH - GRID_X_OFFSET) && (mouse_y >= GRID_Y_OFFSET && mouse_y <= SCREEN_WIDTH - GRID_Y_OFFSET)) {
                unsigned int grid_x = (mouse_x - GRID_X_OFFSET) / GRID_DIM;
                unsigned int grid_y = (mouse_y - GRID_Y_OFFSET) / GRID_DIM;
                
                // if button is within gameplay area
                if ((grid_x >= 0 && grid_x < grid.size()) && (grid_y >= 0 && grid_y < grid[0].size())) {
                    grid[grid_x][grid_y].handle(grid, walls_list, walls_to_build_black, walls_to_build_white, wall_orientation);
                }
            }
        }
    }
}

void wall_handle(std::vector<wall> &walls_list) {
    // for each wall placed
    for (wall &current_wall : walls_list) {
        // render wall
        if (current_wall.colour) {
            SDL_BlitSurface(wall_black, NULL, screen, &current_wall.hitbox);
        } else {
            SDL_BlitSurface(wall_white, NULL, screen, &current_wall.hitbox);
        }
    }
}

void build_walls(const options &parameters, std::vector<std::vector<button>> &grid, std::vector<wall> &walls_list, std::vector<button> &walls_to_build, std::vector<button> &walls_buffer, bool &walls_building) {
    // for each wall in walls to build
    std::vector<button>::iterator current_wall = walls_to_build.begin();
    while (current_wall != walls_to_build.end()) {
        // if the wall is ready to be built
        if ((current_wall->delay_timer.get_ticks() > (parameters.BUILD_SPEED*parameters.BUILD_SPEED_MODIFIER)*current_wall->delay_counter)) {
            // set cell in grid to built
            grid[current_wall->col()][current_wall->row()].built = true;
            // add wall to walls buffer
            walls_buffer.emplace_back(*current_wall);
            // delete wall from walls to build
            walls_to_build.erase(current_wall);
            // build wall
            wall wall_tmp(current_wall->hitbox, false, current_wall->colour);
            walls_list.emplace_back(wall_tmp);
            walls_building = true;
            
        } else { ++current_wall; }
    }
    // if there are no more walls to build, clear bool and buffer
    if (walls_to_build.empty()) { walls_building = false; walls_buffer.clear(); }
}

bool check_fill(std::vector<std::vector<button>> &grid, int x, int y, int max_x, int max_y, std::vector<std::vector<bool>> &visited, std::vector<ball> &balls_list) { 
    // does not fill if cell is out of bounds
    if (x < 0 || x > max_x || y < 0 || y > max_y) { return false; }

    // does not fill if contains ball
    for (ball &current_ball : balls_list) {
        float grid_x = ((current_ball.x_pos + current_ball.rad/2.0) - GRID_X_OFFSET) / GRID_DIM;
        float grid_y = ((current_ball.y_pos + current_ball.rad/2.0) - GRID_Y_OFFSET) / GRID_DIM;
        float grid_offset = GRID_DIM / current_ball.rad;
        // if ball is within area
        if ((x <= grid_x+grid_offset && x >= grid_x-grid_offset) && (y <= grid_y+grid_offset && y >= grid_y-grid_offset)) { return false; }
    }

    // can fill if cell is built or already visited
    if (grid[x][y].built || visited[x][y]) { return true;}

    // set cell to visited
    visited[x][y] = true;

    // check all directions recursively
    if (check_fill(grid, x-1, y, max_x, max_y, visited, balls_list) == false) { return false; };
    if (check_fill(grid, x+1, y, max_x, max_y, visited, balls_list) == false) { return false; };
    if (check_fill(grid, x, y-1, max_x, max_y, visited, balls_list) == false) { return false; };
    if (check_fill(grid, x, y+1, max_x, max_y, visited, balls_list) == false) { return false; };

    // else
    return true;
}

void handle_endgame(bool win, SDL_Surface* condition_surface, SDL_Surface* condition_animation_surface, state &game_state, timer &fps, timer &quit_timer, std::vector<wall> &walls_list, std::vector<button> &walls_to_build_black, std::vector<button> &walls_to_build_white, std::vector<button> &walls_black_buffer, std::vector<button> &walls_white_buffer, std::vector<ball> &balls_list, timer &ball_timer) {
    // get ready to quit the game
    if (!quit_timer.is_started()) {
        quit_timer.start();
        fps.stop();
        ball_timer.stop();
    }
    
    while (quit_timer.is_started() && !game_state.quit) {
        // render image to screen
        SDL_FillRect(screen, NULL, 0x000000);
        SDL_BlitSurface(background_surface, NULL, screen, NULL);
        wall_handle(walls_list);
        ball_handle(game_state, balls_list, ball_timer, walls_list, walls_to_build_black, walls_to_build_white, walls_black_buffer, walls_white_buffer);
        
        // animate screen
        if (quit_timer.get_ticks() % 1500 < 750) {
            apply_surface((SCREEN_WIDTH-condition_surface->w)/2, (SCREEN_HEIGHT-condition_surface->h)/2, condition_surface, screen);
            hud_handle(game_state);
        }
        else {
            apply_surface((SCREEN_WIDTH-condition_animation_surface->w)/2, (SCREEN_HEIGHT-condition_animation_surface->h)/2, condition_animation_surface, screen);
            if (win) {
                // flash level
                render_digits(game_state.current_lives, LIVES_DIGIT_1_OFFSET, LIVES_DIGIT_2_OFFSET, false);
                render_digits(game_state.current_percentage, PERCENTAGE_DIGIT_1_OFFSET, PERCENTAGE_DIGIT_2_OFFSET, true);
            } else {
                // flash lives
                render_digits(game_state.current_level, LEVEL_DIGIT_1_OFFSET, LEVEL_DIGIT_2_OFFSET, false);
                render_digits(game_state.current_percentage, PERCENTAGE_DIGIT_1_OFFSET, PERCENTAGE_DIGIT_2_OFFSET, true);
            }
        }
        SDL_Flip(screen);
        
        // wait for quit or escape key to exit
        while(SDL_PollEvent(&event)) {
            if ((event.type == SDL_QUIT) || ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE))) {
                game_state.quit = true;
                break;
            }
        }
    }
}

void update_game_state(const options &parameters, state &game_state, timer &fps, timer &level_timer, timer &quit_timer, std::vector<std::vector<button>> &grid, std::vector<wall> &walls_list, std::vector<button> &walls_to_build_black, std::vector<button> &walls_to_build_white, std::vector<button> &walls_black_buffer, std::vector<button> &walls_white_buffer, bool &walls_black_building, bool &walls_white_building, std::vector<ball> &balls_list, timer &ball_timer) {

    // for each cell in grid, check if walls need to be filled
    const int max_x = grid.size() - 1;
    const int max_y = grid[0].size() - 1;
    std::vector<std::vector<bool>> visited(max_x + 1, std::vector<bool>(max_y + 1, false));
    for(int x = 0; x <= max_x; ++x){
        for(int y = 0; y <= max_y; ++y){
            // if wall is not built and not filled
            if (!grid[x][y].built && !grid[x][y].filled) {
                // reset visited vector and check if wall can be filled
                std::fill(visited.begin(), visited.end(), std::vector<bool>(max_y + 1, false));
                grid[x][y].filled = check_fill(grid, x, y, max_x, max_y, visited, balls_list);
            } else {
                // if wall is filled but not complete and nothing is currently being built
                if(grid[x][y].filled && !grid[x][y].complete && !walls_black_building && !walls_white_building) {
                    // add wall to the list of walls and mark as complete
                    wall wall_tmp(grid[x][y].hitbox, false, true);
                    walls_list.emplace_back(wall_tmp);
                    grid[x][y].active = true;
                    grid[x][y].built = true;
                    grid[x][y].complete = true;
                }
            }
        }
    }

    // set capture percentage
    game_state.current_percentage = float(walls_list.size()) / float(grid.size()*grid[0].size()) * 100.0;
    // check if percentage target has be reached
    if (game_state.current_percentage > parameters.PERCENTAGE_TARGET) {
        if (!level_timer.is_started()) {
            // advance to next level
            level_timer.start();

            // check if player has won the game
            if (game_state.current_level + 1 > 50) {
                handle_endgame(true, game_winner_surface, game_winner_animation_surface, game_state, fps, quit_timer, walls_list, walls_to_build_black, walls_to_build_white, walls_black_buffer, walls_white_buffer, balls_list, ball_timer);
                return;
            } else { ++game_state.current_level; }

            fps.pause();
            ball_timer.pause();
        }

        // render level complete image
        SDL_FillRect(screen, NULL, 0x000000);
        SDL_BlitSurface(background_surface, NULL, screen, NULL);
        hud_handle(game_state);
        wall_handle(walls_list);
        ball_handle(game_state, balls_list, ball_timer, walls_list, walls_to_build_black, walls_to_build_white, walls_black_buffer, walls_white_buffer);
        apply_surface((SCREEN_WIDTH-level_complete_surface->w)/2, (SCREEN_HEIGHT-level_complete_surface->h)/2, level_complete_surface, screen);
        SDL_Flip(screen);
        
        // wait until game is resumed
        if (!fps.is_paused()) {
            level_timer.stop();
            game_state.current_percentage = 0;
            game_state.current_lives = parameters.STARTING_LIVES;
            // reset walls
            walls_list.clear();
            // reset buttons
            for (std::vector<button> &row : grid) {
                for (button &cell : row) {
                    cell.reset();
                }
            }
            // reset balls
            balls_list.clear();
            ball_init(parameters, game_state, balls_list);
        }
    }

    // check if player is out of lives
    if (game_state.current_lives <= 0) {
        handle_endgame(false, game_over_surface, game_over_animation_surface, game_state, fps, quit_timer, walls_list, walls_to_build_black, walls_to_build_white, walls_black_buffer, walls_white_buffer, balls_list, ball_timer);
        return;
    }
}
