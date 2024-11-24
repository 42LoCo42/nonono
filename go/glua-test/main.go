package main

import (
	_ "embed"
	"fmt"
	"log"
	"os"
	"reflect"
	"strings"

	"github.com/d5/tengo/v2"
	"github.com/d5/tengo/v2/stdlib"
)

//go:embed test.tengo
var src []byte

func StructIndexSet(this any, index, value tengo.Object) error {
	reflect.
		ValueOf(this).Elem().                           // this = pointer to struct; resolve it
		FieldByName(tengo.ToInterface(index).(string)). // get field
		Set(reflect.ValueOf(tengo.ToInterface(value)))  // set field value
	return nil
}

type Foo struct {
	tengo.ObjectImpl
	Bar int64
	Baz string
}

func (foo *Foo) IndexSet(index, value tengo.Object) error {
	return StructIndexSet(foo, index, value)
}

func (foo *Foo) String() string {
	return fmt.Sprintf("{bar = %v, baz = %v}", foo.Bar, foo.Baz)
}

func main() {
	script := tengo.NewScript(src)
	script.SetImports(stdlib.GetModuleMap(stdlib.AllModuleNames()...))
	script.Add("a", 1)
	script.Add("b", 2)
	script.Add("c", 3)
	script.Add("d", 4)

	script.Add("interop", &tengo.UserFunction{
		Value: func(args ...tengo.Object) (tengo.Object, error) {
			str := args[0].(*tengo.String).Value
			num := args[1].(*tengo.Int).Value
			res := strings.Repeat(str, int(num))

			log.Printf(
				"interop called with str = %v, num = %v -> %v",
				str, num, res,
			)

			return &Foo{
				Bar: 42,
				Baz: res,
			}, nil
		},
	})

	script.Add("println", &tengo.UserFunction{
		Value: func(args ...tengo.Object) (ret tengo.Object, err error) {
			values := []any{}
			for _, arg := range args {
				values = append(values, tengo.ToInterface(arg))
			}

			n, err := fmt.Fprintln(os.Stderr, values...)
			if err != nil {
				return nil, err
			}

			return tengo.FromInterface(n)
		},
	})

	res, err := script.Run()
	if err != nil {
		log.Fatal("tengo execution failed: ", err)
	}

	log.Printf(
		"sum = %v, mul = %v, res = %v",
		res.Get("sum"),
		res.Get("mul"),
		res.Get("res"),
	)
}
