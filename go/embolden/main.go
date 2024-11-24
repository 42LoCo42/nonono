package main

import (
	"bytes"
	"fmt"
	"io"
	"math"
	"os"
	"regexp"
)

var (
	bold  = []byte("[1;38;2;0;0;0m")
	reset = []byte("[22;38;2;40;40;40m")
)

func main() {

	buf := new(bytes.Buffer)
	if _, err := io.Copy(buf, os.Stdin); err != nil {
		panic(err)
	}
	text := buf.Bytes()

	re := regexp.MustCompile("\\w+")
	fmt.Print("[48;2;255;255;238m")
	os.Stdout.Write(re.ReplaceAllFunc(text, func(b []byte) []byte {
		cut := int(math.Ceil(float64(len(b)) / 2))
		prefix := b[:cut]
		suffix := b[cut:]

		return append(append(append(bold, prefix...), reset...), suffix...)
	}))
}
