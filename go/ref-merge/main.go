package main

import (
	"encoding/json"
	"log"
	"os"
	"strings"
)

const REF = "$ref"

func main() {
	if err := run(); err != nil {
		log.Fatal(err)
	}
}

func run() error {
	var data any
	if err := json.NewDecoder(os.Stdin).Decode(&data); err != nil {
		return err
	}

	Traverse(data, data)

	if err := json.NewEncoder(os.Stdout).Encode(data); err != nil {
		return err
	}

	return nil
}

func Traverse(root, json any) {
	switch v := json.(type) {
	case map[string]any:
		if MustMerge(v) {
			path := strings.Split(v[REF].(string)[2:], "/")
			delete(v, REF)

			data := Get(root, path...).(map[string]any)
			for key, val := range data {
				v[key] = val
			}
		}

		for _, v := range v {
			Traverse(root, v)
		}

	case []any:
		for _, v := range v {
			Traverse(root, v)
		}
	}
}

func Get(json any, keys ...string) any {
	current := json

	for _, key := range keys {
		next, ok := current.(map[string]any)
		if !ok {
			log.Fatal("no map at ", keys)
		}

		current = next[key]
	}

	return current
}

func MustMerge(json any) bool {
	m := json.(map[string]any)

	for key := range m {
		if key == REF {
			return true && len(m) > 1 // at least one other key present
		}
	}

	return false
}
