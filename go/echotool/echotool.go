package echotool

import (
	"crypto/rand"
	"encoding/base64"
	"fmt"
	"log"
	"net/http"
	"strings"
	"time"

	"github.com/golang-jwt/jwt"
	"github.com/labstack/echo/v4"
	"github.com/labstack/echo/v4/middleware"
	"github.com/pkg/errors"
	"golang.org/x/crypto/argon2"
)

func Die(code int, err error, format string, args ...any) error {
	res := echo.NewHTTPError(code)
	if err == nil {
		res.Internal = errors.Errorf(format, args...)
	} else {
		res.Internal = errors.Wrapf(err, format, args...)
	}
	return res
}

func MkHash(user string, pass string) string {
	return base64.StdEncoding.EncodeToString(argon2.IDKey(
		[]byte(pass),
		[]byte(user+pass),
		1,
		64*1024,
		4,
		32,
	))
}

func Auth(next echo.HandlerFunc) echo.HandlerFunc {
	return func(c echo.Context) error {
		if c.Get("user") == nil {
			return Die(http.StatusUnauthorized, nil, "needs authentication")
		}
		return next(c)
	}
}

func SetupDefaultEcho[User any](
	staticFS http.FileSystem,
	jwtKeyB64 string,
	jwtIssuer string,
	jwtLifetime time.Duration,
	name2user func(string) (User, error),
	user2hash func(User) (string, error),
) (*echo.Echo, *echo.Group) {
	if jwtKeyB64 == "" {
		key := make([]byte, 64)
		if _, err := rand.Read(key); err != nil {
			log.Fatal("could not generate new JWT key: ", err)
		}

		fmt.Println(base64.StdEncoding.EncodeToString(key))
		return nil, nil
	}

	jwtKey, err := base64.StdEncoding.DecodeString(jwtKeyB64)
	if err != nil {
		log.Fatal("could not decode JWT key: ", err)
	}

	e := echo.New()
	e.HideBanner = true

	e.Use(
		middleware.LoggerWithConfig(middleware.LoggerConfig{
			Format:           "${time_custom} ${remote_ip} - ${method} ${host}${uri} - ${status} ${error}\n",
			CustomTimeFormat: "2006/01/02 15:04:05",
		}),

		middleware.StaticWithConfig(middleware.StaticConfig{
			Root:       "static",
			Filesystem: staticFS,
		}),

		func(next echo.HandlerFunc) echo.HandlerFunc {
			return func(c echo.Context) error {
				var auth string

				if authCookie, err := c.Cookie("auth"); err == nil {
					auth = authCookie.Value
				}

				if authHeader := c.Request().Header.Get("Authorization"); authHeader != "" {
					if value, ok := strings.CutPrefix(authHeader, "Bearer"); ok {
						auth = value
					}
				}

				if auth != "" {
					token, err := jwt.Parse(auth, func(t *jwt.Token) (interface{}, error) {
						if _, ok := t.Method.(*jwt.SigningMethodHMAC); !ok {
							return nil, errors.Errorf(
								"JWT has invalid signing method: %v",
								t.Header["alg"],
							)
						}
						return jwtKey, nil
					})
					if err != nil {
						return Die(http.StatusUnauthorized, err, "could not get JWT")
					}

					name := token.Claims.(jwt.MapClaims)["sub"].(string)
					user, err := name2user(name)
					if err != nil {
						return Die(http.StatusUnauthorized, err, "could not get user %v", name)
					}

					c.Set("user", user)
				}

				return next(c)
			}
		},
	)

	api := e.Group("/api")
	api.POST("/login", func(c echo.Context) error {
		name := c.FormValue("username")
		user, err := name2user(name)
		if err != nil {
			return Die(http.StatusUnauthorized, err, "could not get user %v", name)
		}

		hashWant, err := user2hash(user)
		if err != nil {
			return Die(http.StatusUnauthorized, err, "could not get hash for user %v", name)
		}

		pass := c.FormValue("password")
		hashHave := MkHash(name, pass)

		if hashWant != hashHave {
			return Die(http.StatusUnauthorized, nil, "hash mismatch")
		}

		expires := time.Now().Add(jwtLifetime)
		token := jwt.NewWithClaims(jwt.SigningMethodHS512, jwt.StandardClaims{
			Issuer:    jwtIssuer,
			Subject:   name,
			IssuedAt:  time.Now().Unix(),
			ExpiresAt: expires.Unix(),
		})

		signed, err := token.SignedString(jwtKey)
		if err != nil {
			return Die(http.StatusUnauthorized, err, "could not sign token")
		}

		c.SetCookie(&http.Cookie{
			Name:     "auth",
			Value:    signed,
			Path:     "/",
			Expires:  expires,
			Secure:   true,
			HttpOnly: true,
			SameSite: http.SameSiteStrictMode,
		})

		return c.String(http.StatusOK, signed)
	})

	return e, api
}
