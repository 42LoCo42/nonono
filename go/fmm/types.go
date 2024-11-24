package main

import "time"

type Mods struct {
	Mods []Mod `json:"results"`
}

type Mod struct {
	Name           string    `json:"name"`
	Title          string    `json:"title"`
	Owner          string    `json:"owner"`
	Summary        string    `json:"summary"`
	DownloadsCount uint64    `json:"downloads_count"`
	Category       string    `json:"category"`
	Score          float64   `json:"score"`
	Thumbnail      string    `json:"thumbnail"`
	LatestRelease  Release   `json:"latest_release"`
	Releases       []Release `json:"releases"`
}

type Release struct {
	Info struct {
		Dependencies    []string `json:"dependencies"`
		FactorioVersion string   `json:"factorio_version"`
	} `json:"info_json"`
	ReleasedAt time.Time `json:"released_at"`
	Version    string    `json:"version"`
}
