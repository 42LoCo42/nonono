package main

import (
	"crypto/sha256"
	"encoding/hex"
	"fmt"
	"log"

	"github.com/open-quantum-safe/liboqs-go/oqs"
)

func main() {
	kem := oqs.KeyEncapsulation{}
	defer kem.Clean()

	if err := kem.Init("Kyber512", nil); err != nil {
		log.Fatal("could not create KEM: ", err)
	}

	pubkey, err := kem.GenerateKeyPair()
	if err != nil {
		log.Fatal("could not create keypair: ", err)
	}

	fingerprint := sha256.Sum256(pubkey)
	fmt.Println(
		"Public key fingerprint:",
		hex.EncodeToString(fingerprint[:]),
	)
}
