package main

import (
	"fmt"
	"log"
	"reflect"

	"github.com/labstack/echo/v4"
	"github.com/pkg/errors"
)

type Example struct {
	Signed   int32
	Unsigned uint32

	Float  float32
	Double float64

	String string

	Array [4]string
	Slice []string

	Map map[int]int

	Struct struct {
		Function func(int) int
	}

	JSON int `json:"json"`

	Foo int `wired.name:"foo"`
	Bar struct {
		Baz int
	} `wired.atomic:""`

	Quux int `wired.ignore:""`
}

func main() {
	example := Example{
		Signed:   -42,
		Unsigned: 1337,
		Float:    12.34,
		Double:   1e-10,
		String:   "Hello, World!",
		Array: [4]string{
			"these",
			"are",
			"array",
			"items",
		},
		Slice: []string{
			"even",
			"more",
			"items",
		},
		Map: map[int]int{
			1: 2,
			2: 4,
			4: 8,
			8: 16,
		},
		Struct: struct{ Function func(int) int }{
			Function: func(i int) int {
				return i * 2
			},
		},
	}

	e := echo.New()
	e.Use(func(next echo.HandlerFunc) echo.HandlerFunc {
		return func(c echo.Context) error {
			err := next(c)
			if err != nil {
				log.Printf("%v %v: %v", c.Request().Method, c.Request().URL, err)
			}
			return err
		}
	})

	// e.GET("/Signed", func(c echo.Context) error {
	// 	return c.JSON(http.StatusOK, example.Signed)
	// })
	// e.POST("/Signed", func(c echo.Context) error {
	// 	return json.NewDecoder(c.Request().Body).Decode(&example.Signed)
	// })

	if err := Generator(e, &example); err != nil {
		log.Fatal("generator failed: ", err)
	}

	e.Start(":37812")
}

func Generator(e *echo.Echo, api any) error {
	val := reflect.ValueOf(api).Elem()

	if val.Kind() != reflect.Struct {
		return errors.New("API value is not a struct")
	}

	return RunStruct(e, val)
}

func RunStruct(e *echo.Echo, val reflect.Value) error {
	typ := val.Type()

	for i := 0; i < val.NumField(); i++ {
		fieldVal := val.Field(i)
		fieldFld := typ.Field(i)
		fieldTyp := fieldVal.Type()
		fieldKnd := fieldVal.Kind()

		log.Printf(
			"%s: %s (%s) = %v",
			fieldFld.Name,
			fieldTyp,
			fieldKnd,
			GetValue(fieldVal),
		)

		if fieldKnd == reflect.Struct {
			fmt.Fprintln(log.Writer())
			RunStruct(e, fieldVal)
		}
	}

	return nil
}
