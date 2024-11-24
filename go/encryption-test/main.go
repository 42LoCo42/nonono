package main

// #cgo pkg-config: libsodium
// #include <sodium.h>
import "C"

import (
	"bufio"
	"crypto/sha512"
	"fmt"
	"log"
	"os"
	"runtime"

	"github.com/42LoCo42/z85m"
	"github.com/jamesruan/sodium"
	"github.com/nbutton23/zxcvbn-go"
	"github.com/pkg/errors"
	"golang.org/x/crypto/argon2"
	"golang.org/x/term"
)

func main() {
	userA, err := askKP()
	if err != nil {
		log.Fatal(errors.WithMessage(err, "could not read user A keypair"))
	}
	userABox := userA.ToBox()

	log.Print("Public key: ", toZ85M(userA.PublicKey.Bytes))
	log.Print("Secret key: ", toZ85M(userA.SecretKey.Bytes))

	fmt.Fprintln(os.Stderr)

	userB, err := askKP()
	if err != nil {
		log.Fatal(errors.WithMessage(err, "could not read user B keypair"))
	}
	userBBox := userB.ToBox()

	log.Print("Public key: ", toZ85M(userB.PublicKey.Bytes))
	log.Print("Secret key: ", toZ85M(userB.SecretKey.Bytes))

	msg := "foobar"

	fmt.Fprintln(os.Stderr)
	log.Print("anonymous encryption:")
	encrypted := anonEnc([]byte(msg), userBBox.PublicKey)
	log.Print("encrypted: ", toZ85M(encrypted))

	decrypted, err := anonDec(encrypted, userBBox)
	if err != nil {
		log.Fatal(errors.WithMessage(err, "could not decrypt"))
	}
	log.Print("decrypted: ", string(decrypted))

	log.Print("authenticated encryption:")
	encrypted = authEnc([]byte(msg), userABox.SecretKey, userBBox.PublicKey)
	log.Print("encrypted: ", toZ85M(encrypted))

	decrypted, err = authDec(encrypted, userABox.PublicKey, userBBox.SecretKey)
	if err != nil {
		log.Fatal(errors.WithMessage(err, "could not decrypt"))
	}
	log.Print("decrypted: ", string(decrypted))
}

func askKP() (sodium.SignKP, error) {
	reader := bufio.NewReader(os.Stdin)

	fmt.Fprint(os.Stderr, "Username: ")
	username, err := reader.ReadString('\n')
	if err != nil {
		return sodium.SignKP{}, errors.WithMessage(err, "could not read username")
	}

	fmt.Fprint(os.Stderr, "Password (will not be echoed): ")
	rawPassword, err := term.ReadPassword(int(os.Stdin.Fd()))
	if err != nil {
		return sodium.SignKP{}, errors.WithMessage(err, "could not read password")
	}

	password := string(rawPassword)
	fmt.Fprintln(os.Stderr)

	strength := zxcvbn.PasswordStrength(password, []string{username})
	log.Print("Entropy: ", strength.Entropy)
	log.Print("Cracking time: ", strength.CrackTimeDisplay)
	log.Printf("Score: %d/4", strength.Score)

	return cskpg(username, password), nil
}

// clientside keypair generation
// creates a key using only secret, clientside cryptographic material
func cskpg(user, pass string) sodium.SignKP {
	return sodium.SeedSignKP(sodium.SignSeed{
		Bytes: argon2.IDKey(
			[]byte(pass),
			sha512.New().Sum([]byte(user+"\x00"+pass)),
			uint32(sodium.CryptoPWHashOpsLimitModerate),
			uint32(sodium.CryptoPWHashMemLimitModerate>>10),
			uint8(runtime.NumCPU()),
			C.crypto_sign_SEEDBYTES,
		),
	})
}

func toZ85M(b []byte) string {
	encoded, err := z85m.Encode(b)
	if err != nil {
		return err.Error()
	}

	return string(encoded)
}

func anonEnc(msg []byte, to sodium.BoxPublicKey) []byte {
	return sodium.Bytes(msg).SealedBox(to)
}

func anonDec(encrypted []byte, to sodium.BoxKP) ([]byte, error) {
	return sodium.Bytes(encrypted).SealedBoxOpen(to)
}

func authEnc(msg []byte, from sodium.BoxSecretKey, to sodium.BoxPublicKey) []byte {
	nonce := sodium.BoxNonce{}
	sodium.Randomize(&nonce)

	encrypted := sodium.Bytes(msg).Box(nonce, to, from)
	return append(nonce.Bytes, encrypted...)
}

func authDec(encrypted []byte, from sodium.BoxPublicKey, to sodium.BoxSecretKey) ([]byte, error) {
	nonce := sodium.BoxNonce{Bytes: encrypted[:C.crypto_box_NONCEBYTES]}
	realEncrypted := encrypted[C.crypto_box_NONCEBYTES:]

	return sodium.Bytes(realEncrypted).BoxOpen(nonce, from, to)
}
