#include "raylib.h"

#include <iostream>

int main()
{
	InitWindow(800, 600, "Hello, World!");
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(DARKBLUE);
		DrawText("Hello, world!", 10, 10, 20, RAYWHITE);
		EndDrawing();
	}
	CloseWindow();

	return 0;
}
