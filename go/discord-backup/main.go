package main

import (
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"path"
	"strconv"
	"strings"
	"time"

	"github.com/bwmarrin/discordgo"
)

func main() {
	token, err := ioutil.ReadFile(os.Args[1])
	if err != nil {
		panic(err)
	}

	discord, err := discordgo.New(strings.TrimSpace(string(token)))
	if err != nil {
		panic(err)
	}

	guildNames := map[string]string{}
	channels := map[string]*discordgo.Channel{}

	discord.AddHandler(func(s *discordgo.Session, m *discordgo.MessageCreate) {
		channel, found := channels[m.ChannelID]
		if !found {
			channel, err = s.Channel(m.ChannelID)
			if err != nil {
				channel = &discordgo.Channel{
					Name: m.ChannelID,
					Type: discordgo.ChannelTypeDM,
				}
				log.Printf("Fallback to channel ID %s: %s", channel.Name, err)
			}
			channels[m.ChannelID] = channel
		}


		var category string
		switch channel.Type {
		case discordgo.ChannelTypeDM: category = "direct"
		case discordgo.ChannelTypeGroupDM: category = "group"
		default: category = "guild"
		}

		dir := path.Join("storage", category)
		if m.GuildID == "" {
			dir = path.Join(dir, channel.Name)
		} else {
			guildName, found := guildNames[m.GuildID]
			if !found {
				guild, err := s.Guild(m.GuildID)
				if err != nil {
					guildName = m.GuildID
					log.Printf("Fallback to guild ID %s: %s", guildName, err)
				} else {
					guildName = guild.Name
				}
				guildNames[m.GuildID] = guildName
			}

			dir = path.Join(
				dir,
				"server",
				guildName,
				channel.Name,
			)
		}

		dir = path.Join(dir, m.Timestamp.Format(time.RFC3339Nano))
		log.Print("Saving message ", dir)

		if err := os.MkdirAll(dir, 0755); err != nil {
			log.Printf("Could not create message directory %s: %s", dir, err)
			return
		}

		raw, err := json.Marshal(*m.Message)
		if err != nil {
			log.Print("Could not convert message to JSON: ", err)
		} else {
			if err := ioutil.WriteFile(path.Join(dir, "msg.json"), raw, 0644); err != nil {
				log.Print("Could not write JSON: ", err)
				fmt.Println(string(raw))
			}
		}

		if err := ioutil.WriteFile(path.Join(dir, "content"), []byte(m.Content+"\n"), 0644); err != nil {
			log.Printf("Could not write content \"%s\": %s", m.Content, err)
		}

		if err := ioutil.WriteFile(path.Join(dir, "author"), []byte(m.Author.Username+"\n"), 0644); err != nil {
			log.Printf("Could not write author %s: %s", m.Author.Username, err)
		}

		for i, a := range m.Attachments {
			resp, err := http.Get(a.URL)
			if err != nil {
				log.Printf("Could not download attachment %s: %s", a.URL, err)
				continue
			}
			defer resp.Body.Close()

			name := strconv.Itoa(i) + "_" + a.Filename
			file, err := os.Create(path.Join(dir, name))
			if err != nil {
				log.Printf("Could not open file %s: %s", name, err)
				continue
			}
			defer file.Close()

			if _, err := io.Copy(file, resp.Body); err != nil {
				log.Printf("Could not save attachment to file %s: %s", name, err)
			}
		}
	})

	discord.AddHandler(func(s *discordgo.Session, m *discordgo.MessageUpdate) {
	})

	discord.Identify.Intents =
		discordgo.IntentsGuildMessages |
			discordgo.IntentDirectMessages |
			discordgo.IntentMessageContent
	if err := discord.Open(); err != nil {
		panic(err)
	}

	log.Print("Press enter to stop.")
	fmt.Scanln()
	log.Print("Shutting down...")
	discord.Close()
}
