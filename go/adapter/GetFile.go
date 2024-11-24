package main

import (
	"github.com/knusbaum/go9p/fs"
	"github.com/knusbaum/go9p/proto"
	"github.com/pkg/errors"
	"github.com/vmihailenco/msgpack"
)

type GetFile[T any] struct {
	*fs.BaseFile
	buffers  map[uint64]*Buffer
	function func() (T, error)
}

func NewGetFile[T any](s *proto.Stat, function func() (T, error)) *GetFile[T] {
	return &GetFile[T]{
		BaseFile: fs.NewBaseFile(s),
		buffers:  make(map[uint64]*Buffer),
		function: function,
	}
}

func (f *GetFile[T]) Open(fid uint64, omode proto.Mode) error {
	value, err := f.function()
	if err != nil {
		return errors.Wrap(err, "error in function execution")
	}

	data, err := msgpack.Marshal(value)
	if err != nil {
		return errors.Wrap(err, "could not encode value")
	}

	buf := &Buffer{Data: data}
	f.buffers[fid] = buf
	return nil
}

func (f *GetFile[T]) Read(fid, offset, count uint64) ([]byte, error) {
	return f.buffers[fid].Read9(offset, count), nil
}

func (f *GetFile[T]) Close(fid uint64) error {
	delete(f.buffers, fid)
	return nil
}
