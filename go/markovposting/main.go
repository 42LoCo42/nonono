package main

import (
	"fmt"
	"log"
	"math/rand"
	"os"
	"strconv"
	"strings"
)

func main() {
	rawData, err := os.ReadFile("/dev/stdin")
	if err != nil {
		log.Fatal(err)
	}

	words := strings.Split(string(rawData), " ")
	table := map[string][]string{}

	addToTable := func(windowSize int) {
		for pos := range words {
			if pos+windowSize > len(words) {
				break
			}
			here := words[pos : pos+windowSize]
			prefix := strings.Join(here[:windowSize-1], " ")
			suffix := here[windowSize-1]
			table[prefix] = append(table[prefix], suffix)
		}
	}

	for i := 1; i <= 10; i++ {
		addToTable(i)
	}

	current := os.Args[1]
	outputSize, _ := strconv.Atoi(os.Args[2])
	fmt.Print(current, " ")

	for {
		candidates := table[current]
		if len(candidates) == 0 {
			break
		}
		selected := candidates[rand.Intn(len(candidates))]
		next := strings.Split(current+" "+selected, " ")
		if len(next) >= outputSize {
			current = strings.Join(next[1:outputSize], " ")
		} else {
			current = strings.Join(next, " ")
		}
		fmt.Print(selected, " ")
	}
}
