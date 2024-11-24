package main

import (
	"log"
	"os"

	"github.com/hanwen/go-fuse/v2/fuse"
)

type FS struct {
	fuse.RawFileSystem
}

func (fs *FS) Lookup(
	cancel <-chan struct{},
	header *fuse.InHeader,
	name string,
	out *fuse.EntryOut,
) (status fuse.Status) {
	log.Print(header.Pid)
	return fuse.ToStatus(nil)
}

func main() {
	if len(os.Args) < 2 {
		log.Fatalf("Usage: %s <dir>", os.Args[0])
	}
	dir := os.Args[1]

	fs := &FS{
		RawFileSystem: fuse.NewDefaultRawFileSystem(),
	}

	server, err := fuse.NewServer(fs, dir, &fuse.MountOptions{Debug: true})
	if err != nil {
		log.Fatal(err)
	}

	defer server.Unmount()
	server.Serve()
}
