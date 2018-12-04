#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ncurses.h>

#define IN_BOUNDS(x, y) ((0 <= x && x < width) && (0 <= y && y < height))

#define WIDTH_MAX 100
#define HEIGHT_MAX 100

typedef struct {
  char cells[WIDTH_MAX][HEIGHT_MAX];
} Landmap;

void render(int curx, int cury);
void flood(int x, int y);
void lose();
void win();

int width = 10, height = 10, num_mine = 10;
Landmap land, map, mines;
int uncovered = 0;
FILE *fdb;

int main() {
  fdb = fopen("debug", "w");
  memset(land.cells, '#', WIDTH_MAX * HEIGHT_MAX * sizeof(char));
  memset(mines.cells, 0, sizeof(Landmap));
  srand(time(NULL));

  int i, x, y;
  for (i = 0; i < num_mine; ++i) {
    x = rand() % width, y = rand() % height;
    if (mines.cells[x][y])
      --i;
    else
      mines.cells[x][y] = 1;
  }

  // this can be written much betterly :P
  int j;
  for (i = 0; i < width; ++i) {
    for (j = 0; j < height; ++j) {
      if (mines.cells[i][j]) {
	map.cells[i][j] = '#';
	continue;
      }
      int k, l, tot = 0;
      for (k = -1; k <= 1; ++k)
	for (l = -1; l <= 1; ++l)
	  tot += IN_BOUNDS(i + k, j + l) ? mines.cells[i+k][j+l] : 0;
      map.cells[i][j] = tot ? tot + '0' : ' ';
    }
  }

  initscr();
  raw();
  noecho();
  keypad(stdscr, 1);
  start_color();

  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_BLUE);
  init_pair(3, COLOR_RED, COLOR_RED);

  int curx = 0, cury = 0;
  for (;;) {
    render(curx, cury);
    switch (getch()) {
    case 'k': case KEY_UP:
      if (cury > 0)
	--cury;
      break;
    case 'j': case KEY_DOWN:
      if (cury < height - 1)
	++cury;
      break;
    case 'h': case KEY_LEFT:
      if (curx > 0)
	--curx;
      break;
    case 'l': case KEY_RIGHT:
      if (curx < width - 1)
	++curx;
      break;
    case 'u': case ' ':
      if (mines.cells[curx][cury])
	lose(mines);
      if (land.cells[curx][cury] == '#')
	flood(curx, cury);
      break;
    case 'q':
      endwin();
      exit(0);
    }
  }
  return 0; // shouldn't be reached
}

void render(int curx, int cury) {
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

void flood(int x, int y) {
  if (x < 0 || x >= width || y < 0 || y >= height)
    return;
  if (land.cells[x][y] == map.cells[x][y])
    return;
  fprintf(fdb, "%d %d\n", x, y);
  land.cells[x][y] = map.cells[x][y];
  ++uncovered;
  if (uncovered == width * height - num_mine)
    win();
  if (land.cells[x][y] == ' ') {
    flood(x + 1, y);
    flood(x, y + 1);
    flood(x - 1, y);
    flood(x, y - 1);
  }
}

void lose() {
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
  exit(0);
}

void win() {
  int i, j;
  attron(COLOR_PAIR(3));
  for (i = 0; i < width; ++i)
    for (j = 0; j < height; ++j)
      if (mines.cells[i][j])
	mvaddch(j, 2 * i, ' ');
  attroff(COLOR_PAIR(3));
  mvprintw(height, 0, "You win!");
  getch();
  endwin();
  exit(0);
}
