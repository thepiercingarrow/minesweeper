#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#define WIDTH_MAX 100
#define HEIGHT_MAX 100

typedef struct {
  char cells[WIDTH_MAX][HEIGHT_MAX];
} Landmap;

void render(Landmap land, int curx, int cury);
void flood(Landmap land, Landmap map, int x, int y);
void end_game(Landmap mines);

int width = 10, height = 10, num_mine = 10;

FILE *fdb;

int main() {
  fdb = fopen("debug", "w");
  Landmap land, map, mines;
  memset(land.cells, '#', WIDTH_MAX * HEIGHT_MAX * sizeof(char));
  memset(mines.cells, 0, sizeof(Landmap));
  sranddev();

  int i, x, y;
  for (i = 0; i < num_mine; ++i) {
    x = rand() % width, y = rand() % height;
    if (mines.cells[x][y])
      --i;
    else
      mines.cells[x][y] = 1;
  }

  int j;
  for (i = 0; i < width; ++i) {
    for (j = 0; j < height; ++j) {
      int k, l, tot = 0;
      for (k = -1; k <= 1; ++k)
	for (l = -1; l <= 1; ++l)
	  tot += (0 <= (i+k) && (i+k) < width) && (0 <= (j+l) && (j+l) < height) ? mines.cells[i+k][j+l] : 0;
      map.cells[i][j] = tot ? tot + '0' : ' ';
    }
  }

  initscr();
  keypad(stdscr, TRUE);
  raw();
  noecho();
  start_color();

  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_BLUE);
  init_pair(3, COLOR_RED, COLOR_RED);

  int curx = 0, cury = 0;
  for (;;) {
    render(land, curx, cury);
    switch (getch()) {
    case KEY_UP:
      if (cury > 0)
	--cury;
      break;
    case KEY_DOWN:
      if (cury < height - 1)
	++cury;
      break;
    case KEY_LEFT:
      if (curx > 0)
	--curx;
      break;
    case KEY_RIGHT:
      if (curx < width - 1)
	++curx;
      break;
    case 'u':
      if (mines.cells[curx][cury])
	end_game(mines);
      if (land.cells[curx][cury] == '#')
	flood(land, map, curx, cury);
      break;
    case 'q':
      endwin();
      exit(0);
    }
  }
  return 0; // shouldn't be reached
}

void render(Landmap land, int curx, int cury) {
  fprintf(fdb, "render");
  clear();

  int i, j;
  attron(COLOR_PAIR(1));
  for (i = 0; i < width; ++i)
    for (j = 0; j < height; ++j)
      mvaddch(j, 2 * i, land.cells[i][j]);
  attroff(COLOR_PAIR(1));

  attron(COLOR_PAIR(2));
  mvaddch(cury, 2 * curx, land.cells[curx][cury]);
  attroff(COLOR_PAIR(2));

  refresh();
}

void flood(Landmap land, Landmap map, int x, int y) {
  fprintf(fdb, "%d %d\n", x, y);
  if (x < 0 || x >= width || y < 0 || y >= height)
    return;
  if (land.cells[x][y] == map.cells[x][y])
    return;
  land.cells[x][y] = map.cells[x][y];
  if (land.cells[x][y] == ' ') {
    flood(land, map, x + 1, y);
    flood(land, map, x, y + 1);
    flood(land, map, x - 1, y);
    flood(land, map, x, y - 1);
  }
}

void end_game(Landmap mines) {
  int i, j;
  attron(COLOR_PAIR(3));
  for (i = 0; i < width; ++i)
    for (j = 0; j < height; ++j)
      if (mines.cells[i][j])
	mvaddch(j, 2 * i, ' ');
  attroff(COLOR_PAIR(3));
  mvprintw(height, 0, "You lose!");
  getch();
  endwin();
  exit(1);
}
