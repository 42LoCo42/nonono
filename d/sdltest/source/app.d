import std.algorithm;
import std.math;
import std.stdio;

import bindbc.sdl;

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

void main() {
	auto sdlSupport = loadSDL();
	if(sdlSupport == SDLSupport.noLibrary) {
		writeln("Could not find SDL library");
		return;
	}
	if(sdlSupport == SDLSupport.badLibrary) {
		writeln("Could not load SDL library");
		return;
	}
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		writeln("Could not init SDL: ", SDL_GetError());
		return;
	}

	auto window = SDL_CreateWindow(
		"SDL Test",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	if(window == null) {
		writeln("Could not create window: ", SDL_GetError());
		return;
	}

	auto renderer = SDL_CreateRenderer(
		window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if(renderer == null) {
		writeln("Could not create renderer: ", SDL_GetError());
		return;
	}

	SDL_Vertex[] vertices = [
		{{WINDOW_WIDTH / 2, 0},         {255, 0, 0, 255}},
		{{0,            WINDOW_HEIGHT}, {0, 255, 0, 255}},
		{{WINDOW_WIDTH, WINDOW_HEIGHT}, {0, 0, 255, 255}},
	];

	int time = 0;

	auto quit = false;
	while(!quit) {
		SDL_Event ev;
		while(SDL_PollEvent(&ev) != 0) {
			switch(ev.type) {
				case SDL_QUIT:
					quit = true;
				break;

				case SDL_KEYDOWN:
					if(ev.key.keysym.sym == SDLK_q) {
						quit = true;
					}
				break;

				default:
			}
		}

		time++;

		double linearPeriodic(int period, int phase = 0) {
			double raw = (time + phase) % period;
			if(raw > period / 2) raw = period - raw;
			return raw * 2 / period;
		}

		double sinPeriodic(int period, int phase = 0) {
			return sin(cast(double) (time + phase) * 2 * PI / period) / 2 + 0.5;
		}

		SDL_Color rainbow(int raw) {
			SDL_Color result = {0, 0, 0, 0xFF};
			int   segment = raw / 256;
			ubyte value   = cast(ubyte) (raw % 256);
			switch(segment) {
				case 0: case 6:
					result.r = 0xFF;
					result.g = value;
				break;
				case 1:
					result.r = 0xFF - value;
					result.g = 0xFF;
				break;
				case 2:
					result.g = 0xFF;
					result.b = value;
				break;
				case 3:
					result.g = 0xFF - value;
					result.b = 0xFF;
				break;
				case 4:
					result.r = value;
					result.b = 0xFF;
				break;
				case 5:
					result.r = 0xFF;
					result.b = 0xFF - value;
				break;
				default:
			}
			return result;
		}

		vertices[0].position.x = sinPeriodic(120) * WINDOW_WIDTH;
		vertices[1].position.y = sinPeriodic(120) * WINDOW_HEIGHT;
		vertices[2].position.x = min(2 * sinPeriodic(300) * WINDOW_WIDTH, WINDOW_WIDTH);
		vertices[2].position.y = min(2 * sinPeriodic(300, 150) * WINDOW_HEIGHT, WINDOW_HEIGHT);

		auto now = rainbow(cast(int) (sinPeriodic(360) * 256 * 6));
		SDL_SetRenderDrawColor(renderer, now.r, now.g, now.b, now.a);
		SDL_RenderClear(renderer);

		SDL_RenderGeometry(
			renderer, null,
			vertices.ptr, cast(int) vertices.length,
			null, 0
		);

		SDL_RenderPresent(renderer);
	}


	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
