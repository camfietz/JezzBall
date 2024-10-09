#pragma once
#include "SDL/SDL.h"
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <cassert>

bool check_collision(const std::vector<SDL_Rect> &A, const std::vector<SDL_Rect> &B) {
    // check collision function based on Lazy Foo' Productions (https://lazyfoo.net/SDL_tutorials/)
    int left_A, left_B;
    int right_A, right_B;
    int top_A, top_B;
    int bottom_A, bottom_B;
    
    // for each rect in A
    for(std::vector<SDL_Rect>::size_type box_A = 0; box_A < A.size(); ++box_A) {
        left_A = A[box_A].x;
        right_A = A[box_A].x + A[box_A].w;
        top_A = A[box_A].y;
        bottom_A = A[box_A].y + A[box_A].h;
        
        // for each rect in B
        for(std::vector<SDL_Rect>::size_type box_B = 0; box_B < B.size(); ++box_B) {
            left_B = B[box_B].x;
            right_B = B[box_B].x + B[box_B].w;
            top_B = B[box_B].y;
            bottom_B = B[box_B].y + B[box_B].h;
            
            // if collision detected
            if (((bottom_A <= top_B) || (top_A >= bottom_B) || (right_A <= left_B) || (left_A >= right_B)) == false ) { return true; }
        }
    }

    return false;
}

bool check_collision(const std::vector<SDL_Rect> &A, const std::vector<button> &B) {
    // modified check collision function based on Lazy Foo' Productions (https://lazyfoo.net/SDL_tutorials/)
    int left_A, left_B;
    int right_A, right_B;
    int top_A, top_B;
    int bottom_A, bottom_B;
    
    // for each hitbox in A
    for(std::vector<SDL_Rect>::size_type box_A = 0; box_A < A.size(); ++box_A) {
        left_A = A[box_A].x;
        right_A = A[box_A].x + A[box_A].w;
        top_A = A[box_A].y;
        bottom_A = A[box_A].y + A[box_A].h;
        
        // for each hitbox in B
        for(std::vector<SDL_Rect>::size_type box_B = 0; box_B < B.size(); ++box_B) {
            left_B = B[box_B].hitbox.x;
            right_B = B[box_B].hitbox.x + B[box_B].hitbox.w;
            top_B = B[box_B].hitbox.y;
            bottom_B = B[box_B].hitbox.y + B[box_B].hitbox.h;
            
            // if collision detected
             if (((bottom_A <= top_B) || (top_A >= bottom_B) || (right_A <= left_B) || (left_A >= right_B)) == false ) { return true; }
        }
    }

    return false;
}

wall check_collision(const std::vector<SDL_Rect> &A, std::vector<wall> &B) {
    // modified check collision function based on Lazy Foo' Productions (https://lazyfoo.net/SDL_tutorials/)
    int left_A, left_B;
    int right_A, right_B;
    int top_A, top_B;
    int bottom_A, bottom_B;
    
    // for each hitbox in A
    for(std::vector<SDL_Rect>::size_type box_A = 0; box_A < A.size(); ++box_A) {
        left_A = A[box_A].x;
        right_A = A[box_A].x + A[box_A].w;
        top_A = A[box_A].y;
        bottom_A = A[box_A].y + A[box_A].h;
        
        // for each hitbox in B
        for(std::vector<SDL_Rect>::size_type box_B = 0; box_B < B.size(); ++box_B) {
            left_B = B[box_B].hitbox.x;
            right_B = B[box_B].hitbox.x + B[box_B].hitbox.w;
            top_B = B[box_B].hitbox.y;
            bottom_B = B[box_B].hitbox.y + B[box_B].hitbox.h;
            
            // if collision detected
             if (((bottom_A <= top_B) || (top_A >= bottom_B) || (right_A <= left_B) || (left_A >= right_B)) == false ) { return wall(B[box_B].hitbox, true); }
        }
    }

    return wall(false);
}

// TIMER CLASS
timer::timer() {
    start_ticks = 0;
    paused_ticks = 0;
    paused = false;
    started = false;
}
void timer::start() {
    started = true;
    paused = false;
    start_ticks = SDL_GetTicks();
}
void timer::stop() {
    started = false;
    paused = false;
}
void timer::pause() {
    if ((started == true) && (paused == false)) {
        paused = true;
        paused_ticks = SDL_GetTicks() - start_ticks;
    }
}
void timer::unpause() {
    if (paused == true) {
        paused = false;
        start_ticks = SDL_GetTicks() - paused_ticks;
        paused_ticks = 0;
    }
}
int timer::get_ticks() const {     
    // if timer is running
    if (started == true) {
        // if timer is paused
        if (paused == true) { return paused_ticks; }
        else { return SDL_GetTicks() - start_ticks; }
    }
    // if timer is not running
    return 0;
}
bool timer::is_started() const { return started; }
bool timer::is_paused() const { return paused; }

// BUTTON CLASS
button::button(int x, int y, int w, int h, std::pair<int, int> p) {
    hitbox.x = x;
    hitbox.y = y;
    hitbox.w = w;
    hitbox.h = h;
    hitbox_vector.emplace_back(hitbox);
    pos.first = p.first;
    pos.second = p.second;
    colour = false;
    active = false;
    built = false;
    filled = false;
    complete = false;
}
void button::handle(std::vector<std::vector<button>> &grid, std::vector<wall> &walls_list, std::vector<button> &walls_to_build_black, std::vector<button> &walls_to_build_white, orientation &wall_orientation) {
    // start recurssion
    if (walls_to_build_black.empty() && walls_to_build_white.empty()){
        check_adjacent_wall(grid, walls_list, walls_to_build_black, walls_to_build_white, pos, true, true, 0, wall_orientation);
    }
}
void button::check_adjacent_wall(std::vector<std::vector<button>> &grid, std::vector<wall> &walls_list, std::vector<button> &walls_to_build_black, std::vector<button> &walls_to_build_white, std::pair<int, int> pos, bool orig_flag, bool next_flag, int counter, orientation wall_orientation) {

    int col = pos.first;
    int row = pos.second;

    int next_col, prev_col, next_row, prev_row;

    int max_col = (SCREEN_WIDTH - 2*GRID_X_OFFSET) / GRID_DIM - 1;
    int max_row = (SCREEN_HEIGHT - 2*GRID_Y_OFFSET) / GRID_DIM - 1;

    // if walls have reached end of grid
    if ((col > max_col) || (col < 0) || (row > max_row) || (row < 0)) { return; }

    // if its not the original wall, increment counter
    if (!orig_flag) { ++counter; }

    // check collision with any walls
    if (check_collision(hitbox_vector, walls_list)) { return; }
  
    // else
    active = true;
    colour = next_flag;
    delay_timer.start();
    delay_counter = counter;

    if (wall_orientation == orientation::vertical){
        next_col = col;
        prev_col = col;
        next_row = row + 1;
        prev_row = row - 1;
    } else {
        next_col = col + 1;
        prev_col = col - 1;
        next_row = row;
        prev_row = row;    
    }
    
    if (orig_flag){
        // ensure bounds, check both
        if (next_col <= max_col && next_row <= max_row) {
            // check next
            grid[next_col][next_row].check_adjacent_wall(grid, walls_list, walls_to_build_black, walls_to_build_white, std::make_pair(next_col, next_row), false, true, counter, wall_orientation);
        }
        if (prev_col >= 0 && prev_row >= 0) {
            // check previous
            grid[prev_col][prev_row].check_adjacent_wall(grid, walls_list, walls_to_build_black, walls_to_build_white, std::make_pair(prev_col, prev_row), false, false, counter, wall_orientation);
        }
    } else if (next_flag) {
        // ensure bounds
        if (next_col <= max_col && next_row <= max_row) {
            // continue next
            grid[next_col][next_row].check_adjacent_wall(grid, walls_list, walls_to_build_black, walls_to_build_white, std::make_pair(next_col, next_row), false, true, counter, wall_orientation);
        }
    } else {
        // ensure bounds
        if (prev_col >= 0 && prev_row >= 0) {
            // continue previous
            grid[prev_col][prev_row].check_adjacent_wall(grid, walls_list, walls_to_build_black, walls_to_build_white, std::make_pair(prev_col, prev_row), false, false, counter, wall_orientation);
        }
    }
    
    // finally
    if (colour) { walls_to_build_black.emplace_back(*this); }
    else { walls_to_build_white.emplace_back(*this); }
}
int button::col() const { return pos.first; }
int button::row() const { return pos.second; }
void button::reset () {
    colour = false;
    active = false;
    built = false;
    filled = false;
    complete = false;
}

// WALL CLASS
wall::wall(SDL_Rect wall, bool collision, bool colour) {
    hitbox = wall;
    this->collision = collision;
    this->colour = colour;
}
wall::wall(SDL_Rect wall, bool collision) {
    hitbox = wall;
    this->collision = collision;
}
wall::wall(bool collision) {
    this->collision = collision;
}
wall::operator bool() const { return collision; }

// BALL CLASS
ball::ball(int x, int y, int speed) {
    x_pos = x;
    y_pos = y;
    x_speed = speed;
    y_speed = speed;

    hitbox.resize(11);
    hitbox[ 0 ].w = 6;  hitbox[ 0 ].h = 1;
    hitbox[ 1 ].w = 10; hitbox[ 1 ].h = 1;
    hitbox[ 2 ].w = 14; hitbox[ 2 ].h = 1;
    hitbox[ 3 ].w = 16; hitbox[ 3 ].h = 2;
    hitbox[ 4 ].w = 18; hitbox[ 4 ].h = 2;
    hitbox[ 5 ].w = 20; hitbox[ 5 ].h = 6;
    hitbox[ 6 ].w = 18; hitbox[ 6 ].h = 2;
    hitbox[ 7 ].w = 16; hitbox[ 7 ].h = 2;
    hitbox[ 8 ].w = 14; hitbox[ 8 ].h = 1;
    hitbox[ 9 ].w = 10; hitbox[ 9 ].h = 1;
    hitbox[ 10 ].w = 6; hitbox[ 10 ].h = 1;
    shift_boxes();
}
void ball::update(timer &ball_timer) {

    // set x-direction
    x_pos += x_speed * (ball_timer.get_ticks()/1000.f);
    if ((x_pos <= GRID_X_OFFSET) || (x_pos + rad >= SCREEN_WIDTH - GRID_X_OFFSET)) {
        x_speed *= -1;
        x_pos = std::clamp(x_pos, float(GRID_X_OFFSET), float(SCREEN_WIDTH - GRID_X_OFFSET - rad));
    }

    // set y-direction
    y_pos += y_speed * (ball_timer.get_ticks()/1000.f);
    if ((y_pos <= GRID_Y_OFFSET) || (y_pos + rad >= SCREEN_HEIGHT - GRID_Y_OFFSET)) {
        y_speed *= -1;
        y_pos = std::clamp(y_pos, float(GRID_Y_OFFSET), float(SCREEN_HEIGHT - GRID_Y_OFFSET - rad));
    }

    // shift hitbox
    shift_boxes();

}
void ball::shift_boxes() {
    int row_offset = 0;
    for (std::vector<SDL_Rect>::size_type set = 0; set < hitbox.size(); set++) {
        // center box
        hitbox[set].x = x_pos + (rad - hitbox[set].w) / 2;
        // set box at row offset
        hitbox[set].y = y_pos + row_offset;
        // move row offset down to height of box
        row_offset += hitbox[set].h;
    }
}
void ball::set_direction(float &pos, float &other_pos) {
    if (pos < other_pos) {
        pos -= 1;
        other_pos += 1;
    } else {
        pos += 1;
        other_pos -= 1;
    }
}
void ball::set_position(float x, float y) {
    x_pos = x;
    y_pos = y;
    shift_boxes();
}

void window_init() {
    // initialize all SDL subsystems
    int sdl_init = SDL_Init(SDL_INIT_EVERYTHING);
    assert(sdl_init == 0);

    // set up screen (using SDL_HWSURFACE since background generally remains static)
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
    assert(screen != NULL);

    // set up cursor
    cursor_vertical = init_system_cursor(cursor_vertical_image);
    cursor_horizontal = init_system_cursor(cursor_horizontal_image);
    assert(cursor_vertical != NULL);
    assert(cursor_horizontal != NULL);
    SDL_SetCursor(cursor_vertical);

    // set window caption
    SDL_WM_SetCaption("JezzBall", NULL);
}

void digits_init() {
    // for each digit 0123456789, clip image by digit offset
    for (int digit = 0; digit < 10; ++digit){
        digits_clip[digit].x = DIGITS_OFFSET*digit;
        digits_clip[digit].y = 0;
        digits_clip[digit].h = digits_surface->h;
        digits_clip[digit].w = DIGITS_OFFSET;
    }
}

SDL_Surface* load_image (const std::string filename) {
    
    SDL_Surface* img_tmp = NULL;
    SDL_Surface* img_optimized = NULL;

    img_tmp = SDL_LoadBMP(filename.c_str());

    // error checking
    if (img_tmp != NULL) {
        // create optimized image
        img_optimized = SDL_DisplayFormat(img_tmp);
        SDL_FreeSurface(img_tmp);
    }

    return img_optimized;
}

void load_files(const options &parameters) {
    // load images
    background_surface = load_image("assets/background.bmp");
    pause_surface = load_image("assets/pause.bmp");
    level_complete_surface = load_image("assets/level_complete_" + parameters.BALL_COLOUR + ".bmp");
    game_over_surface = load_image("assets/game_over_" + parameters.BALL_COLOUR + ".bmp");
    game_over_animation_surface = load_image("assets/game_over_animation.bmp");
    game_winner_surface = load_image("assets/game_winner.bmp");
    game_winner_animation_surface = load_image("assets/game_winner_animation.bmp");
    balls_surface = load_image("assets/ball_" + parameters.BALL_COLOUR + ".bmp");
    wall_black = load_image("assets/wall_black.bmp");
    wall_white = load_image("assets/wall_white.bmp");
    digits_surface = load_image("assets/digits_" + parameters.BALL_COLOUR + ".bmp");

    // error checking
    if (background_surface == NULL) { std::cerr << "ensure that the assets folder is in the same directory as the executable"; }
    assert(background_surface != NULL);
    assert(pause_surface != NULL);
    assert(level_complete_surface != NULL);
    assert(game_over_surface != NULL);
    assert(game_over_animation_surface != NULL);
    assert(game_winner_animation_surface != NULL);
    assert(balls_surface != NULL);
    assert(wall_black != NULL);
    assert(wall_white != NULL);
    assert(digits_surface != NULL);

    // ensure ball surface has same height and width
    assert(balls_surface->w == balls_surface->h);
}

void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL) {
    // set offsets
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    
    // set transparency and blit surface
    SDL_SetColorKey(source, SDL_SRCCOLORKEY, SDL_MapRGB(source->format, 255, 255, 255));
    SDL_BlitSurface(source, clip, destination, &offset);
}

void window_exit() {
    // free surfaces
    for (auto surface : surface_array) { SDL_FreeSurface(surface); }

    // free cursors
    for (auto cursor : cursor_array) { SDL_FreeCursor(cursor); }

    // quit SDL
    SDL_Quit();
}
