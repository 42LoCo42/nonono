package main

import (
	"context"
	"fmt"
	"log"
	"os"

	"github.com/shurcooL/githubv4"
	"golang.org/x/oauth2"
)

func main() {
	if len(os.Args) != 3 {
		log.Fatal("Usage: github-newest-tag <user> <repo>")
	}

	ctx := context.Background()

	src := oauth2.StaticTokenSource(
		&oauth2.Token{AccessToken: os.Getenv("GITHUB_TOKEN")},
	)
	httpClient := oauth2.NewClient(ctx, src)

	client := githubv4.NewClient(httpClient)

	var query struct {
		Repository struct {
			Refs struct {
				Edges []struct {
					Node struct {
						Name string
					}
				}
			} `graphql:"refs(refPrefix: \"refs/tags/\", first: 1, orderBy: {field: TAG_COMMIT_DATE, direction: DESC})"`
		} `graphql:"repository(owner: $owner, name: $name)"`
	}

	vars := map[string]any{
		"owner": githubv4.String(os.Args[1]),
		"name":  githubv4.String(os.Args[2]),
	}

	if err := client.Query(ctx, &query, vars); err != nil {
		log.Fatal(err)
	}

	tags := query.Repository.Refs.Edges
	if len(tags) < 1 {
		log.Fatal("this repo has no tags!")
	}

	fmt.Println(query.Repository.Refs.Edges[0].Node.Name)
}
