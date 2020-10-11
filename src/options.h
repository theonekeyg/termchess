#ifndef _H_OPTIONS
#define _H_OPTIONS

#include "termchess.h"

void TurnOptions_init(struct TurnOptions *opts);
void TurnOptions_free(struct TurnOptions *opts);
void TurnOptions_render(struct TurnOptions *options);
void TurnOptions_addnew(struct TurnOptions *opts, struct SquareRepr *square);

void TurnOptions_parse_rook(struct BoardRepr *board, struct SquareRepr *square);
void TurnOptions_parse_diagonal(struct BoardRepr *board, struct SquareRepr *square);
void TurnOptions_parse_knight(struct BoardRepr *board, struct SquareRepr *square);
void TurnOptions_parse_king(struct BoardRepr *board, struct SquareRepr *square);

#endif // _H_OPTIONS
