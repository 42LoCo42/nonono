package main

import (
	"encoding/binary"
	"unsafe"
	"wazero-test/shared"
)

//export foo
func foo(_user *byte) {
	user := &shared.User{}
	user.UnmarshalMsg(Arg(_user))

	print(user.Pretty())
}

func Arg(ptr *byte) []byte {
	return unsafe.Slice(
		(*byte)(unsafe.Add(unsafe.Pointer(ptr), 8)),
		binary.LittleEndian.Uint64(unsafe.Slice(ptr, 8)),
	)
}

func main() {}
