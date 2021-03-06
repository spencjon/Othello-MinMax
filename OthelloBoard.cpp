/*
 * OthelloBoard.cpp
 *
 *  Created on: Apr 18, 2015
 *      Author: wong
 */
#include <vector>
#include <assert.h>
#include "OthelloBoard.h"
#include <tuple>
#include <utility> //make_pair()
#include <iostream>



using std::tuple;
using std::vector;

OthelloBoard::OthelloBoard(int cols, int rows, char p1_symb, char p2_symb) :
		Board(cols, rows), p1_symbol(p1_symb), p2_symbol(p2_symb) {

}

OthelloBoard::OthelloBoard(const OthelloBoard& other) :
		Board(other), p1_symbol(other.p1_symbol), p2_symbol(other.p2_symbol) {

}

OthelloBoard::~OthelloBoard() {

}

void OthelloBoard::initialize() {
	set_cell(num_cols / 2 - 1, num_rows / 2 - 1, p1_symbol);
	set_cell(num_cols / 2, num_rows / 2, p1_symbol);
	set_cell(num_cols / 2 - 1, num_rows / 2, p2_symbol);
	set_cell(num_cols / 2, num_rows / 2 - 1, p2_symbol);


}

OthelloBoard& OthelloBoard::operator=(const OthelloBoard& rhs) {
	Board::operator=(rhs);
	p1_symbol = rhs.p1_symbol;
	p2_symbol = rhs.p2_symbol;
	return *this;
}

void OthelloBoard::set_coords_in_direction(int col, int row, int& next_col,
		int& next_row, int dir) const {
	switch (dir) {
	case N:
		next_col = col;
		next_row = row + 1;
		break;
	case NE:
		next_col = col + 1;
		next_row = row + 1;
		break;
	case E:
		next_col = col + 1;
		next_row = row;
		break;
	case SE:
		next_col = col + 1;
		next_row = row - 1;
		break;
	case S:
		next_col = col;
		next_row = row - 1;
		break;
	case SW:
		next_col = col - 1;
		next_row = row - 1;
		break;
	case W:
		next_col = col - 1;
		next_row = row;
		break;
	case NW:
		next_col = col - 1;
		next_row = row + 1;
		break;
	default:
		assert("Invalid direction");
	}
}

bool OthelloBoard::check_endpoint(int col, int row, char symbol, int dir,
		bool match_symbol) const {
	int next_row = -1;
	int next_col = -1;

	if (!is_in_bounds(col, row) || is_cell_empty(col, row)) {
		return false;
	} else {
		if (match_symbol) {
			if (get_cell(col, row) == symbol) {
				return true;
			} else {
				set_coords_in_direction(col, row, next_col, next_row, dir);
				return check_endpoint(next_col, next_row, symbol, dir,
						match_symbol);
			}
		} else {
			if (get_cell(col, row) == symbol) {
				return false;
			} else {
				set_coords_in_direction(col, row, next_col, next_row, dir);
				return check_endpoint(next_col, next_row, symbol, dir,
						!match_symbol);
			}
		}
	}
}

bool OthelloBoard::is_legal_move(int col, int row, char symbol) const {
	bool result = false;
	int next_row = -1;
	int next_col = -1;
	if (!is_in_bounds(col, row) || !is_cell_empty(col, row)) {
		return result;
	}
	for (int d = N; d <= NW; d++) {
		set_coords_in_direction(col, row, next_col, next_row, d);
		if (check_endpoint(next_col, next_row, symbol, d, false)) {
			result = true;
			break;
		}
	}
	return result;
}

int OthelloBoard::flip_pieces_helper(int col, int row, char symbol, int dir) {
	int next_row = -1;
	int next_col = -1;

	if (get_cell(col, row) == symbol) {
		return 0;
	} else {
		set_cell(col, row, symbol);
		set_coords_in_direction(col, row, next_col, next_row, dir);
		return 1 + flip_pieces_helper(next_col, next_row, symbol, dir);
	}
}

int OthelloBoard::flip_pieces(int col, int row, char symbol) {
	int pieces_flipped = 0;
	int next_row = -1;
	int next_col = -1;

	assert(is_in_bounds(col, row));
	for (int d = N; d <= NW; d++) {
		set_coords_in_direction(col, row, next_col, next_row, d);
		if (check_endpoint(next_col, next_row, symbol, d, false)) {
			pieces_flipped += flip_pieces_helper(next_col, next_row, symbol, d);
		}
	}
	return pieces_flipped;
}

bool OthelloBoard::has_legal_moves_remaining(char symbol) const {
	for (int c = 0; c < num_cols; c++) {
		for (int r = 0; r < num_rows; r++) {
			if (is_cell_empty(c, r) && is_legal_move(c, r, symbol)) {
				return true;
			}
		}
	}
	return false;
}

int OthelloBoard::count_score(char symbol) const {
	int score = 0;
	for (int c = 0; c < num_cols; c++) {
		for (int r = 0; r < num_rows; r++) {
			if (grid[c][r] == symbol) {
				score++;
			}
		}
	}
	return score;
}

void OthelloBoard::play_move(int col, int row, char symbol) {
	//std::cout << "\nCol " << col << " row " << row << std::endl;
	set_cell(col, row, symbol);
	flip_pieces(col, row, symbol);
}

//Returns the number of possible moves
//Should be paralelized if we have a larger board ()like 8x8 or so
int OthelloBoard::get_num_possible_moves(char color) {
	int num_moves = 0;
	for (int col = 0; col < this->num_rows; col++)
		for (int row = 0; row < this->num_rows; row++) 
			if (this->is_legal_move(col, row, color))
				num_moves++;
	
	return num_moves;
}
//Returns a vector of int touples (col, row) of valid moves
//Should be paralelized if we have a larger board (like 8x8 or so)
vector<tuple<int, int>> OthelloBoard::get_possible_moves(char color) {
	vector<tuple<int, int>> possible_moves;
	for (int col = 0; col < this->get_num_cols(); col++)
		for (int row = 0; row < this->get_num_rows(); row++)
			if (this->is_legal_move(col, row, color)){
				//std::cout << "get possible moves Possible mve: Col " << col << " row " << row << std::endl;
				tuple<int, int> tempTuple (std::make_pair(col, row));
				possible_moves.push_back(tempTuple);
			}
	return possible_moves;
}	