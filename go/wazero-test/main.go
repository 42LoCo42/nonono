package main

import (
	"context"
	_ "embed"
	"log"
	"os"
	"wazero-test/shared"

	"github.com/pkg/errors"
	"github.com/tetratelabs/wazero"
	wazapi "github.com/tetratelabs/wazero/api"
	wasi "github.com/tetratelabs/wazero/imports/wasi_snapshot_preview1"
	"github.com/tinylib/msgp/msgp"
)

//go:embed wasm/test.wasm
var code []byte

type WASM struct {
	Ctx    context.Context
	Run    wazero.Runtime
	Cfg    wazero.ModuleConfig
	Mod    wazapi.Module
	Malloc wazapi.Function
	free   wazapi.Function
}

func main() {
	wasm, err := LoadWASM()
	if err != nil {
		log.Fatal("could not load WASM: ", err)
	}
	defer wasm.Close()

	user := &shared.User{
		ID:      42,
		Name:    "eleonora",
		IsAdmin: true,
	}

	ptr, err := wasm.Arg(user)
	if err != nil {
		log.Fatal("could not pass user as arg: ", err)
	}
	defer wasm.Free(ptr)

	if _, err := wasm.Call("foo", ptr); err != nil {
		log.Fatal("could not call foo: ", err)
	}
}

func LoadWASM() (*WASM, error) {
	var err error
	wasm := &WASM{}

	wasm.Ctx = context.Background()
	wasm.Run = wazero.NewRuntime(wasm.Ctx)

	if _, err := wasi.Instantiate(wasm.Ctx, wasm.Run); err != nil {
		return nil, errors.Wrap(err, "could not instantiate WASI")
	}

	wasm.Cfg = wazero.NewModuleConfig().
		WithStdin(os.Stdin).
		WithStdout(os.Stdout).
		WithStderr(os.Stderr)

	wasm.Mod, err = wasm.Run.InstantiateWithConfig(
		wasm.Ctx,
		code,
		wasm.Cfg,
	)
	if err != nil {
		return nil, errors.Wrap(err, "could not instantiate module")
	}

	wasm.Malloc = wasm.Mod.ExportedFunction("malloc")
	if wasm.Malloc == nil {
		return nil, errors.New("could not find WASM function malloc")
	}

	wasm.free = wasm.Mod.ExportedFunction("free")
	if wasm.free == nil {
		return nil, errors.New("could not find WASM function free")
	}

	return wasm, nil
}

func (w *WASM) Close() {
	w.Run.Close(w.Ctx)
}

func (w *WASM) Arg(value msgp.Marshaler) (ptr uint64, err error) {
	buf, err := value.MarshalMsg(nil)
	if err != nil {
		return 0, errors.Wrap(err, "could not marshal value")
	}

	size := uint64(len(buf))

	results, err := w.Malloc.Call(w.Ctx, size+8)
	if err != nil {
		return 0, errors.Wrap(err, "could not allocate WASM memory")
	}

	ptr = results[0]

	if !w.Mod.Memory().WriteUint64Le(uint32(ptr), size) ||
		!w.Mod.Memory().Write(uint32(ptr)+8, buf) {
		return 0, errors.New("could not write to WASM memory")
	}

	return
}

func (w *WASM) Free(data uint64) {
	w.free.Call(w.Ctx, data)
}

func (w *WASM) Call(name string, args ...uint64) ([]uint64, error) {
	return w.Mod.ExportedFunction(name).Call(w.Ctx, args...)
}
