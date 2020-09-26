#ifndef _TERMCHESS_H_
#define _TERMCHESS_H_

#define DEBUG
#define SQUARE_SIZEX 11
#define SQUARE_SIZEY 5
#define BOARD_SIZE   8

#define SAME_SIDE(x, y) ((#x) & (#y))

struct TurnOptions {
  struct SquareRepr *opt;
  int opt_len;
};

struct PieceRepr {
  int type;
  int side;
  bool has_moved;
};

struct SquareRepr {
  WINDOW *win;
  struct TurnOptions opts;

  struct PieceRepr piece;
  bool isoccu;
  int y;
  int x;
};

struct BoardRepr {
  struct SquareRepr squares[BOARD_SIZE][BOARD_SIZE];
  bool white_castle;
  bool black_castle;
  bool white_castle_q;
  bool black_castle_q;
};

void win_render(WINDOW *);
void win_render_piece(struct SquareRepr *);
void PieceRepr_init(struct PieceRepr *);
void TurnOptions_init(struct TurnOptions *);
void TurnOptions_free(struct TurnOptions *);
void TurnOptions_render(struct TurnOptions *);

#endif // _TERMCHESS_H_
