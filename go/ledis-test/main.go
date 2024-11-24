package main

import (
	"log"

	lediscfg "github.com/ledisdb/ledisdb/config"
	"github.com/ledisdb/ledisdb/ledis"
	"github.com/pkg/errors"
)

func main() {
	if err := run(); err != nil {
		log.Fatal(err)
	}
}

func run() error {
	cfg := lediscfg.NewConfigDefault()

	l, err := ledis.Open(cfg)
	if err != nil {
		return errors.Wrap(err, "could not open Ledis")
	}
	defer l.Close()

	db, err := l.Select(0)
	if err != nil {
		return errors.Wrap(err, "could not select Ledis DB 0")
	}

	db.Set([]byte("foobar"), []byte("data"))
	log.Print(db.Get([]byte("foobar")))

	log.Print(db.Exists([]byte("foobar")))

	return nil
}
