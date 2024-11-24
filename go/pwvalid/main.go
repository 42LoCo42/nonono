package main

import (
	"bufio"
	"fmt"
	"os"

	"github.com/nbutton23/zxcvbn-go"
)

func main() {
	const minEntropyBits = 60
	reader := bufio.NewReader(os.Stdin)

	for {
		fmt.Print("Password: ")

		line, _, err := reader.ReadLine()
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			continue
		}

		password := string(line)
		strength := zxcvbn.PasswordStrength(password, nil)

		fmt.Println("Entropy:   ", strength.Entropy)
		fmt.Println("Crack time:", strength.CrackTimeDisplay)
		fmt.Printf("Score:      %d/4\n", strength.Score)
	}
}
