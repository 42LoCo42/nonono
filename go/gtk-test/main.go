package main

import (
	_ "embed"
	"log"

	"github.com/gotk3/gotk3/gtk"
)

//go:embed ui.glade
var ui string

func main() {
	gtk.Init(nil)

	builder, err := gtk.BuilderNewFromString(ui)
	if err != nil {
		log.Fatal("could not create UI builder: ", err)
	}

	window := getObject[*gtk.Window](builder, "window")
	window.Connect("destroy", gtk.MainQuit)

	gtk.Main()
}

func getObject[T any](builder *gtk.Builder, name string) T {
	raw, err := builder.GetObject(name)
	if err != nil {
		log.Fatalf("could not get object %s: %v", name, err)
	}

	return raw.(T)
}
