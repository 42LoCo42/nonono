package main

import (
	"flag"
	"log"

	"github.com/jamesruan/sodium"
)

type Nonce struct {
	sodium.Bytes
}

func (n *Nonce) Size() int {
	return 32
}

func (n *Nonce) Length() int {
	return len(n.Bytes)
}

func main() {
	mode := flag.String("run", "server", "what action to run")
	flag.Parse()

	if err := map[string]func() error{
		"server": server,
		"client": client,
	}[*mode](); err != nil {
		log.Fatal(err)
	}
}
