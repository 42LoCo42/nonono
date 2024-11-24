package main

import (
	"bytes"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"

	"github.com/jamesruan/sodium"
	"golang.org/x/crypto/argon2"
	"golang.org/x/term"
)

func client() error {
	var username string
	fmt.Print("Username: ")
	fmt.Scanln(&username)

	fmt.Print("Password: ")
	password, err := term.ReadPassword(int(os.Stdin.Fd()))
	if err != nil {
		return err
	}
	fmt.Println()

	seed := argon2.IDKey(password, []byte(username), 1, 64*1024, 1, 32)
	kp := sodium.SeedSignKP(sodium.SignSeed{Bytes: seed})

	log.Print(base64.StdEncoding.EncodeToString(kp.PublicKey.Bytes))

	resp, err := http.Get("http://localhost:8080/login")
	if err != nil {
		return err
	}

	nonceEnc, err := Read2String(resp.Body)
	if err != nil {
		return err
	}

	nonceRaw, err := base64.StdEncoding.DecodeString(nonceEnc)
	if err != nil {
		return err
	}

	signature := sodium.Bytes(nonceRaw).SignDetached(kp.SecretKey)

	req := LoginRequest{
		User:      username,
		Nonce:     nonceEnc,
		Signature: base64.StdEncoding.EncodeToString(signature.Bytes),
	}

	raw, err := json.Marshal(req)
	if err != nil {
		return err
	}

	resp, err = http.Post("http://localhost:8080/login", "application/json", bytes.NewReader(raw))
	if err != nil {
		return err
	}

	msg, err := Read2String(resp.Body)
	if err != nil {
		return err
	}
	log.Print(resp.Status, ": ", msg)

	return nil
}

func Read2String(r io.Reader) (string, error) {
	raw, err := io.ReadAll(r)
	if err != nil {
		return "", err
	}

	return string(raw), nil
}
