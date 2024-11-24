package main

import (
	"bufio"
	"log"
	"os"
	"time"

	"github.com/hugolgst/rich-go/client"
)

func main() {
	if err := client.Login("1085537727212179566"); err != nil {
		log.Fatal(err)
	}

	defer func() {
		if err := client.SetActivity(client.Activity{}); err != nil {
			log.Print("Failed resetting activity: ", err)
		}
		client.Logout()
	}()

	now := time.Now()
	// end := now.Add(time.Minute * 10)

	activity := client.Activity{
		Details:    "details",
		State:      "state",
		LargeImage: "https://eleonora.gay/profile.jpg",
		LargeText:  "large text",
		SmallImage: "https://eleonora.gay/favicon.png",
		SmallText:  "small text",
		Party: &client.Party{
			ID:         "42",
			Players:    13,
			MaxPlayers: 37,
		},
		Timestamps: &client.Timestamps{
			Start: &now,
			// End:   &end,
		},
		Buttons: []*client.Button{{
			Label: "label",
			Url:   "https://eleonora.gay/",
		}},
	}

	if err := client.SetActivity(activity); err != nil {
		log.Fatal(err)
	}

	log.Print("Running...")
	log.Print("Hit ENTER to stop")

	in := bufio.NewReader(os.Stdin)
	in.ReadString('\n')
}
