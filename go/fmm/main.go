package main

import (
	"log"

	"github.com/rivo/tview"
)

func main() {
	mods, err := readModCache()
	if err != nil {
		panic(err)
	}

	app := tview.NewApplication()

	searchbar := tview.NewInputField().
		SetLabel("Search: ").
		SetFieldWidth(30).
		SetChangedFunc(func(text string) {
			log.Print("search bar changed: ", text)
		})

	list := tview.NewList()
	for _, mod := range mods.Mods {
		list.AddItem(mod.Title, mod.Summary, 'a', nil)
	}

	grid := tview.NewGrid().
		SetRows(1, 0).
		SetBorders(true).
		AddItem(searchbar, 0, 0, 0, 0, 0, 0, true)
		// AddItem(list, 1, 0, 0, 0, 0, 0, false)

	if err := app.SetRoot(grid, true).SetFocus(grid).Run(); err != nil {
		panic(err)
	}
}
