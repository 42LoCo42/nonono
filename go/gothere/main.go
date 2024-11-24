package main

import (
	"crypto/tls"
	"flag"
	"log"
	"net"

	"github.com/emersion/go-smtp"
	"github.com/pkg/errors"
)

func main() {
	addr := flag.String("a", ":2525", "address to listen on")
	cert := flag.String("c", "cert.pem", "path to certificate file")
	key := flag.String("k", "key.pem", "path to key file")

	flag.Parse()

	if err := run(*addr, *cert, *key); err != nil {
		log.Fatal(err)
	}
}

func run(address, certpath, keypath string) error {
	s := smtp.NewServer(&Server{
		Users: []User{{
			Name: "testuser",
			Aliases: map[string]string{
				"proton@eleonora.gay": "leonsch@protonmail.com",
				"debug@eleonora.gay":  "debug",
			},
		}},
	})

	cert, err := tls.LoadX509KeyPair(certpath, keypath)
	if err != nil {
		return errors.Wrap(err, "could not load certificate")
	}

	s.MaxRecipients = 1
	s.EnableSMTPUTF8 = true
	s.TLSConfig = &tls.Config{
		Certificates: []tls.Certificate{cert},
		ServerName:   "eleonora.gay",
	}

	network := "tcp"
	pretty := network + "://" + address

	l, err := net.Listen(network, address)
	if err != nil {
		return errors.Wrapf(err, "could not listen on %v", pretty)
	}

	log.Printf("listening on %v", pretty)

	if err := s.Serve(l); err != nil {
		return errors.Wrap(err, "could not run server")
	}

	return nil
}
