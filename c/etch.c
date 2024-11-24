#include <SDL2/SDL.h>
#include <err.h>
#include <time.h>

#define sdl(x, msg) if(x < 0) \
	errx(1, "%d: %s: %s", __LINE__, msg, SDL_GetError())

int main() {
	struct timespec time = {0};
	clock_gettime(CLOCK_REALTIME, &time);
	srand(time.tv_sec * time.tv_nsec);

	sdl(SDL_Init(SDL_INIT_EVERYTHING), "could not init SDL");

	SDL_DisplayMode mode = {0};
	sdl(SDL_GetCurrentDisplayMode(0, &mode), "could not get display mode");

	SDL_Window*   window = NULL;
	SDL_Renderer* render = NULL;
	sdl(SDL_CreateWindowAndRenderer(mode.w, mode.h,
		SDL_WINDOW_FULLSCREEN, &window, &render),
		"could not create window & renderer"
	);

	SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
	SDL_RenderClear(render);
	// SDL_SetRenderDrawColor(render, 255, 255, 255, 255);

	int x = mode.w >> 1;
	int y = mode.h >> 1;
	int size = 50;

	SDL_Event e = {0};
	while(SDL_WaitEvent(&e)) {
		switch(e.type) {
		case SDL_QUIT: goto end;
		case SDL_KEYDOWN:
			switch(e.key.keysym.sym) {
			case SDLK_UP:    y -= size; break;
			case SDLK_DOWN:  y += size; break;
			case SDLK_LEFT:  x -= size; break;
			case SDLK_RIGHT: x += size; break;
			}
			break;
		}

		// SDL_RenderDrawPoint(render, x, y);
		SDL_Rect rect = {
			.x = x,
			.y = y,
			.w = size,
			.h = size,
		};
		// SDL_SetRenderDrawColor(render, random(), random(), random(), 255);
		SDL_SetRenderDrawColor(render, x * 256 / mode.w, y * 256 / mode.h, random() & 10 + 100, 255);
		SDL_RenderFillRect(render, &rect);
		SDL_RenderPresent(render);
	}

end:
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
