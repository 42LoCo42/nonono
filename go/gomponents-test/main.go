package main

import (
	"embed"
	"log"
	"os/exec"
	"time"

	"github.com/labstack/echo/v4"
	g "github.com/maragudk/gomponents"
	c "github.com/maragudk/gomponents/components"
	. "github.com/maragudk/gomponents/html"
)

//go:embed static
var static embed.FS

func main() {
	exec.Command("notify-send", "reloaded").Run()

	e := echo.New()
	e.StaticFS("/", static)

	e.GET("/", createHandler(indexPage()))
	e.GET("/contact", createHandler(contactPage()))
	e.GET("/about", createHandler(aboutPage()))

	if err := e.Start(":8080"); err != nil {
		log.Fatal(err)
	}
}

func createHandler(title string, body g.Node) echo.HandlerFunc {
	return func(c echo.Context) error {
		return Page(title, c.Request().URL.Path, body).Render(c.Response())
	}
}

func indexPage() (string, g.Node) {
	return "Welcome!", Div(
		H1(g.Text("Welcome to this example page")),
		P(g.Text("I hope it will make you happy. 😄 It's using TailwindCSS for styling.")),
	)
}

func contactPage() (string, g.Node) {
	return "Contact", Div(
		H1(g.Text("Contact us")),
		P(g.Text("Just do it.")),
	)
}

func aboutPage() (string, g.Node) {
	return "About", Div(
		H1(g.Text("About this site")),
		P(g.Text("This is a site showing off gomponents.")),
	)
}

func Page(title, path string, body g.Node) g.Node {
	block := func(color string) g.Node {
		return Span(Class(color), g.Raw("&nbsp;&nbsp;&nbsp;"))
	}

	// HTML5 boilerplate document
	return c.HTML5(c.HTML5Props{
		Title:    title,
		Language: "en",
		Head: []g.Node{
			Link(Rel("stylesheet"), Href("/static/tailwind.css"), Type("text/css")),
		},
		Body: []g.Node{
			Class("bg-black"),
			Navbar(path, []PageLink{
				{Path: "/", Name: "Home"},
				{Path: "/contact", Name: "Contact"},
				{Path: "/about", Name: "About"},
			}),
			Container(
				Prose(body),
				PageFooter(),
				Div(Class("prose"),
					block("bg-red"),
					block("bg-green"),
					block("bg-yellow"),
					block("bg-blue"),
					block("bg-purple"),
				),
			),
		},
	})
}

type PageLink struct {
	Path string
	Name string
}

func Navbar(currentPath string, links []PageLink) g.Node {
	return Nav(Class("bg-gray mb-4"),
		Container(
			Div(Class("flex items-center space-x-4 h-16"),
				// We can Map custom slices to Nodes
				g.Group(g.Map(links, func(l PageLink) g.Node {
					return NavbarLink(l.Path, l.Name, currentPath == l.Path)
				})),
			),
		),
	)
}

// NavbarLink is a link in the Navbar.
func NavbarLink(path, text string, active bool) g.Node {
	return A(Href(path), g.Text(text),
		// Apply CSS classes conditionally
		c.Classes{
			"px-3 py-2 rounded-md text-sm font-medium":   true,
			"text-white bg-black":                        active,
			"text-black hover:text-white hover:bg-black": !active,
		},
	)
}

func Container(children ...g.Node) g.Node {
	return Div(Class("max-w-7xl mx-auto px-2 sm:px-6 lg:px-8"), g.Group(children))
}

func Prose(children ...g.Node) g.Node {
	return Div(Class("prose"), g.Group(children))
}

func PageFooter() g.Node {
	now := time.Now()

	return Footer(Class("prose prose-sm"), P(
		// We can use string interpolation directly, like fmt.Sprintf.
		g.Textf("Rendered %v. ", now.Format(time.RFC3339)),

		// Conditional inclusion
		g.If(now.Second()%2 == 0, g.Text("It's an even second.")),
		g.If(now.Second()%2 == 1, g.Text("It's an odd second.")),
	),

		P(A(Href("https://www.gomponents.com"), g.Text("gomponents"))),
	)
}
