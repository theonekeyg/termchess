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

int
wrapper_add_options(struct SquareRepr *self, struct SquareRepr *other) {
  if (other->isoccu == TRUE) {
    if (other->piece.side ^ self->piece.side) {
      TurnOptions_addnew(&self->opts, other);
    }
    return 1;
  }
  TurnOptions_addnew(&self->opts, other);
  return 0;
}

void
TurnOptions_parse_pawn(struct BoardRepr *board,
                       struct SquareRepr *square) {
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
    /* Check taking the opponents piece */
    if (square->x-1 >= 0) {
      if (board->squares[square->y+1][square->x-1].isoccu == TRUE &&
          board->squares[square->y+1][square->x-1].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[square->y+1][square->x-1]);
      }
    }
    if (square->x+1 < BOARD_SIZE) {
      if (board->squares[square->y+1][square->x+1].isoccu == TRUE &&
          board->squares[square->y+1][square->x+1].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[square->y+1][square->x+1]);
      }
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
    if (square->x-1 >= 0) {
      if (board->squares[square->y-1][square->x-1].isoccu == TRUE &&
          board->squares[square->y-1][square->x-1].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[square->y-1][square->x-1]);
      }
    }
    if (square->x+1 < BOARD_SIZE) {
      if (board->squares[square->y-1][square->x+1].isoccu == TRUE &&
          board->squares[square->y-1][square->x+1].piece.side ^ square->piece.side) {
        TurnOptions_addnew(&square->opts, &board->squares[square->y-1][square->x+1]);
      }
    }
  }
}

void
TurnOptions_parse_rook(struct BoardRepr *board,
                       struct SquareRepr *square) {
  int x, y;
  y = square->y;
  for (x=square->x+1; x < BOARD_SIZE; ++x) {
    if (wrapper_add_options(square, &board->squares[y][x]))
      break;
  }
  for (x=square->x-1; x >= 0; --x) {
    if (wrapper_add_options(square, &board->squares[y][x]))
      break;
  }
  x = square->x;
  for (y=square->y+1; y < BOARD_SIZE; ++y) {
    if (wrapper_add_options(square, &board->squares[y][x]))
      break;
  }
  for (y=square->y-1; y >= 0; --y) {
    if (wrapper_add_options(square, &board->squares[y][x]))
      break;
  }
}

void
TurnOptions_parse_diagonal(struct BoardRepr *board,
                           struct SquareRepr *square) {
  int x, y;
  for (x=square->x+1, y=square->y+1; (x < BOARD_SIZE) && (y < BOARD_SIZE); ++x, ++y) {
    if (wrapper_add_options(square, &board->squares[y][x]))
      break;
  }
  for (x=square->x-1, y=square->y+1; (x >= 0) && (y < BOARD_SIZE); --x, ++y) {
    if (wrapper_add_options(square, &board->squares[y][x]))
      break;
  }
  for (x=square->x+1, y=square->y-1; (x < BOARD_SIZE) && (y >= 0); ++x, --y) {
    if (wrapper_add_options(square, &board->squares[y][x]))
      break;
  }
  for (x=square->x-1, y=square->y-1; (x >= 0) && (y >= 0); --x, --y) {
    if (wrapper_add_options(square, &board->squares[y][x]))
      break;
  }
}

void
TurnOptions_parse_knight(struct BoardRepr *board,
                         struct SquareRepr *square) {
  int x, y;
  x = square->x+2;
  if (x < BOARD_SIZE) {
    y = square->y-1;
    if (y >= 0)
      wrapper_add_options(square, &board->squares[y][x]);

    y = square->y+1;
    if (y < BOARD_SIZE)
      wrapper_add_options(square, &board->squares[y][x]);
  }
  x = square->x-2;
  if (x >= 0) {
    y = square->y-1;
    if (y >= 0)
      wrapper_add_options(square, &board->squares[y][x]);

    y = square->y+1;
    if (y < BOARD_SIZE)
      wrapper_add_options(square, &board->squares[y][x]);
  }
  y = square->y+2;
  if (y < BOARD_SIZE) {
    x = square->x-1;
    if (x >= 0)
      wrapper_add_options(square, &board->squares[y][x]);

    x = square->x+1;
    if (x < BOARD_SIZE)
      wrapper_add_options(square, &board->squares[y][x]);
  }
  y = square->y-2;
  if (y >= 0) {
    x = square->x-1;
    if (x >= 0)
      wrapper_add_options(square, &board->squares[y][x]);

    x = square->x+1;
    if (x < BOARD_SIZE)
      wrapper_add_options(square, &board->squares[y][x]);
  }
}

void 
TurnOptions_parse_king(struct BoardRepr *board,
                       struct SquareRepr *square) {
  int x, y;
  y = square->y;
  /* Castling rights */
  if (square->piece.has_moved == FALSE) {
    for (x=square->x-1; x > 0; --x) {
      if (board->squares[y][x].isoccu == TRUE)
        break;
    }
    if (x == 0) {
      if (board->squares[y][x].piece.has_moved == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[y][square->x-2]);
    }

    for (x=square->x+1; x < BOARD_SIZE-1; ++x) {
      if (board->squares[y][x].isoccu == TRUE)
        break;
    }
    if (x == BOARD_SIZE-1) {
      if (board->squares[y][x].piece.has_moved == FALSE)
        TurnOptions_addnew(&square->opts, &board->squares[y][square->x+2]);
    }
  }

  y = square->y;
  if ((x = square->x-1) >= 0) {
    wrapper_add_options(square, &board->squares[y][x]);
    if ((y = square->y-1) >= 0)
      wrapper_add_options(square, &board->squares[y][x]);
    if ((y = square->y+1) < BOARD_SIZE)
      wrapper_add_options(square, &board->squares[y][x]);
  }

  y = square->y;
  if ((x = square->x+1) < BOARD_SIZE) {
    wrapper_add_options(square, &board->squares[y][x]);
    if ((y = square->y-1) >= 0)
      wrapper_add_options(square, &board->squares[y][x]);
    if ((y = square->y+1) < BOARD_SIZE)
      wrapper_add_options(square, &board->squares[y][x]);
  }

  x = square->x;
  if ((y = square->y-1) >= 0)
    wrapper_add_options(square, &board->squares[y][x]);
  if ((y = square->y+1) < BOARD_SIZE)
    wrapper_add_options(square, &board->squares[y][x]);
}
