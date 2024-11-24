#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#define FACTOR 50

typedef struct {
	Vector2 pos;
	Vector2 dir;
} Arrow;

Arrow ArrowTo(Vector2 from, Vector2 to) {
	return (Arrow){
		.pos = from,
		.dir = Vector2Subtract(to, from),
	};
}

Vector2 ArrowEnd(Arrow a) {
	return Vector2Add(a.pos, a.dir);
}

void ArrowDraw(Arrow a, Color color) {
	DrawLineV(a.pos, ArrowEnd(a), color);
}

Arrow ArrowReflect(Arrow a, Arrow mirror) {
	return (Arrow){
		.pos = mirror.pos,
		.dir = Vector2Reflect(
			a.dir, Vector2Normalize(Vector2Rotate(mirror.dir, PI / 2))
		),
	};
}

int main(void) {
	int width  = 16 * FACTOR;
	int height = 16 * FACTOR;
	InitWindow(width, height, "bounce");
	SetTargetFPS(60);

	while(!WindowShouldClose()) {
		width  = GetScreenWidth();
		height = GetScreenHeight();

		BeginDrawing();
		ClearBackground(BLACK);

		// draw mouse vector
		Arrow mouse = ArrowTo(GetMousePosition(), CLITERAL(Vector2){0, height});
		ArrowDraw(mouse, WHITE);

		// draw left boundary
		Arrow boundary = {
			.pos = {0, height},
			.dir = {width, 0},
		};
		ArrowDraw(boundary, WHITE);

		// initial light
		Arrow light = {
			.pos = {width, mouse.pos.y},
			.dir = {-width, 0},
		};

		size_t i = 0;
		for(i = 0; i < 100; i++) {
			Arrow mirror = i % 2 == 0 ? mouse : boundary;

			bool ok = CheckCollisionLines(
				light.pos, ArrowEnd(light), mirror.pos, ArrowEnd(mirror),
				&mirror.pos
			);
			if(!ok) break;

			DrawCircleV(mirror.pos, 5, BLUE);

			light = ArrowTo(light.pos, mirror.pos);
			ArrowDraw(light, YELLOW);

			light     = ArrowReflect(light, mirror);
			light.dir = Vector2Scale(light.dir, 1000);
		}

		float angle = -Vector2Angle(mouse.dir, CLITERAL(Vector2){0}) * 180 / PI;
		DrawText(
			TextFormat("angle: %f°\nbounces: %d", angle, i), 50, 50, 24, WHITE
		);
		DrawFPS(width - 100, 50);
		EndDrawing();
	}
}
