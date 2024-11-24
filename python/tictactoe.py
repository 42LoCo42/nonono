#!/usr/bin/env python3
from time import sleep
from random import randint as ri

board = [[0, 0, 0],
         [0, 0, 0],
         [0, 0, 0]]

win_player = False
win_computer = False


def check_win_me(board, player):
    if board[0][0] == 1 and board[1][0] == 1 and board[2][0] == 1:
        win_player = True
        return win_player

    elif board[0][1] == 1 and board[1][1] == 1 and board[2][1] == 1:
        win_player = True
        return win_player

    elif board[0][2] == 1 and board[1][2] == 1 and board[2][2] == 1:
        win_player = True
        return win_player

    elif board[0][0] == 1 and board[1][1] == 1 and board[2][2] == 1:
        win_player = True
        return win_player

    elif board[0][2] == 1 and board[1][1] == 1 and board[2][0] == 1:
        win_player = True
        return win_player


    for row in board:
        if row[0] == 1 and row[1] == 1 and row[2] == 1:
            win_player = True
            return win_player


def check_win_comp(board, player):
    if board[0][0] == 9 and board[1][0] == 9 and board[2][0] == 9:
        win_computer = True
        return win_computer
    elif board[0][1] == 9 and board[1][1] == 9 and board[2][1] == 9:
        win_computer = True
        return win_computer
    elif board[0][2] == 9 and board[1][2] == 9 and board[2][2] == 9:
        win_computer = True
        return win_computer

    elif board[0][0] == 9 and board[1][1] == 9 and board[2][2] == 9:
        win_computer = True
        return win_computer

    elif board[0][2] == 9 and board[1][1] == 9 and board[2][0] == 9:
        win_computer = True
        return win_computer

    for row in board:
        if row[0] == 9 and row[1] == 9 and row[2] == 9:
            win_computer = True
            return win_computer

while not win_player and not win_computer:
    print("Wo möchtest du dein Stein platzieren?(Reihe, Spalte)")
    eingabe = False
    while not eingabe:
        reihe = int(input("Reihe?"))
        spalte = int(input("Spalte"))
        if board[reihe][spalte] == 0:
            eingabe = True
        else:
            print("Feld schon besetzt")
        board[reihe][spalte] = 1

    for row in board:
        print(row)

    if check_win_me(board, win_player) == True:
        win_player = True
        break



    comp = False
    while not comp:
        random_reihe = ri(0, 2)
        random_spalte = ri(0, 2)

        if board[random_reihe][random_spalte] == 0:
            print("\n COMPUTER AM ZUG")
            sleep(1)
            board[random_reihe][random_spalte] = 9
            comp = True

    print()
    for row in board:
        print(row)
    print()


    if check_win_comp(board, win_computer):
        win_computer = True
        break


if check_win_me(board, win_player):
    print("\nDu hast gewonnen!")

elif check_win_comp(board, win_computer):
    print("\nDu hast verloren!")

