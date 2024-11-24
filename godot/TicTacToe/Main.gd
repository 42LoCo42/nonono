extends ColorRect

const size = 500
const offset = size / 3

enum Player {
	RED,
	BLUE,
	NONE,
}

var currentPlayer = Player.RED
var board = [
	[Player.NONE, Player.NONE, Player.NONE],
	[Player.NONE, Player.NONE, Player.NONE],
	[Player.NONE, Player.NONE, Player.NONE],
]

func _on_Node2D_gui_input(event: InputEvent):
	if event is InputEventMouseButton and event.is_pressed():
		# get clicked tile
		var x = floor(event.position.x / offset)
		var y = floor(event.position.y / offset)
		board[x][y] = currentPlayer

		update()

		# check rows
		for row in board:
			var win = true
			for cell in row:
				win = win && (row[0] != Player.NONE) && (cell == row[0])

			if(win == true):
				win()
				return

		# check rows
		for col in range(len(board[0])):
			var start = board[0][col]
			var win = true
			for row in range(len(board)):
				var cell = board[row][col]
				win = win && (start != Player.NONE) && (cell == start)

		# check main diag
		var start = board[0][0]
		var win = true
		for c in range(len(board)):
			var cell = board[c][c]
			win = win && (start != Player.NONE) && (start == cell)

		if(win == true):
			win()
			return
			
		# check other diag
		start = board[0][2]
		win = true
		for c in range(len(board)):
			var cell = board[c][2 - c]
			win = win && (start != Player.NONE) && (start == cell)

		if(win == true):
			win()
			return

		# switch to next player
		currentPlayer = Player.BLUE - currentPlayer

func win():
	print("win %d!" % currentPlayer)

func _draw():
	for row in range(len(board)):
		for col in range(len(board[row])):
			var cell = board[row][col]
			if(cell == Player.RED):
				draw_line(
					Vector2(
						floor(offset * (row + 0.5)) - 25,
						floor(offset * (col + 0.5)) - 25
					),
					Vector2(
						floor(offset * (row + 0.5)) + 25,
						floor(offset * (col + 0.5)) + 25
					),
					Color.red,
					3.0
				)
				draw_line(
					Vector2(
						floor(offset * (row + 0.5)) - 25,
						floor(offset * (col + 0.5)) + 25
					),
					Vector2(
						floor(offset * (row + 0.5)) + 25,
						floor(offset * (col + 0.5)) - 25
					),
					Color.red,
					3.0
				)
			elif(cell == Player.BLUE):
				draw_arc(
					Vector2(
						floor(offset * (row + 0.5)),
						floor(offset * (col + 0.5))
					),
					25.0,
					0.0,
					2 * PI,
					100,
					Color.blue,
					3.0,
					true
				)
