package main

import (
	"io"
	"log"
	"net"
	"strings"

	"github.com/emersion/go-smtp"
	"github.com/pkg/errors"
)

type Server struct {
	Users []User
}

func (s *Server) NewSession(c *smtp.Conn) (smtp.Session, error) {
	return &Session{
		server: s,
	}, nil
}

func (s *Server) FindAlias(name string) (User, string, error) {
	for _, user := range s.Users {
		for alias, target := range user.Aliases {
			if alias == name {
				return user, target, nil
			}
		}
	}

	return User{}, "", errors.Errorf("no user found for rcpt %v", name)
}

func (s *Server) SendMail(from string, rcpt string, msg io.Reader) error {
	parts := strings.SplitN(rcpt, "@", 2)
	if len(parts) != 2 {
		return errors.Errorf("rcpt %v must have exactly one @", rcpt)
	}

	mx, err := net.LookupMX(parts[1])
	if err != nil {
		return errors.Wrapf(err, "could not lookup MX for %v", rcpt)
	}
	if len(mx) < 1 {
		return errors.Errorf("no MX records found for %v", rcpt)
	}

	host := strings.TrimSuffix(mx[0].Host, ".") + ":25"
	log.Printf("connecting to %v", host)

	if err := smtp.SendMail(
		host,
		nil,
		from,
		[]string{rcpt},
		msg,
	); err != nil {
		return errors.Wrapf(
			err,
			"could not send mail to %v",
			rcpt,
		)
	}

	log.Print("mail successfully sent!")

	return nil
}
