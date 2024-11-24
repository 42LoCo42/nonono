package shared

import "fmt"

//go:generate go run github.com/tinylib/msgp -v -io=false -tests=false

type User struct {
	ID      int
	Name    string
	IsAdmin bool
}

func (u *User) Pretty() string {
	return "" +
		fmt.Sprintf("ID:       %v\n", u.ID) +
		fmt.Sprintf("Name:     %v\n", u.Name) +
		fmt.Sprintf("Is Admin: %v\n", u.IsAdmin)
}
