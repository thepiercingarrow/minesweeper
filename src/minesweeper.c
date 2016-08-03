#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#define WIDTH_MAX 100
#define HEIGHT_MAX 100

void render(char land[WIDTH_MAX][HEIGHT_MAX], int curx, int cury);
void flood(char land[WIDTH_MAX][HEIGHT_MAX], char map[WIDTH_MAX][HEIGHT_MAX], int x, int y);
void end_game(char mines[WIDTH_MAX][HEIGHT_MAX]);

int width = 10, height = 10, num_mine = 10;

FILE *fdb;

int main() {
  fdb = fopen("debug", "w");
  char land[WIDTH_MAX][HEIGHT_MAX], map[WIDTH_MAX][HEIGHT_MAX], mines[WIDTH_MAX][HEIGHT_MAX];
  memset(land, '#', sizeof land);
  memset(mines, 0, sizeof mines);
  sranddev();

  int i;
  for (i = 0; i < num_mine; ++i) {
    int x = rand() % 10, y = rand() % 10;
    if (mines[x][y])
      --i;
    else
      mines[x][y] = 1;
  }

  int j;
  for (i = 0; i < width; ++i) {
    for (j = 0; j < height; ++j) {
      int k, l, tot = 0;
      for (k = -1; k <= 1; ++k)
	for (l = -1; l <= 1; ++l)
	  tot += (0 <= (i+k) && (i+k) < width) && (0 <= (j+l) && (j+l) < height) ? mines[i+k][j+l] : 0;
      map[i][j] = tot ? tot + '0' : ' ';
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

  int c, curx = 0, cury = 0;
  printw("Press any key to continue...");
  for (;;) {
    switch (getch()) {
    case KEY_UP:
      if (cury != 0)
	--cury;
      break;
    case KEY_DOWN:
      if (cury != height - 1)
	++cury;
      break;
    case KEY_LEFT:
      if (curx != 0)
	--curx;
      break;
    case KEY_RIGHT:
      if (curx != width - 1)
	++curx;
      break;
    case '\n':
      if (mines[curx][cury])
	end_game(mines);
      if (land[curx][cury] == '#')
	flood(land, map, curx, cury);
      break;
    case 'q':
      endwin();
      exit(0);
    }

    render(land, curx, cury);
  }
}

void render(char land[WIDTH_MAX][HEIGHT_MAX], int curx, int cury) {
  clear();

  int i, j;
  attron(COLOR_PAIR(1));
  for (i = 0; i < width; ++i)
    for (j = 0; j < height; ++j)
      mvaddch(j, 2 * i, land[i][j]);
  attroff(COLOR_PAIR(1));

  attron(COLOR_PAIR(2));
  mvaddch(cury, 2 * curx, land[curx][cury]);
  attroff(COLOR_PAIR(2));

  refresh();
}

void flood(char land[WIDTH_MAX][HEIGHT_MAX], char map[WIDTH_MAX][HEIGHT_MAX], int x, int y) {
  fprintf(fdb, "%d %d\n", x, y);
  if (x < 0 || x >= width || y < 0 || y >= height)
    return;
  if (land[x][y] == map[x][y])
    return;
  land[x][y] = map[x][y];
  if (land[x][y] == ' ') {
    flood(land, map, x + 1, y);
    flood(land, map, x, y + 1);
    flood(land, map, x - 1, y);
    flood(land, map, x, y - 1);
  }
}

void end_game(char mines[WIDTH_MAX][HEIGHT_MAX]) {
  int i, j;
  attron(COLOR_PAIR(3));
  for (i = 0; i < width; ++i)
    for (j = 0; j < height; ++j)
      if (mines[i][j])
	mvaddch(j, 2 * i, ' ');
  attroff(COLOR_PAIR(3));
  mvprintw(height, 0, "You lose!");
  getch();
  endwin();
  exit(1);
}
