//go:build !prod

package main

import "github.com/labstack/echo/v4"

func config(e *echo.Echo) {}
