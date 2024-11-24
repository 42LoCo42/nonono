BEGIN {
	numbers[0] = 0
}

function inc(pos) {
	numbers[pos]++;
	for(i = pos + 1; i < length(numbers); i++) {
		numbers[i] = 0
	}
}

match($0, /^(#+) /, m) {
	pos = length(m[1]) - 1
	inc(pos)
	$1 = $1 " "
	for(i = 0; i <= pos; i++) {
		$1 = $1 numbers[i] "."
	}
}

{print}
