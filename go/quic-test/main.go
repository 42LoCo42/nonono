package main

import (
	"log"

	"github.com/aerogo/aero"
	"github.com/quic-go/quic-go/http3"
)

func main() {
	app := aero.New()
	app.Use(func(handler aero.Handler) aero.Handler {
		return func(ctx aero.Context) error {
			log.Printf(
				"%s: %s %s",
				ctx.Request().Internal().RemoteAddr,
				ctx.Request().Method(),
				ctx.Request().Internal().URL,
			)
			return handler(ctx)
		}
	})

	app.Get("/", func(ctx aero.Context) error {
		return ctx.HTML("<h1>Hello World!</h1>")
	})

	app.Get("/foo", func(ctx aero.Context) error {
		return ctx.Text("foobar")
	})

	log.Fatal(http3.ListenAndServeQUIC(":37812", "crt", "key", app))
}
