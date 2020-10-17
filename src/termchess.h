#ifndef _H_TERMCHESS
#define _H_TERMCHESS

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <ncurses.h>

/* #define DEBUG */
#define SQUARE_SIZEX 11
#define SQUARE_SIZEY 5
#define BOARD_SIZE   8

static bool HAS_COLORS = FALSE;
/* Coordinates of current square */
static int cord_x = 0;
static int cord_y = 0;
static int prev_cord_x = 0;
static int prev_cord_y = 0;
static const int square_midx = SQUARE_SIZEX / 2;
static const int square_midy = SQUARE_SIZEY / 2;
static struct BoardRepr board;
static struct SquareRepr *FOCUSED_SQUARE = NULL;

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

/* #define PIECE_NONE   0x00 */
/* #define PIECE_KING   0x01 */
/* #define PIECE_ROOK   0x02 */
/* #define PIECE_BISHOP 0x04 */
/* #define PIECE_QUEEN  (PIECE_ROOK | PIECE_BISHOP) */
/* #define PIECE_KNIGHT 0x08 */
/* #define PIECE_PAWN   0x10 */

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
/* TODO: Right now to print piece character, this code uses enum PIECES_ENUM to
 * index each piece character from arrays below, but that forces doing unncessary checks
 * when getting square's piece->type is required, for instance to check square for PIECE_BISHOP and
 * PIECE_QUEEN there's no other way but to do 2 ifs, when ANDing piece->type and PIECE_BISHOP
 * is possible and would be less boilerplate, but that impossible with current implementation
 * with enum. Maybe make different identifiers of types in PieceRepr structure?
 */ static const char *PIECES_WHITE[] = {
  "@", "Q", "R", "B", "N", "P"
};
static const char *PIECES_BLACK[] = {
  "#", "q", "r", "b", "n", "p"
};

struct TurnOptions {
  struct SquareRepr **opt;
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
  int TURN_SIDE;
  struct SquareRepr *king_w;
  struct SquareRepr *king_b;
  struct SquareRepr squares[BOARD_SIZE][BOARD_SIZE];
};

void win_render(WINDOW *);
void win_render_piece(struct SquareRepr *square);
void PieceRepr_init(struct PieceRepr *piece);
void SquareRepr_set(struct SquareRepr *square, int piece, bool isoccu, bool side);

#include "options.h"

#endif // _H_TERMCHESS
