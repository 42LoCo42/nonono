package main

import (
	"io"
	"log"
	"os"

	"github.com/emersion/go-smtp"
	"github.com/pkg/errors"
)

type Session struct {
	server *Server
	from   string
	target string
}

func (s *Session) AuthPlain(user, pass string) error {
	return nil
}

func (s *Session) Mail(from string, opts *smtp.MailOptions) error {
	log.Print("from: ", from)
	s.from = from
	return nil
}

func (s *Session) Rcpt(alias string) error {
	log.Print("rcpt: ", alias)

	user, target, err := s.server.FindAlias(alias)
	if err != nil {
		return errors.Wrapf(err, "could not handle rcpt %v", err)
	}

	s.target = target
	log.Printf(
		"user %v: %v -> %v",
		user.Name,
		alias,
		target,
	)

	return nil
}

func (s *Session) Data(r io.Reader) error {
	if s.target == "debug" {
		if _, err := io.Copy(os.Stdout, r); err != nil {
			return errors.Wrap(err, "could not copy email to debug output")
		}

		return nil
	}

	if err := s.server.SendMail(s.from, s.target, r); err != nil {
		return errors.Wrap(err, "could not forward mail")
	}

	return nil
}

func (s *Session) Reset() {}

func (s *Session) Logout() error {
	return nil
}
