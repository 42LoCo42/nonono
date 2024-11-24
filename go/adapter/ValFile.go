package main

import (
	"log"

	"github.com/knusbaum/go9p/fs"
	"github.com/knusbaum/go9p/proto"
	"github.com/pkg/errors"
	"github.com/vmihailenco/msgpack"
)

type ValFile[T any] struct {
	*fs.StaticFile
	Value T
}

func NewValFile[T any](s *proto.Stat, value T) *ValFile[T] {
	data, err := msgpack.Marshal(value)
	if err != nil {
		log.Fatal(errors.Wrap(err, "could not encode value"))
	}

	return &ValFile[T]{
		StaticFile: fs.NewStaticFile(s, data),
		Value:      value,
	}
}

func (f *ValFile[T]) Close(fid uint64) error {
	if err := msgpack.Unmarshal(f.Data, &f.Value); err != nil {
		return errors.Wrap(err, "could not decode data")
	}

	return nil
}
