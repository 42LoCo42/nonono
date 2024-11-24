package main

import (
	"encoding/json"
	"log"
	"os"
)

func refreshModCache() error {
	log.Print("Refreshing mod cache...")

	body, err := getURL(ALL_MODS_URL)
	if err != nil {
		return err
	}

	return os.WriteFile(MOD_CACHE_FILE, body, 0644)
}

func readModCache() (*Mods, error) {
	data, err := os.ReadFile(MOD_CACHE_FILE)
	if err != nil {
		if os.IsNotExist(err) {
			if err := refreshModCache(); err != nil {
				return nil, err
			}
			return readModCache()
		}
		return nil, err
	}

	var mods Mods
	if err := json.Unmarshal(data, &mods); err != nil {
		return nil, err
	}

	return &mods, nil
}
