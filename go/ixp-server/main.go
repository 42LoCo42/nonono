package main

// #cgo LDFLAGS: -lixp
// #include "wrappers.h"
import "C"

import (
	"fmt"
	"net"
	"unsafe"
)

func main() {
	listener, err := net.Listen("tcp", "localhost:37812")
	defer listener.Close()
	if err != nil {
		panic(err)
	}

	rawListener, err := listener.(*net.TCPListener).SyscallConn()
	if err != nil {
		panic(err)
	}

	rawListener.Control(setup)
}

func setup(fd uintptr) {
	srv9p := C.Ixp9Srv {
		// attach: (*[0]byte)(C._attach),
	}

	var srv C.IxpServer
	srv_conn := C.ixp_listen(
		&srv, C.int(fd), unsafe.Pointer(&srv9p),
		(*[0]byte)(C.ixp_serve9conn), nil,
	)
	fmt.Println(srv_conn)
	C.ixp_serverloop(&srv)
}

//export attach
func attach(req *C.Ixp9Req) {

}

//export clunk
func clunk(req *C.Ixp9Req) {

}

//export create
func create(req *C.Ixp9Req) {

}

//export flush
func flush(req *C.Ixp9Req) {

}

//export open
func open(req *C.Ixp9Req) {

}

//export read
func read(req *C.Ixp9Req) {

}

//export remove
func remove(req *C.Ixp9Req) {

}

//export stat
func stat(req *C.Ixp9Req) {

}

//export walk
func walk(req *C.Ixp9Req) {

}

//export write
func write(req *C.Ixp9Req) {

}

//export wstat
func wstat(req *C.Ixp9Req) {

}

//export freefid
func freefid(fid *C.IxpFid) {

}
