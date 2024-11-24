package main

import (
	_ "embed"
	"flag"
	"io"
	"log"
	"net/http"
	"os"
	"path/filepath"

	"github.com/labstack/echo/v4"
	"github.com/pkg/errors"
)

//go:embed index.html
var index []byte

func main() {
	addr := flag.String("a", ":8080", "Address to listen on")
	flag.Parse()

	if *addr == "" {
		flag.Usage()
		os.Exit(1)
	}

	e := echo.New()
	e.HideBanner = true

	e.GET("/", func(c echo.Context) error {
		return c.HTMLBlob(http.StatusOK, index)
	})

	e.POST("/upload/:path", func(c echo.Context) error {
		path := filepath.Join("upload", c.Param("path"))
		log.Print("uploading ", path)
		if err := os.MkdirAll(filepath.Dir(path), 0755); err != nil {
			return errors.Wrap(err, "could not create upload target dir")
		}
		out, err := os.Create(path)
		if err != nil {
			return errors.Wrap(err, "could not create upload target file")
		}
		defer out.Close()
		if _, err := io.Copy(out, c.Request().Body); err != nil {
			return errors.Wrap(err, "could not copy data to file")
		}
		return nil
	})

	log.Fatal(e.Start(*addr))
}
