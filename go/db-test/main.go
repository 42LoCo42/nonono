package main

import (
	"encoding/json"
	"log"
	"net/http"
	"strings"

	"dbtest/api"

	"github.com/asdine/storm/codec/msgpack"
	"github.com/asdine/storm/v3"
	_ "github.com/deepmap/oapi-codegen/pkg/runtime"
	"github.com/labstack/echo/v4"
)

const SONGFILE_BUCKET = "songfiles"

type MyServer struct {
	db *storm.DB
}

func main() {
	var err error
	server := new(MyServer)
	server.db, err = storm.Open(
		"test.db",
		storm.Codec(msgpack.Codec),
	)
	if err != nil {
		log.Fatal(err)
	}
	defer server.db.Close()

	e := echo.New()
	api.RegisterHandlers(e, server)
	e.Start("localhost:37812")
}

func Err(ctx echo.Context, err error) error {
	if err == storm.ErrNotFound {
		return ctx.NoContent(http.StatusNotFound)
	}
	return err
}

func JSON(ctx echo.Context, data any) error {
	return ctx.JSON(http.StatusOK, data)
}

// DeleteSongsName implements api.ServerInterface
func (s *MyServer) DeleteSongsName(ctx echo.Context, name string) error {
	tx, err := s.db.Begin(true)
	if err != nil {
		return Err(ctx, err)
	}
	defer tx.Rollback()

	if err := tx.DeleteStruct(&api.SongInfo{
		Name: name,
	}); err != nil {
		return Err(ctx, err)
	}

	if err := tx.Delete(SONGFILE_BUCKET, name); err != nil {
		return Err(ctx, err)
	}

	return tx.Commit()
}

// DeleteStatsId implements api.ServerInterface
func (s *MyServer) DeleteStatsId(ctx echo.Context, id uint64) error {
	return Err(ctx, s.db.DeleteStruct(&api.Stat{
		ID: id,
	}))
}

// DeleteUsersName implements api.ServerInterface
func (s *MyServer) DeleteUsersName(ctx echo.Context, name string) error {
	return Err(ctx, s.db.DeleteStruct(&api.User{
		Name: name,
	}))
}

// GetSongs implements api.ServerInterface
func (s *MyServer) GetSongs(ctx echo.Context) error {
	var songs []api.SongInfo
	if err := s.db.All(&songs); err != nil {
		return Err(ctx, err)
	}

	return JSON(ctx, songs)
}

// GetSongsName implements api.ServerInterface
func (s *MyServer) GetSongsName(ctx echo.Context, name string) error {
	var song api.SongInfo
	if err := s.db.One("Name", name, &song); err != nil {
		return Err(ctx, err)
	}

	return JSON(ctx, song)
}

// GetSongsNameFile implements api.ServerInterface
func (s *MyServer) GetSongsNameFile(ctx echo.Context, name string) error {
	var song api.SongFile
	if err := s.db.Get(SONGFILE_BUCKET, name, &song); err != nil {
		return Err(ctx, err)
	}

	return ctx.Blob(http.StatusOK, "application/octet-stream", song)
}

// GetStats implements api.ServerInterface
func (s *MyServer) GetStats(ctx echo.Context) error {
	var stats []api.Stat
	if err := s.db.All(&stats); err != nil {
		return Err(ctx, err)
	}

	return JSON(ctx, stats)

}

// GetStatsId implements api.ServerInterface
func (s *MyServer) GetStatsId(ctx echo.Context, id uint64) error {
	var stat api.Stat
	if err := s.db.One("ID", id, &stat); err != nil {
		return Err(ctx, err)
	}

	return JSON(ctx, stat)
}

// GetStatsSongSong implements api.ServerInterface
func (s *MyServer) GetStatsSongSong(ctx echo.Context, song string) error {
	var stats []api.Stat
	if err := s.db.Find("Song", song, &stats); err != nil {
		return Err(ctx, err)
	}

	return JSON(ctx, stats)
}

// GetStatsUserUser implements api.ServerInterface
func (s *MyServer) GetStatsUserUser(ctx echo.Context, user string) error {
	var stats []api.Stat
	if err := s.db.Find("User", user, &stats); err != nil {
		return Err(ctx, err)
	}

	return JSON(ctx, stats)
}

// GetUsers implements api.ServerInterface
func (s *MyServer) GetUsers(ctx echo.Context) error {
	var users []api.User
	if err := s.db.All(&users); err != nil {
		return Err(ctx, err)
	}

	return JSON(ctx, users)
}

// GetUsersName implements api.ServerInterface
func (s *MyServer) GetUsersName(ctx echo.Context, name string) error {
	var user api.User
	if err := s.db.One("Name", name, &user); err != nil {
		return Err(ctx, err)
	}

	return JSON(ctx, user)
}

// PostSongs implements api.ServerInterface
func (s *MyServer) PostSongs(ctx echo.Context) error {
	form, err := ctx.MultipartForm()
	if err != nil {
		return Err(ctx, err)
	}

	infos := form.Value["Info"]
	if len(infos) != 1 {
		return ctx.NoContent(http.StatusBadRequest)
	}

	files := form.File["File"]
	if len(files) != 1 {
		return ctx.NoContent(http.StatusBadRequest)
	}

	var info api.SongInfo
	if err := json.NewDecoder(strings.NewReader(infos[0])).Decode(&info); err != nil {
		return Err(ctx, err)
	}

	file, err := files[0].Open()
	if err != nil {
		return Err(ctx, err)
	}

	buf := make([]byte, files[0].Size)
	if _, err := file.Read(buf); err != nil {
		return Err(ctx, err)
	}

	tx, err := s.db.Begin(true)
	if err != nil {
		return Err(ctx, err)
	}
	defer tx.Rollback()

	if err := tx.Save(&info); err != nil {
		return Err(ctx, err)
	}

	if err := tx.Set(SONGFILE_BUCKET, info.Name, buf); err != nil {
		return Err(ctx, err)
	}

	return tx.Commit()
}

// PostStats implements api.ServerInterface
func (s *MyServer) PostStats(ctx echo.Context) error {
	var stat api.Stat
	if err := json.NewDecoder(ctx.Request().Body).Decode(&stat); err != nil {
		return Err(ctx, err)
	}

	if err := s.db.Save(&stat); err != nil {
		return Err(ctx, err)
	}

	return ctx.NoContent(http.StatusOK)
}

// PostUsers implements api.ServerInterface
func (s *MyServer) PostUsers(ctx echo.Context) error {
	var user api.User
	if err := json.NewDecoder(ctx.Request().Body).Decode(&user); err != nil {
		return Err(ctx, err)
	}

	if err := s.db.Save(&user); err != nil {
		return Err(ctx, err)
	}

	return ctx.NoContent(http.StatusOK)
}
