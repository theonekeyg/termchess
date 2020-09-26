#include <stdlib.h>
#include <stdbool.h>

#include <ncurses.h>

#include "termchess.h"

static const int SIDE_NONE  = -1;
static const int SIDE_WHITE = 0;
static const int SIDE_BLACK = 1;

enum PIECES_ENUM {
  PIECE_KING,
  PIECE_QUEEN,
  PIECE_ROOK,
  PIECE_BISHOP,
  PIECE_KNIGHT,
  PIECE_PAWN,
  PIECE_NONE,
};

const char *PIECES_WHITE[] = {
  "@", "Q", "R", "B", "N", "P"
};
const char *PIECES_BLACK[] = {
  "#", "q", "r", "b", "n", "p"
};

/* sl - side left;   sr - side right; st - side top;
 * sb - side bottom; tl - top left;   tr - top right;
 * bl - bottom left; br - bottom right */
struct winBorder {
  char sl; char sr; char st;
  char sb; char tl; char tr;
  char bl; char br;
};
static const struct winBorder WIN_BORDER = {
  '|', '|', '-', '-',
  '+', '+', '+', '+'
};
static bool HAS_COLORS = FALSE;
/* Coordinates of current square */
static int cord_x = 0;
static int cord_y = 0;
static int prev_cord_x = 0;
static int prev_cord_y = 0;
static const int square_midx = SQUARE_SIZEX / 2;
static const int square_midy = SQUARE_SIZEY / 2;
static struct SquareRepr *FOCUSED_SQUARE = NULL;

void
FOCUSED_SQUARE_init(struct SquareRepr *square) {
  FOCUSED_SQUARE = square;
  for (int i=0; i<FOCUSED_SQUARE->opts.opt_len; ++i) {
    wattron(FOCUSED_SQUARE->opts.opt[i].win, A_REVERSE);
  }
}

void
FOCUSED_SQUARE_free() {
  if (FOCUSED_SQUARE) {
    for (int i=0; i<FOCUSED_SQUARE->opts.opt_len; ++i) {
      wattroff(FOCUSED_SQUARE->opts.opt[i].win, A_REVERSE);
    }
    TurnOptions_render(&FOCUSED_SQUARE->opts);
    FOCUSED_SQUARE = NULL;
  }
}

void
TurnOptions_init(struct TurnOptions *opts) {
  opts->opt = (struct SquareRepr *) malloc(0);
  opts->opt_len = 0;
}

void
TurnOptions_free(struct TurnOptions *opts) {
  if (opts->opt) {
    free(opts->opt);
  }
  opts->opt_len = 0;
}

void
TurnOptions_addnew(struct TurnOptions *opts, struct SquareRepr *square) {
  /* FIXME: Currently this makes unnecessary copying into opts->opt array.
   * replace copying with pointers */
  if (square) {
    opts->opt = (struct SquareRepr *) \
                realloc(opts->opt, sizeof(struct SquareRepr) * ++opts->opt_len);
    opts->opt[opts->opt_len-1] = *square;
  }
}

void
TurnOptions_render(struct TurnOptions *options) {
#ifdef DEBUG
  printw("%d - ", options->opt_len);
#endif
  for (int i=0; i<options->opt_len; ++i) {
    win_render(options->opt[i].win);
    wrefresh(options->opt[i].win);
  }
}

void
TurnOptions_parse_rook(struct SquareRepr *square, struct BoardRepr *board) {
  for (int x=square->x+1; x < BOARD_SIZE; ++x) {
    if (board->squares[square->y][x].isoccu == TRUE) {
      if (board->squares[square->y][x].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[square->y][x]);
      }
      break;
    }
    TurnOptions_addnew(&square->opts, &board->squares[square->y][x]);
  }
  for (int x=square->x-1; x >= 0; --x) {
    if (board->squares[square->y][x].isoccu == TRUE) {
      if (board->squares[square->y][x].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[square->y][x]);
      }
      break;
    }
    TurnOptions_addnew(&square->opts, &board->squares[square->y][x]);
  }
  for (int y=square->y+1; y < BOARD_SIZE; ++y) {
    if (board->squares[y][square->x].isoccu == TRUE) {
      if (board->squares[y][square->x].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[y][square->x]);
      }
      break;
    }
    TurnOptions_addnew(&square->opts, &board->squares[y][square->x]);
  }
  for (int y=square->y-1; y >= 0; --y) {
    if (board->squares[y][square->x].isoccu == TRUE) {
      if (board->squares[y][square->x].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[y][square->x]);
      }
      break;
    }
    TurnOptions_addnew(&square->opts, &board->squares[y][square->x]);
  }
}

void
TurnOptions_parse_diagonal(struct SquareRepr *square, struct BoardRepr *board) {
  for (int x=square->x+1, y=square->y+1; (x < BOARD_SIZE) && (y < BOARD_SIZE); ++x, ++y) {
    if (board->squares[y][x].isoccu == TRUE) {
      if (board->squares[y][x].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      }
      break;
    }
    TurnOptions_addnew(&square->opts, &board->squares[y][x]);
  }
  for (int x=square->x-1, y=square->y+1; (x >= 0) && (y < BOARD_SIZE); --x, ++y) {
    if (board->squares[y][x].isoccu == TRUE) {
      if (board->squares[y][x].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      }
      break;
    }
    TurnOptions_addnew(&square->opts, &board->squares[y][x]);
  }
  for (int x=square->x+1, y=square->y-1; (x < BOARD_SIZE) && (y >= 0); ++x, --y) {
    if (board->squares[y][x].isoccu == TRUE) {
      if (board->squares[y][x].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      }
      break;
    }
    TurnOptions_addnew(&square->opts, &board->squares[y][x]);
  }
  for (int x=square->x-1, y=square->y-1; (x >= 0) && (y >= 0); --x, --y) {
    if (board->squares[y][x].isoccu == TRUE) {
      if (board->squares[y][x].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      }
      break;
    }
    TurnOptions_addnew(&square->opts, &board->squares[y][x]);
  }
}

void
TurnOptions_parse_knight(struct SquareRepr *square, struct BoardRepr *board) {
  int x, y;
  x = square->x+2;
  if (x < BOARD_SIZE) {
    y = square->y-1;
    if (y >= 0) {
      if (board->squares[y][x].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      else if (board->squares[y][x].piece.side ^ square->piece.side)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
    }
    y = square->y+1;
    if (y < BOARD_SIZE) {
      if (board->squares[y][x].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      else if (board->squares[y][x].piece.side ^ square->piece.side)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
    }
  }
  x = square->x-2;
  if (x >= 0) {
    y = square->y-1;
    if (y >= 0) {
      if (board->squares[y][x].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      else if (board->squares[y][x].piece.side ^ square->piece.side)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
    }
    y = square->y+1;
    if (y < BOARD_SIZE) {
      if (board->squares[y][x].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      else if (board->squares[y][x].piece.side ^ square->piece.side)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
    }
  }
  y = square->y+2;
  if (y < BOARD_SIZE) {
    x = square->x-1;
    if (x >= 0) {
      if (board->squares[y][x].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      else if (board->squares[y][x].piece.side ^ square->piece.side)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
    }
    x = square->x+1;
    if (x < BOARD_SIZE) {
      if (board->squares[y][x].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      else if (board->squares[y][x].piece.side ^ square->piece.side)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
    }
  }
  y = square->y-2;
  if (y >= 0) {
    x = square->x-1;
    if (x >= 0) {
      if (board->squares[y][x].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      else if (board->squares[y][x].piece.side ^ square->piece.side)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
    }
    x = square->x+1;
    if (x < BOARD_SIZE) {
      if (board->squares[y][x].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      else if (board->squares[y][x].piece.side ^ square->piece.side)
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
    }
  }
}

void 
TurnOptions_parse_king(struct SquareRepr *square, struct BoardRepr *board) {
  /* TODO: add castling rights */
  if (square->x-1 >= 0) {
    if (board->squares[square->y][square->x-1].isoccu == FALSE)
      TurnOptions_addnew(&square->opts, &board->squares[square->y][square->x-1]);
    else if (board->squares[square->y][square->x-1].piece.side ^ square->piece.side)
      TurnOptions_addnew(&square->opts, &board->squares[square->y][square->x-1]);
    if (square->y-1 >= 0) {
      if (board->squares[square->y-1][square->x-1].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[square->y-1][square->x-1]);
      if (board->squares[square->y-1][square->x-1].piece.side ^ square->piece.side)
        TurnOptions_addnew(&square->opts, &board->squares[square->y-1][square->x-1]);
    }
    if (square->y+1 >= 0) {
      if (board->squares[square->y+1][square->x-1].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[square->y+1][square->x-1]);
      if (board->squares[square->y+1][square->x-1].piece.side ^ square->piece.side)
        TurnOptions_addnew(&square->opts, &board->squares[square->y+1][square->x-1]);
    }
  }

  if (square->x+1 < BOARD_SIZE) {
    if (board->squares[square->y][square->x+1].isoccu == FALSE)
      TurnOptions_addnew(&square->opts, &board->squares[square->y][square->x+1]);
    else if (board->squares[square->y][square->x+1].piece.side ^ square->piece.side)
      TurnOptions_addnew(&square->opts, &board->squares[square->y][square->x+1]);
    if (square->y-1 >= 0) {
      if (board->squares[square->y-1][square->x+1].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[square->y-1][square->x+1]);
      if (board->squares[square->y-1][square->x+1].piece.side ^ square->piece.side)
        TurnOptions_addnew(&square->opts, &board->squares[square->y-1][square->x+1]);
    }
    if (square->y+1 >= 0) {
      if (board->squares[square->y+1][square->x+1].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[square->y+1][square->x+1]);
      if (board->squares[square->y+1][square->x+1].piece.side ^ square->piece.side)
        TurnOptions_addnew(&square->opts, &board->squares[square->y+1][square->x+1]);
    }
  }

  /* FIXME: These checks could be replaced by checks above, but that would result in code duplication,
   * since they would have to be checked in both `square->x-1` and `square->x+1`, need to 
   * check if code duplication cost is worth it in terms of performance gain from removing
   * unnecessary checks */
  if (square->y-1 >= 0) {
    if (board->squares[square->y-1][square->x].isoccu == FALSE)
      TurnOptions_addnew(&square->opts, &board->squares[square->y-1][square->x]);
    else if (board->squares[square->y-1][square->x].piece.side ^ square->piece.side)
      TurnOptions_addnew(&square->opts, &board->squares[square->y-1][square->x]);
  }
  if (square->y+1 < BOARD_SIZE) {
    if (board->squares[square->y+1][square->x].isoccu == FALSE)
      TurnOptions_addnew(&square->opts, &board->squares[square->y+1][square->x]);
    else if (board->squares[square->y+1][square->x].piece.side ^ square->piece.side)
      TurnOptions_addnew(&square->opts, &board->squares[square->y+1][square->x]);
  }
}

void
TurnOptions_exclude_unsafe(struct SquareRepr *square, struct BoardRepr *board) {
}

void
SquareRepr_set(struct SquareRepr *square, int piece, bool isoccu, bool side) {
  square->piece.type = piece;
  square->piece.side = side;
  square->isoccu     = isoccu;
}

void
SquareRepr_parse_options(struct SquareRepr *square, struct BoardRepr *board) {
  if (square->isoccu == TRUE) {
    switch(square->piece.type) {
      case PIECE_PAWN:
        if (square->piece.side == SIDE_WHITE) {
          /* Check move 1 step ahead */
          if (board->squares[square->y+1][square->x].isoccu == FALSE) {
            TurnOptions_addnew(&square->opts, &board->squares[square->y+1][square->x]);
            /* Check move 2 steps ahead */
            if (square->piece.has_moved == FALSE &&
                board->squares[square->y+2][square->x].isoccu == FALSE) {
              TurnOptions_addnew(&square->opts, &board->squares[square->y+2][square->x]);
            }
          }
          /* FIXME: indexing to `square->x(-+)1` is unsafe, specifically
           * when square->x equals to 0 or BOARD_SIZE */
          /* Check taking the opponents piece */
          if (board->squares[square->y+1][square->x-1].isoccu == TRUE &&
              board->squares[square->y+1][square->x-1].piece.side ^ square->piece.side) {
            TurnOptions_addnew(&square->opts, &board->squares[square->y+1][square->x-1]);
          }
          if (board->squares[square->y+1][square->x+1].isoccu == TRUE &&
              board->squares[square->y+1][square->x+1].piece.side ^ square->piece.side) {
            TurnOptions_addnew(&square->opts, &board->squares[square->y+1][square->x+1]);
          }
        }
        else if (square->piece.side == SIDE_BLACK) {
          /* Check move 1 step ahead */
          if (board->squares[square->y-1][square->x].isoccu == FALSE) {
            TurnOptions_addnew(&square->opts, &board->squares[square->y-1][square->x]);
            /* Check move 2 steps ahead */
            if (square->piece.has_moved == FALSE &&
                board->squares[square->y-2][square->x].isoccu == FALSE) {
              TurnOptions_addnew(&square->opts, &board->squares[square->y-2][square->x]);
            }
          }
          /* Check taking the opponents piece */
          if (board->squares[square->y-1][square->x-1].isoccu == TRUE &&
              board->squares[square->y-1][square->x-1].piece.side ^ square->piece.side) {
            TurnOptions_addnew(&square->opts, &board->squares[square->y-1][square->x-1]);
          }
          if (board->squares[square->y-1][square->x+1].isoccu == TRUE &&
              board->squares[square->y-1][square->x+1].piece.side ^ square->piece.side) {
            TurnOptions_addnew(&square->opts, &board->squares[square->y-1][square->x+1]);
          }
        }
        break;

      case PIECE_KNIGHT:
        TurnOptions_parse_knight(square, board);
        break;

      case PIECE_BISHOP:
        TurnOptions_parse_diagonal(square, board);
        break;

      case PIECE_ROOK:
        TurnOptions_parse_rook(square, board);
        break;

      case PIECE_QUEEN:
        TurnOptions_parse_diagonal(square, board);
        TurnOptions_parse_rook(square, board);
        break;

      case PIECE_KING:
        TurnOptions_parse_king(square, board);
        break;
    }
  }
}

void
SquareRepr_init(struct SquareRepr *square, int y, int x) {
  bool side = SIDE_WHITE;

  square->y = y;
  square->x = x;
  TurnOptions_init(&square->opts);

  switch (y) {
    case 7:
      side = SIDE_BLACK;
    case 0:
      switch (x) {
        case 0:
        case 7:
          SquareRepr_set(square, PIECE_ROOK, TRUE, side);
          break;
        case 1:
        case 6:
          SquareRepr_set(square, PIECE_KNIGHT, TRUE, side);
          break;
        case 2:
        case 5:
          SquareRepr_set(square, PIECE_BISHOP, TRUE, side);
          break;
        case 3:
          SquareRepr_set(square, PIECE_QUEEN, TRUE, side);
          break;
        case 4:
          SquareRepr_set(square, PIECE_KING, TRUE, side);
          break;
      }
      break;
    case 6:
      side = SIDE_BLACK;
    case 1:
      SquareRepr_set(square, PIECE_PAWN, TRUE, side);
      break;
    default:
      SquareRepr_set(square, PIECE_NONE, FALSE, SIDE_NONE);
      break;
  }
  square->piece.has_moved = FALSE;
}

void
win_render(WINDOW *win) {
  wborder(win, WIN_BORDER.sl, WIN_BORDER.sr, WIN_BORDER.st, WIN_BORDER.sb,
          WIN_BORDER.tl, WIN_BORDER.tr, WIN_BORDER.bl, WIN_BORDER.br);
}

void
win_render_piece(struct SquareRepr *square) {
  win_render(square->win);
  if (square->isoccu == FALSE) { return; }

  char **pieces = NULL;
  if (square->piece.side == SIDE_WHITE)
    pieces = (char **)PIECES_WHITE;
  else if (square->piece.side == SIDE_BLACK)
    pieces = (char **)PIECES_BLACK;

  if (pieces) {
    mvwprintw(square->win, square_midy, square_midx, pieces[square->piece.type]);
  }
}

void
win_clear(WINDOW *win) {
  werase(win);
  win_render(win);
}

void
win_activate(WINDOW *win, bool flag) {
  if (flag == TRUE) {
    if (HAS_COLORS) {
      wattron(win, COLOR_PAIR(1));
      win_render(win);
      wattroff(win, COLOR_PAIR(1));
    } else {
      wattron(win, A_REVERSE);
      win_render(win);
      wattroff(win, A_REVERSE);
    }
  } else {
    win_render(win);
  }
  wrefresh(win);
}

void
board_parse_options(struct BoardRepr *board) {
  for (int i=0; i<BOARD_SIZE; ++i) {
    for (int j=0; j<BOARD_SIZE; ++j) {
      SquareRepr_parse_options(&board->squares[i][j], board);
    }
  }
}

void
board_init(struct BoardRepr *board) {
  int startx = (COLS  - (BOARD_SIZE*SQUARE_SIZEX)) / 2;
  int starty = (LINES - (BOARD_SIZE*SQUARE_SIZEY)) / 2;

  board->white_castle   = TRUE;
  board->black_castle   = TRUE;
  board->white_castle_q = TRUE;
  board->black_castle_q = TRUE;
  for (int i=0; i<BOARD_SIZE; ++i) {
    for (int j=0; j<BOARD_SIZE; ++j) {
      board->squares[i][j].win = newwin(SQUARE_SIZEY, SQUARE_SIZEX, starty + (i*SQUARE_SIZEY) - i,
                                    startx + (j*SQUARE_SIZEX) - j);
      SquareRepr_init(&board->squares[i][j], i, j);
      win_render_piece(&board->squares[i][j]);
      wrefresh(board->squares[i][j].win);
    }
  }
  board_parse_options(board);
  win_activate(board->squares[0][0].win, TRUE);
}

void
board_render(struct BoardRepr *board) {
  for (int i=0; i<BOARD_SIZE; ++i) {
    for (int j=0; j<BOARD_SIZE; ++j) {
      win_render_piece(&board->squares[i][j]);
    }
  }
}

bool
make_turn(struct BoardRepr *board) {
  bool valid = FALSE;
  if (!FOCUSED_SQUARE) return 1;
  for (int i=0; i<FOCUSED_SQUARE->opts.opt_len; ++i) {
    if (cord_y == FOCUSED_SQUARE->opts.opt[i].y
        && cord_x == FOCUSED_SQUARE->opts.opt[i].x)
      valid = TRUE;
  }
  if (valid == FALSE) return 1;
#ifdef DEBUG
  printw("T - ");
#endif
  SquareRepr_set(&board->squares[cord_y][cord_x], FOCUSED_SQUARE->piece.type, TRUE,
                 FOCUSED_SQUARE->piece.side);
  SquareRepr_set(FOCUSED_SQUARE, PIECE_NONE, FALSE, SIDE_NONE);
  if (board->squares[cord_y][cord_x].piece.has_moved == FALSE) {
    board->squares[cord_y][cord_x].piece.has_moved = TRUE;
  }

  win_render_piece(&board->squares[cord_y][cord_x]);
  win_clear(FOCUSED_SQUARE->win);
  wrefresh(board->squares[cord_y][cord_x].win);
  wrefresh(FOCUSED_SQUARE->win);

  return 0;
}

void SquareRepr_info(struct SquareRepr *square) {
  char *type, *side;
  switch(square->piece.type) {
    case PIECE_PAWN:
      type = "pawn";
      break;
    case PIECE_KNIGHT:
      type = "knight";
      break;
    case PIECE_BISHOP:
      type = "bishop";
      break;
    case PIECE_ROOK:
      type = "rook";
      break;
    case PIECE_QUEEN:
      type = "queen";
      break;
    case PIECE_KING:
      type = "king";
      break;
    default:
      type = "empty";
      break;
  }
  if (square->piece.side == SIDE_NONE) {
    side = "none";
  } else if (square->piece.side == SIDE_WHITE) {
    side = "white";
  } else if (square->piece.side == SIDE_BLACK) {
    side = "black";
  }

  mvprintw(LINES-2, 1, "{side: %s; piece: %s; x: %d; y: %d}", side, type, square->x, square->y);
}

void
game_loop(struct BoardRepr *board) {
  int ch;

  for (;;) {
    ch = getch();
    switch (ch) {
      case KEY_UP:
      case 'k':
        if (cord_y)
          --cord_y;
        break;

      case KEY_DOWN:
      case 'j':
        if (cord_y < BOARD_SIZE-1)
          ++cord_y;
        break;

      case KEY_LEFT:
      case 'h':
        if (cord_x)
          --cord_x;
        break;

      case KEY_RIGHT:
      case 'l':
        if (cord_x < BOARD_SIZE-1)
          ++cord_x;
        break;
      case KEY_HOME:
        SquareRepr_info(&board->squares[cord_y][cord_x]);
        break;
      case 's':
        if (FOCUSED_SQUARE)
          FOCUSED_SQUARE_free();
        if (board->squares[cord_y][cord_x].opts.opt_len > 0)
          FOCUSED_SQUARE_init(&board->squares[cord_y][cord_x]);
        break;
      case KEY_ENTER:
      case 't':
        if (make_turn(board)) break;
        FOCUSED_SQUARE_free();
        for (int i=0; i<BOARD_SIZE; ++i) {
          for (int j=0; j<BOARD_SIZE; ++j) {
            TurnOptions_free(&board->squares[i][j].opts);
            TurnOptions_init(&board->squares[i][j].opts);
            SquareRepr_parse_options(&board->squares[i][j], board);
          }
        }
        break;
      case 'q':
        FOCUSED_SQUARE_free();
        break;
    }
    win_activate(board->squares[prev_cord_y][prev_cord_x].win, FALSE);
    win_activate(board->squares[cord_y][cord_x].win, TRUE);
    prev_cord_x = cord_x;
    prev_cord_y = cord_y;
    if (FOCUSED_SQUARE) {
      TurnOptions_render(&FOCUSED_SQUARE->opts);
    }
  }
}

void
newgame() {
  struct BoardRepr board;
  board_init(&board);
  game_loop(&board);
}

int
main(int argc, char **argv) {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  refresh();
  if (has_colors() == TRUE) {
    start_color();
    HAS_COLORS = has_colors();
    init_pair(1, COLOR_BLACK, COLOR_CYAN); /* Highlighted square color */
    init_pair(2, COLOR_BLACK, COLOR_GREEN); /* Available turns for selected square */
  }

  newgame();

  endwin();
  return 0;
}
