package main

import (
	"log"
	"net/http"

	"github.com/labstack/echo/v4"
	"github.com/labstack/echo/v4/middleware"
)

func main() {
	e := echo.New()
	e.Use(
		middleware.CSRFWithConfig(middleware.CSRFConfig{
			TokenLookup: "cookie:_csrf",

			CookieHTTPOnly: true,
			CookieSameSite: http.SameSiteStrictMode,
			CookieSecure:   true,
		}),

		middleware.GzipWithConfig(middleware.GzipConfig{
			Level: 5,
		}),

		middleware.Secure(),
	)

	config(e)

	api := e.Group("/api")

	api.GET("", func(c echo.Context) error {
		return c.NoContent(http.StatusOK)
	})

	counter := 0

	api.POST("/counter", func(c echo.Context) error {
		counter += 1
		log.Printf("counter is at %v!", counter)
		return c.JSON(http.StatusOK, counter)
	})

	api.DELETE("/counter", func(c echo.Context) error {
		counter = 0
		log.Print("counter was reset!")
		return c.NoContent(http.StatusOK)
	})

	if err := e.Start(":8080"); err != nil {
		log.Fatal(err)
	}
}
