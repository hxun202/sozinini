#pragma once

typedef struct
{
	int width;
	int height;
	char** data;
} Map;

Map* loadMap(const char* filename);
void freemap(Map* map);
char getile(Map* map, int x, int y);