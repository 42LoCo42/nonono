package main

import (
	"github.com/knusbaum/go9p/fs"
	"github.com/knusbaum/go9p/proto"
	"github.com/pkg/errors"
	"github.com/vmihailenco/msgpack"
)

type SetFile[T any] struct {
	*fs.BaseFile
	buffers  map[uint64]*Buffer
	function func(T) error
}

func NewSetFile[T any](s *proto.Stat, function func(T) error) *SetFile[T] {
	return &SetFile[T]{
		BaseFile: fs.NewBaseFile(s),
		buffers:  make(map[uint64]*Buffer),
		function: function,
	}
}

func (f *SetFile[T]) Open(fid uint64, omode proto.Mode) error {
	f.buffers[fid] = &Buffer{}
	return nil
}

func (f *SetFile[T]) Write(fid, offset uint64, data []byte) (uint32, error) {
	return f.buffers[fid].Write9(offset, data), nil
}

func (f *SetFile[T]) Close(fid uint64) error {
	var value T

	if err := msgpack.Unmarshal(f.buffers[fid].Data, &value); err != nil {
		return errors.Wrap(err, "could not decode data")
	}

	delete(f.buffers, fid)
	return errors.Wrap(f.function(value), "error in function execution")
}
