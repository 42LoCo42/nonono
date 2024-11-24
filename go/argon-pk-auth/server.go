package main

import (
	"encoding/base64"
	"encoding/json"
	"log"
	"net/http"
	"time"

	"github.com/jamesruan/sodium"
	"github.com/labstack/echo/v4"
	"github.com/labstack/echo/v4/middleware"
)

type LoginRequest struct {
	User      string
	Nonce     string
	Signature string
}

func server() error {
	nonces := map[string]any{}
	users := map[string]sodium.SignPublicKey{}

	raw, err := base64.StdEncoding.DecodeString("RalGW8hs4ZYwRk+rBFD18d2U5lCeMQLfsVegmB5O8g8=")
	if err != nil {
		return err
	}

	users["username"] = sodium.SignPublicKey{Bytes: raw}

	e := echo.New()
	e.GET("/login", func(c echo.Context) error {
		for {
			nonce := Nonce{}
			sodium.Randomize(&nonce)
			enc := base64.StdEncoding.EncodeToString(nonce.Bytes)
			if _, found := nonces[enc]; !found {
				nonces[enc] = nil
				go func() {
					time.Sleep(time.Second * 10)
					delete(nonces, enc)
				}()
				return c.String(http.StatusOK, enc)
			}
		}
	})

	e.POST("/login", func(c echo.Context) error {
		fail := func(err error) *echo.HTTPError {
			return &echo.HTTPError{
				Code:     http.StatusForbidden,
				Message:  "Login Failed!",
				Internal: err,
			}
		}

		req := LoginRequest{}
		if err := json.NewDecoder(c.Request().Body).Decode(&req); err != nil {
			return fail(err)
		}

		defer delete(nonces, req.Nonce)

		pubkey, found := users[req.User]
		if !found {
			return fail(err)
		}

		nonce, err := base64.StdEncoding.DecodeString(req.Nonce)
		if err != nil {
			return fail(err)
		}

		signature, err := base64.StdEncoding.DecodeString(req.Signature)
		if err != nil {
			return fail(err)
		}

		if err := sodium.Bytes(nonce).SignVerifyDetached(
			sodium.Signature{Bytes: signature},
			pubkey,
		); err != nil {
			return fail(err)
		}

		return nil
	})

	e.Use(
		middleware.RecoverWithConfig(middleware.RecoverConfig{
			LogErrorFunc: func(c echo.Context, err error, stack []byte) error {
				log.Print(err)
				return nil
			},
		}),
	)

	return e.Start(":8080")
}
