#include "options.h"

void
TurnOptions_init(struct TurnOptions *opts) {
  opts->opt = (struct SquareRepr **) malloc(0);
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
TurnOptions_render(struct TurnOptions *options) {
#ifdef DEBUG
  printw("%d - ", options->opt_len);
#endif
  for (int i=0; i<options->opt_len; ++i) {
    if (options->opt[i]) {
      win_render(options->opt[i]->win);
      wrefresh(options->opt[i]->win);
    }
  }
}

void
TurnOptions_addnew(struct TurnOptions *opts, struct SquareRepr *square) {
  if (square) {
    opts->opt = realloc(opts->opt, sizeof(struct SquareRepr) * ++opts->opt_len);
    opts->opt[opts->opt_len-1] = square;
  }
}

void
TurnOptions_parse_rook(struct BoardRepr *board, struct SquareRepr *square) {
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
TurnOptions_parse_diagonal(struct BoardRepr *board, struct SquareRepr *square) {
  int x, y;
  for (x=square->x+1, y=square->y+1; (x < BOARD_SIZE) && (y < BOARD_SIZE); ++x, ++y) {
    if (board->squares[y][x].isoccu == TRUE) {
      if (board->squares[y][x].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      }
      break;
    }
    TurnOptions_addnew(&square->opts, &board->squares[y][x]);
  }
  for (x=square->x-1, y=square->y+1; (x >= 0) && (y < BOARD_SIZE); --x, ++y) {
    if (board->squares[y][x].isoccu == TRUE) {
      if (board->squares[y][x].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      }
      break;
    }
    TurnOptions_addnew(&square->opts, &board->squares[y][x]);
  }
  for (x=square->x+1, y=square->y-1; (x < BOARD_SIZE) && (y >= 0); ++x, --y) {
    if (board->squares[y][x].isoccu == TRUE) {
      if (board->squares[y][x].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[y][x]);
      }
      break;
    }
    TurnOptions_addnew(&square->opts, &board->squares[y][x]);
  }
  for (x=square->x-1, y=square->y-1; (x >= 0) && (y >= 0); --x, --y) {
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
TurnOptions_parse_knight(struct BoardRepr *board, struct SquareRepr *square) {
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
TurnOptions_parse_king(struct BoardRepr *board, struct SquareRepr *square) {
  /* TODO: add castling rights */
  if (square->x-1 >= 0) { /* Check for turning down */
    if (board->squares[square->y][square->x-1].isoccu == FALSE) /* Straight down */
      TurnOptions_addnew(&square->opts, &board->squares[square->y][square->x-1]);
    else if (board->squares[square->y][square->x-1].piece.side ^ square->piece.side)
      TurnOptions_addnew(&square->opts, &board->squares[square->y][square->x-1]);
    if (square->y-1 >= 0) {
      if (board->squares[square->y-1][square->x-1].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[square->y-1][square->x-1]);
      else if (board->squares[square->y-1][square->x-1].piece.side ^ square->piece.side)
        TurnOptions_addnew(&square->opts, &board->squares[square->y-1][square->x-1]);
    }
    if (square->y+1 < BOARD_SIZE) {
      if (board->squares[square->y+1][square->x-1].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[square->y+1][square->x-1]);
      else if (board->squares[square->y+1][square->x-1].piece.side ^ square->piece.side)
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
      else if (board->squares[square->y-1][square->x+1].piece.side ^ square->piece.side)
        TurnOptions_addnew(&square->opts, &board->squares[square->y-1][square->x+1]);
    }
    if (square->y+1 < BOARD_SIZE) {
      if (board->squares[square->y+1][square->x+1].isoccu == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[square->y+1][square->x+1]);
      else if (board->squares[square->y+1][square->x+1].piece.side ^ square->piece.side)
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
