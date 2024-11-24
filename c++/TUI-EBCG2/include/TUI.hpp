#pragma once
#include <ncurses.h>
#include <CursedCat.hpp>
#include <vector>

#include "defs.hpp"

int main(int argc, char* argv[]);
void startTUI();
void printGame();
int handleMenu(const CursedCat::LineDataSet& lds);

void printBoard();
