package main

import (
	"io"
	"log"
	"os"

	"github.com/pkg/errors"
)

func main() {
	if len(os.Args) < 2 {
		log.Fatal("Usage: call <file>")
	}

	// open file
	file, err := os.OpenFile(
		os.Args[1],
		os.O_RDWR,
		0,
	)
	if err != nil {
		log.Fatal(errors.Wrap(err, "could not open file"))
	}
	defer file.Close()

	// write input
	if _, err := io.Copy(file, os.Stdin); err != nil {
		log.Fatal(errors.Wrap(err, "could not write input"))
	}

	// seek to 0 before read to signal execution
	// since read fails on other offsets
	if _, err := file.Seek(0, 0); err != nil {
		log.Fatal(errors.Wrap(err, "could not seek to 0"))
	}

	// read output
	if _, err := io.Copy(os.Stdout, file); err != nil {
		log.Fatal(errors.Wrap(err, "could not read output"))
	}
}
