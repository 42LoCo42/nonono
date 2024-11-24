//go:build prod

package main

import (
	"embed"
	"net/http"

	"github.com/labstack/echo/v4"
	"github.com/labstack/echo/v4/middleware"
)

//go:embed dist
var dist embed.FS

func config(e *echo.Echo) {
	e.Use(
		middleware.StaticWithConfig(middleware.StaticConfig{
			Root:       "dist",
			Filesystem: http.FS(dist),
		}),
	)
}
