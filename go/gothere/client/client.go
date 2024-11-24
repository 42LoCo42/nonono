package main

import (
	"log"
	"strings"

	"github.com/emersion/go-smtp"
	"github.com/pkg/errors"
)

func main() {
	if err := run(); err != nil {
		log.Fatal(err)
	}
}

func run() error {
	if err := smtp.SendMail(
		"42loco42.duckdns.org:25",
		nil,
		"gothere-dev@example.org",
		[]string{"proton@eleonora.gay"},
		strings.NewReader("foobar this is an email"),
	); err != nil {
		return errors.Wrap(err, "could not send mail")
	}

	return nil
}
