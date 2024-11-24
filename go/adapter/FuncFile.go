package main

import (
	"github.com/knusbaum/go9p/fs"
	"github.com/knusbaum/go9p/proto"
	"github.com/pkg/errors"
	"github.com/vmihailenco/msgpack"
)

type FuncFile[I, O any] struct {
	*fs.BaseFile
	iBuffers map[uint64]*Buffer
	oBuffers map[uint64]*Buffer
	function func(I) (O, error)
}

func NewFuncFile[I, O any](s *proto.Stat, f func(I) (O, error)) *FuncFile[I, O] {
	return &FuncFile[I, O]{
		BaseFile: fs.NewBaseFile(s),
		iBuffers: make(map[uint64]*Buffer),
		oBuffers: make(map[uint64]*Buffer),
		function: f,
	}
}

func (f *FuncFile[I, O]) Open(fid uint64, omode proto.Mode) error {
	f.iBuffers[fid] = &Buffer{}
	f.oBuffers[fid] = &Buffer{}
	return nil
}

func (f *FuncFile[I, O]) Read(fid, offset, count uint64) ([]byte, error) {
	if offset == 0 {
		// execute function
		var input I

		if err := msgpack.Unmarshal(f.iBuffers[fid].Data, &input); err != nil {
			return nil, errors.Wrap(err, "could not decode input")
		}

		value, err := f.function(input)
		if err != nil {
			return nil, errors.Wrap(err, "error in function execution")
		}

		data, err := msgpack.Marshal(value)
		if err != nil {
			return nil, errors.Wrap(err, "could not encode output")
		}

		f.oBuffers[fid].Data = data
	}

	return f.oBuffers[fid].Read9(offset, count), nil
}

func (f *FuncFile[I, O]) Write(fid, offset uint64, data []byte) (uint32, error) {
	return f.iBuffers[fid].Write9(offset, data), nil
}

func (f *FuncFile[I, O]) Close(fid uint64) error {
	delete(f.iBuffers, fid)
	delete(f.oBuffers, fid)
	return nil
}
