package main

import (
	"encoding/json"
	"fmt"
	"log"
	"math/rand"
	"net/http"
	"net/url"
	"time"
)

var api = "https://neal.fun/api/infinite-craft/pair"

type Element struct {
	Name  string `json:"result"`
	Emoji string `json:"emoji"`
	IsNew bool   `json:"isNew"`
}

type Combo struct {
	first, second int
}

type Combos map[Combo]bool

func main() {
	elements := []Element{
		{Name: "Water", Emoji: "💧", IsNew: false},
		{Name: "Fire", Emoji: "🔥", IsNew: false},
		{Name: "Wind", Emoji: "🌬️", IsNew: false},
		{Name: "Earth", Emoji: "🌍", IsNew: false},
	}

	combos := Combos{}

	for {
		combo := Combo{
			first:  rand.Intn(len(elements)),
			second: rand.Intn(len(elements)),
		}
		if found := combos[combo]; found {
			continue
		}

		combos[combo] = true

		log.Print(combo)
		time.Sleep(time.Millisecond * 500)
		continue

		first := elements[combo.first]
		second := elements[combo.second]

		new, err := ask(first.Name, second.Name)
		if err != nil {
			log.Print(err)
			continue
		}
		if new != nil {
			log.Printf(
				"%v %v + %v %v -> %v %v (%v)",
				first.Emoji, first.Name,
				second.Emoji, second.Name,
				new.Emoji, new.Name, new.IsNew,
			)

			elements = append(elements, *new)
		}

		time.Sleep(time.Millisecond * 100)
	}
}

func ask(first, second string) (*Element, error) {
	final := fmt.Sprintf(
		"%s?first=%s&second=%s",
		api,
		url.QueryEscape(first),
		url.QueryEscape(second),
	)

	req, _ := http.NewRequest("GET", final, nil)
	req.Header.Add("Accept", "*/*")
	// req.Header.Add("Accept-Encoding", "gzip, deflate, br")
	req.Header.Add("Accept-Language", "en-US,en;q=0.5")
	req.Header.Add("Connection", "keep-alive")
	req.Header.Add("DNT", "1")
	req.Header.Add("Host", "neal.fun")
	req.Header.Add("Referer", "https://neal.fun/infinite-craft/")
	req.Header.Add("Sec-Fetch-Dest", "empty")
	req.Header.Add("Sec-Fetch-Mode", "cors")
	req.Header.Add("Sec-Fetch-Site", "same-origin")
	req.Header.Add("Sec-GPC", "1")
	req.Header.Add("User-Agent", "Mozilla/5.0 (X11; Linux x86_64; rv:123.0) Gecko/20100101 Firefox/123.0")

	resp, err := http.DefaultClient.Do(req)
	if err != nil {
		return nil, err
	}

	var element Element
	if err := json.NewDecoder(resp.Body).Decode(&element); err != nil {
		return nil, err
	}

	if element.Name == "Nothing" {
		return nil, fmt.Errorf("nothing")
	}

	return &element, nil
}
