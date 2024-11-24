package main

import (
	"log"

	"github.com/gotk3/gotk3/gtk"
	gooey_objects "github.com/suda/go-gooey/pkg/objects"
	gooey_properties "github.com/suda/go-gooey/pkg/properties"
	gooey_widgets "github.com/suda/go-gooey/pkg/widgets"
)

func main() {
	gtk.Init(nil)

	counter := gooey_properties.NewStringProperty()

	window := gooey_objects.Window{
		Title:   "GUI Test",
		Destroy: func() { gtk.MainQuit() },
		Children: []gooey_widgets.Widgetable{
			&gooey_widgets.Box{
				Orientation: gtk.ORIENTATION_VERTICAL,
				Spacing:     5,
				Children: []gooey_widgets.Widgetable{
					&gooey_widgets.Label{
						Text: *counter,
					},
					&gooey_widgets.Button{
						Label:   "Hello!",
						Clicked: func() { counter.Set(counter.Value + " 👋") },
					},
				},
			},
		},
	}

	if err := window.Open(); err != nil {
		log.Fatal(err)
	}

	gtk.Main()
}
