package main

import (
	"crypto/rand"
	"crypto/sha256"
	"encoding/hex"
	"fmt"
	"log"
	"reflect"

	"github.com/knusbaum/go9p"
	"github.com/knusbaum/go9p/fs"
	"github.com/pkg/errors"
)

const id = "leonsch"

type Song struct {
	Name   string
	Length float64
	Data   []byte
}

type API struct {
	val *Song
	sub Song
	get func() ([]byte, error)
	set func(data []byte) error
	fun func(self *API, id int) (string, error) `wired:"func"`
}

var api = API{
	val: &Song{
		Name:   "test",
		Length: 13.37,
		Data:   []byte{1, 2, 3, 4, 5, 6},
	},

	sub: Song{
		Name:   "foobar",
		Length: 1234,
		Data:   nil,
	},

	get: func() ([]byte, error) {
		data := make([]byte, 65535)
		if _, err := rand.Read(data); err != nil {
			return nil, errors.Wrap(err, "could not generate random data")
		}

		sum := sha256.Sum256(data)
		log.Print("get: ", hex.EncodeToString(sum[:]))

		return data, nil
	},

	set: func(data []byte) error {
		sum := sha256.Sum256(data)
		log.Print("set: ", hex.EncodeToString(sum[:]))
		return nil
	},

	fun: func(api *API, id int) (string, error) {
		log.Print("func executed")
		return fmt.Sprintf(
			"Hello, ID %d! val is %v",
			id,
			api.val,
		), nil
	},
}

func generate(
	fs_ *fs.FS,
	dir *fs.StaticDir,
	root any,
	val reflect.Value,
) error {
	typ := val.Type()

	for i := 0; i < val.NumField(); i++ {
		subVal := val.Field(i)
		subTyp := typ.Field(i)
		log.Print(subTyp.Name, ": ", subVal)

		switch k := subTyp.Type.Kind(); k {
		case reflect.Array | reflect.Slice:
			log.Print("TODO array")

		case reflect.Map:
			log.Print("TODO map")

		case reflect.Func:
			log.Print("I: ", subTyp.Type.NumIn())
			log.Print("O: ", subTyp.Type.NumOut())
			log.Print("TODO func")

		case reflect.Struct:
			subDir := fs.NewStaticDir(fs_.NewStat(
				subTyp.Name, id, id, 0555,
			))
			if err := dir.AddChild(subDir); err != nil {
				return errors.Wrap(err, "could not add subdir "+subTyp.Name)
			}
			if err := generate(fs_, subDir, root, subVal); err != nil {
				return errors.Wrap(err, "in struct "+subTyp.Name)
			}

		case reflect.Chan, reflect.Interface:
			return errors.Errorf(
				"field %s: type chan not supported",
				subTyp.Name,
			)

		default:
			log.Print("Unknown kind: ", k)
		}
	}

	return nil
}

func main() {
	staticFS, staticDir := fs.NewFS(id, id, 0555)
	if err := generate(
		staticFS, staticDir, &api, reflect.ValueOf(api),
	); err != nil {
		log.Fatal(err)
	}

	log.Print("Listening...")
	log.Fatal(errors.Wrap(
		go9p.Serve(":37812", staticFS.Server()),
		"could not serve the file system"),
	)
}
