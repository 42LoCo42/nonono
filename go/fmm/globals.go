package main

const (
	ALL_MODS_URL = "https://mods.factorio.com/api/mods?version=1.1&page_size=max"
	MOD_INFO_URL = "https://mods.factorio.com/api/mods/%s/full"
	DOWNLOAD_URL = "https://factorio-launcher-mods.storage.googleapis.com/%s/%s.zip"

	MOD_CACHE_FILE = "mods.json"
	MODS_DIR       = "mods"
	BASE_DIR       = "data/base"
	INFO_JSON      = "info.json"
)
