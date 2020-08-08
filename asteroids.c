/*

  An implementation of ASTEROIDS in SDL2
  Last modified 21st of April 2020
  Copyright (c) 2020 Arthur Ferreira

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.

*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#define PI 3.1415926535
#define WIDTH 800
#define HEIGHT 600
#define FPS 60

#define AST_MAX 80
#define AST_NB 4
#define AST_SIZE_MAX 32
#define AST_SIZE_MIN 8
#define AST_RES 16

#define BUL_MAX 50
#define BPS 5
#define BUL_SPEED 5


struct ship{
  float x;
  float y;
  float angle;
  float speedX;
  float speedY;
} ship;

typedef struct asteroid_t {
  float x;
  float y;
  float r;
  float p[AST_RES];
  float angle;
  float speed;
} asteroid;

asteroid asteroids[AST_MAX];
int nbAsteroids = AST_NB;

typedef struct bullet_t {
  float x;
  float y;
  float speedX;
  float speedY;
} bullet;

bullet bullets[BUL_MAX] = {{0}};
int bulletNext = 0;

unsigned int level = 1;
unsigned int score = 0;
unsigned int highScore = 0;


inline float rand_fl() {
  return (float)rand() / (float)RAND_MAX;
}


void initLevel() {
  ship.x = WIDTH / 2;
  ship.y = HEIGHT / 2;
  ship.angle  = -PI / 2;
  ship.speedX = 0;
  ship.speedY = 0;

  nbAsteroids = AST_NB;
  for (int i = 0; i < nbAsteroids; i++) {
    asteroids[i].x = rand() % WIDTH;
    asteroids[i].y = rand() % HEIGHT;
    if (pow(ship.x - asteroids[i].x, 2) + pow(ship.y - asteroids[i].y, 2) < 5000) {
      i--;
      continue;
    }
    asteroids[i].r = AST_SIZE_MAX;
    for (int j = 0; j < AST_RES; j++)
      asteroids[i].p[j] = asteroids[i].r + (rand_fl() * asteroids[i].r / 5);
    asteroids[i].angle = rand_fl() * 2 * PI;
    asteroids[i].speed = (rand_fl() + 0.2) * level;
  }

  for (int i = 0; i <  BUL_MAX; i++) {
    bullets[i].x = -1;
    bullets[i].speedX = -1;
  }
}

void updateScene() {
  for (int i = 0; i < nbAsteroids; i++) {
    asteroids[i].x += asteroids[i].speed * cos(asteroids[i].angle);
    asteroids[i].y += asteroids[i].speed * sin(asteroids[i].angle);

    if (asteroids[i].x < 0) asteroids[i].x = WIDTH - 1;
    if (asteroids[i].y < 0) asteroids[i].y = HEIGHT - 1;
    if (asteroids[i].x >= WIDTH) asteroids[i].x = 0;
    if (asteroids[i].y >= HEIGHT) asteroids[i].y = 0;
  }

  for (int i = 0; i <  BUL_MAX; i++) {
    bullets[i].x += bullets[i].speedX;
    bullets[i].y += bullets[i].speedY;
  }

  ship.x += ship.speedX;
  ship.y += ship.speedY;

  if (ship.x < 0) ship.x = WIDTH - 1;
  if (ship.y < 0) ship.y = HEIGHT - 1;
  if (ship.x >= WIDTH) ship.x = 0;
  if (ship.y >= HEIGHT) ship.y = 0;
}

void drawAsteroids(SDL_Renderer *renderer) {
  for (int i = 0; i < nbAsteroids; i++) {
    if (asteroids[i].r > 0) {
      float p0x, p0y, p1x, p1y, p2x, p2y;
      float deltaA = 2 * PI / AST_RES;
      p0x = asteroids[i].x + asteroids[i].p[0] * cos(0);
      p0y = asteroids[i].y + asteroids[i].p[0] * sin(0);
      p2x = p0x;
      p2y = p0y;
      for (int j = 1; j < AST_RES; j++) {
        float angle = j * deltaA;
        p1x = p2x;
        p1y = p2y;
        p2x = asteroids[i].x + asteroids[i].p[j] * cos(angle);
        p2y = asteroids[i].y + asteroids[i].p[j] * sin(angle);
        SDL_RenderDrawLine(renderer, (int)p1x, (int)p1y, (int)p2x, (int)p2y);
      }
      SDL_RenderDrawLine(renderer, (int)p2x, (int)p2y, (int)p0x, (int)p0y);
    }
  }
}

void drawBullets(SDL_Renderer *renderer) {
  for (int i = 0; i < BUL_MAX; i++)
    SDL_RenderDrawPoint(renderer, (int)bullets[i].x, (int)bullets[i].y);
}

void drawShip(SDL_Renderer *renderer, SDL_bool BOOST) {
  float ax = ship.x + 25 * cos(ship.angle);
  float ay = ship.y + 25 * sin(ship.angle);
  float bx = ship.x + 15 * cos(ship.angle + 2*PI/3);
  float by = ship.y + 15 * sin(ship.angle + 2*PI/3);
  float cx = ship.x + 15 * cos(ship.angle + 4*PI/3);
  float cy = ship.y + 15 * sin(ship.angle + 4*PI/3);
  SDL_RenderDrawLine(renderer, (int)ax, (int)ay, (int)bx, (int)by);
  SDL_RenderDrawLine(renderer, (int)ax, (int)ay, (int)cx, (int)cy);
  SDL_RenderDrawLine(renderer, (int)bx, (int)by, (int)ship.x, (int)ship.y);
  SDL_RenderDrawLine(renderer, (int)cx, (int)cy, (int)ship.x, (int)ship.y);

  if (BOOST) {
    float fx = ship.x + 12 * cos(ship.angle + PI);
    float fy = ship.y + 12 * sin(ship.angle + PI);
    float gx = ship.x + 22 * cos(ship.angle + PI);
    float gy = ship.y + 22 * sin(ship.angle + PI);
    SDL_RenderDrawLine(renderer, (int)fx, (int)fy, (int)gx, (int)gy);
  }
}

void fire() {
  bullets[bulletNext].x = ship.x + 30 * cos(ship.angle);
  bullets[bulletNext].y = ship.y + 30 * sin(ship.angle);
  bullets[bulletNext].speedX = BUL_SPEED * cos(ship.angle);
  bullets[bulletNext].speedY = BUL_SPEED * sin(ship.angle);
  if (++bulletNext == BUL_MAX) bulletNext = 0;
}

void drawCrash(SDL_Renderer *renderer) {
  for (int i = 0; i < 10; i++) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    drawShip(renderer, SDL_FALSE);
    SDL_RenderPresent(renderer);
    SDL_Delay(50);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    drawShip(renderer, SDL_FALSE);
    SDL_RenderPresent(renderer);
    SDL_Delay(50);
  }
}

void checkBulletCollision(Mix_Chunk *hitEffect, Mix_Chunk *newLevel) {
  int asteroidLeft = 0;
  for (int b = 0; b < BUL_MAX; b++)
    for (int a = 0; a < nbAsteroids; a++)
      if (asteroids[a].r >= AST_SIZE_MIN) {
        asteroidLeft++;
        if (pow(bullets[b].x - asteroids[a].x, 2) + pow(bullets[b].y - asteroids[a].y, 2) <= asteroids[a].r * asteroids[a].r) {
          bullets[b].x = -1;
          bullets[b].speedX = -1;
          if (asteroids[a].r == AST_SIZE_MIN)
            asteroids[a].r = 0;
          else {
            asteroids[nbAsteroids].x = asteroids[a].x;
            asteroids[nbAsteroids].y = asteroids[a].y;
            asteroids[a].r /= 2;
            asteroids[nbAsteroids].r = asteroids[a].r;
            for (int j=0; j < AST_RES; j++) {
              asteroids[a].p[j] /= 2;
              asteroids[nbAsteroids].p[j] = asteroids[a].p[j];
            }
            asteroids[a].speed *= 1.5;
            asteroids[nbAsteroids].speed =  asteroids[a].speed;
            asteroids[a].angle += PI / 6;
            asteroids[nbAsteroids].angle = asteroids[a].angle - PI / 3;
            nbAsteroids++;
          }
          Mix_PlayChannel(-1, hitEffect, 0);
          score += 1000 - (asteroids[a].r * 10);
          return;
        }
      }
  if(!asteroidLeft) { // new level
    level++;
    initLevel();
    score += 5000;
    Mix_PlayChannel(-1, newLevel, 0);
    SDL_Delay(1000);
  }
}

SDL_bool shipCollision() {
  float ax = ship.x + 25 * cos(ship.angle);
  float ay = ship.y + 25 * sin(ship.angle);
  float bx = ship.x + 15 * cos(ship.angle + 2*PI/3);
  float by = ship.y + 15 * sin(ship.angle + 2*PI/3);
  float cx = ship.x + 15 * cos(ship.angle + 4*PI/3);
  float cy = ship.y + 15 * sin(ship.angle + 4*PI/3);

  for (int i = 0; i < nbAsteroids; i++) {
    if (asteroids[i].r > 0 ) {
      if (pow(asteroids[i].x - ax, 2) + pow(asteroids[i].y - ay, 2) < pow(asteroids[i].r, 2))
        return SDL_TRUE;
      if (pow(asteroids[i].x - bx, 2) + pow(asteroids[i].y - by, 2) < pow(asteroids[i].r, 2))
        return SDL_TRUE;
      if (pow(asteroids[i].x - cx, 2) + pow(asteroids[i].y - cy, 2) < pow(asteroids[i].r, 2))
        return SDL_TRUE;
    }
  }
  return SDL_FALSE;
}

// for low spec computers : replace the above by :
//
// SDL_bool shipCollision() {
//   for (int i = 0; i < nbAsteroids; i++)
//     if (asteroids[i].r>0 && pow(ship.x - asteroids[i].x, 2) + pow(ship.y - asteroids[i].y, 2) < pow(asteroids[i].r + 15, 2))
//       return SDL_TRUE;
//   return SDL_FALSE;
// }

int main(int argc, char **argv) {
  const int frameDelay = 1000 / FPS;
  Uint32 frameStart = 0;
  int frameTime = 0;

  int fireRate = 0;

  SDL_Rect canvas = {0, 0, WIDTH, HEIGHT};

  SDL_bool BOOST = SDL_FALSE;
  SDL_bool LEFT  = SDL_FALSE;
  SDL_bool RIGHT = SDL_FALSE;
  SDL_bool SHOOT = SDL_FALSE;

  srand(time(NULL));

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  SDL_Window   *window   = SDL_CreateWindow("Asteroids", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  SDL_SetRenderDrawBlendMode(renderer,  SDL_BLENDMODE_BLEND);

  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
  Mix_Music *backgroundSound = Mix_LoadMUS("assets/Quake_II_-_Rage.mptm");
  Mix_Chunk *laserEffect = Mix_LoadWAV("assets/laser.wav");
  Mix_Chunk *hitEffect   = Mix_LoadWAV("assets/hit.wav");
  Mix_Chunk *crashEffect = Mix_LoadWAV("assets/crash.wav");
  Mix_Chunk *newLevel    = Mix_LoadWAV("assets/newLevel.wav");
  Mix_Chunk *trustEffect = Mix_LoadWAV("assets/trust.wav");
  trustEffect->volume = 20;
  Mix_PlayMusic(backgroundSound, -1);

  TTF_Init();
  TTF_Font *font = TTF_OpenFont("assets/FreeMono.ttf", 24);
  char scoreString[7] = {0};
  char highScoreString[7] = {0};
  SDL_Color textColor = {200, 200, 200, SDL_ALPHA_OPAQUE};
  SDL_Surface *sSurface = NULL;
  SDL_Texture *sTexture = NULL;
  SDL_Surface *hSurface = NULL;
  SDL_Texture *hTexture = NULL;
  SDL_Rect scoreRect = {50, 10, 0, 0};
  SDL_Rect highScoreRect = {WIDTH - 150, 10, 0, 0};

  SDL_bool isRunning = SDL_TRUE;
  SDL_Event event;

  initLevel();

  while (isRunning) {
    frameStart = SDL_GetTicks();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 64);
    SDL_RenderFillRect(renderer, &canvas);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) isRunning = SDL_FALSE;

      if (event.type ==  SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            isRunning = SDL_FALSE;
            break;
          case SDLK_LEFT:
            LEFT = SDL_TRUE;
            break;
          case SDLK_RIGHT:
            RIGHT = SDL_TRUE;
            break;
          case SDLK_UP:
            BOOST = SDL_TRUE;
            break;
          case SDLK_SPACE:
            SHOOT = SDL_TRUE;
            break;
        }
      }
      if (event.type ==  SDL_KEYUP) {
        switch (event.key.keysym.sym) {
          case SDLK_LEFT:
            LEFT = SDL_FALSE;
            break;
          case SDLK_RIGHT:
            RIGHT = SDL_FALSE;
            break;
          case SDLK_UP:
            BOOST = SDL_FALSE;
            break;
          case SDLK_SPACE:
            SHOOT = SDL_FALSE;
            break;
        }
      }
    }

    if (LEFT ) ship.angle -= 0.1;
    if (RIGHT) ship.angle += 0.1;

    fireRate++;
    if (SHOOT && fireRate > FPS / BPS) {
       fireRate = 0;
       Mix_PlayChannel(-1, laserEffect, 0);
       fire();
       if (score > 10) score -= 10;
     }

    if (BOOST) {
      ship.speedX += 0.1 * cos(ship.angle);
      ship.speedY += 0.1 * sin(ship.angle);
      Mix_PlayChannel(-1, trustEffect, 0);
    }

    updateScene();

    SDL_SetRenderDrawColor(renderer, 00, 255, 00, SDL_ALPHA_OPAQUE);
    drawShip(renderer, BOOST);
    SDL_SetRenderDrawColor(renderer, 255, 00, 00, SDL_ALPHA_OPAQUE);
    drawBullets(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 00, SDL_ALPHA_OPAQUE);
    drawAsteroids(renderer);

    checkBulletCollision(hitEffect, newLevel);

    if (shipCollision()) {  // game over
      Mix_PlayChannel(-1, crashEffect, 0);
      drawCrash(renderer);
      SDL_Delay(2000);

      if (highScore < score) {  // update high score
        highScore = score;
        sprintf(highScoreString, "%.6d", highScore);
        hSurface = TTF_RenderText_Solid(font, highScoreString, textColor);
        hTexture = SDL_CreateTextureFromSurface(renderer, hSurface);
        highScoreRect.w = hSurface->w;
        highScoreRect.h = hSurface->h;
      }
      score = 0;
      level = 1;
      initLevel();
    }

    if (score > 0) score--;
    sprintf(scoreString, "%.6d", score);
    sSurface = TTF_RenderText_Solid(font, scoreString, textColor);
    sTexture = SDL_CreateTextureFromSurface(renderer, sSurface);
    scoreRect.w = sSurface->w;
    scoreRect.h = sSurface->h;
    SDL_RenderCopy(renderer, sTexture, NULL, &scoreRect);
    SDL_RenderCopy(renderer, hTexture, NULL, &highScoreRect);

    frameTime = SDL_GetTicks() - frameStart;
    if (frameDelay > frameTime) SDL_Delay(frameDelay - frameTime);

    SDL_RenderPresent(renderer);
  }

  Mix_FreeChunk(hitEffect);
  Mix_FreeChunk(crashEffect);
  Mix_FreeChunk(laserEffect);
  Mix_FreeChunk(trustEffect);
  Mix_FreeChunk(newLevel);
  Mix_CloseAudio();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  exit(EXIT_SUCCESS);
}
