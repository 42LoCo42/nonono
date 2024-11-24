package main

import (
	"crypto/ed25519"
	"encoding/pem"
	"flag"
	"fmt"
	"log"
	"os"
	osuser "os/user"

	"github.com/pkg/errors"
	"golang.org/x/crypto/argon2"
	"golang.org/x/crypto/ssh"
	"golang.org/x/term"
)

func main() {
	user := flag.String("user", "", "Arbitrary username/salt")
	file := flag.String("file", "", "Name of the private key file.")
	passEnv := flag.Bool("passEnv", false, "Use the KG_PASS environment variable instead of reading the password?")
	flag.Parse()

	if *file == "" {
		log.Print("error: -file must be specified")
		flag.Usage()
		os.Exit(1)
	}

	if *user == "" {
		currentUser, err := osuser.Current()
		if err != nil {
			log.Fatal(errors.Wrap(err, "could not get current user, specify -user to fix!"))
		}

		*user = currentUser.Username
	}

	var pass []byte
	var err error

	if *passEnv {
		pass_, ok := os.LookupEnv("KG_PASS")
		if !ok || pass_ == "" {
			log.Fatal(errors.New("could not get password from KG_PASS"))
		}
		pass = []byte(pass_)
	} else {
		fmt.Print("password: ")
		pass, err = term.ReadPassword(int(os.Stdin.Fd()))
		if err != nil {
			log.Fatal(errors.Wrap(err, "could not read password"))
		}
	}

	secretKey := ed25519.PrivateKey(argon2.IDKey(
		pass,
		[]byte(*user),
		1,                      // time
		64*1024,                // memory in KiB
		4,                      // threads
		ed25519.PrivateKeySize, // output size in bytes
	))

	secretBlock := pem.Block{
		Type:  "OPENSSH PRIVATE KEY",
		Bytes: MarshalED25519PrivateKey(secretKey),
	}
	secretBytes := pem.EncodeToMemory(&secretBlock)

	publicKey, err := ssh.NewPublicKey(secretKey.Public())
	if err != nil {
		log.Fatal(errors.Wrap(err, "could not create SSH key"))
	}
	publicBytes := ssh.MarshalAuthorizedKey(publicKey)
	publicBytes = append(publicBytes[:len(publicBytes)-1], []byte(" "+*user)...)

	if err := os.WriteFile(*file, secretBytes, 0600); err != nil {
		log.Fatal(errors.Wrap(err, "could not save secret key"))
	}

	if err := os.WriteFile(*file+".pub", publicBytes, 0644); err != nil {
		log.Fatal(errors.Wrap(err, "could not save public key"))
	}
}
