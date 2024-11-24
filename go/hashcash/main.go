package main

import (
	"crypto/rand"
	"crypto/sha1"
	"encoding/base64"
	"encoding/binary"
	"fmt"
	"log"
	"strings"
	"time"

	"github.com/go-faster/errors"
)

func absi64(i int64) int64 {
	if i < 0 {
		return -i
	}
	return i
}

type HashCash struct {
	Version  int
	Bits     int
	Date     int64
	Resource string
	Random   []byte
	Counter  uint64

	cache string
}

func (h *HashCash) Init(resource string) (*HashCash, error) {
	h.Version = 1
	h.Bits = 20
	h.Date = time.Now().Unix()
	h.Resource = resource
	h.Random = make([]byte, 12)
	h.Counter = 0

	if _, err := rand.Read(h.Random); err != nil {
		return nil, errors.Wrap(err, "could not generate random value")
	}

	return h, nil
}

func (h *HashCash) ShowHeader() string {
	if h.cache == "" {
		h.cache = fmt.Sprintf("%v:%v:%v:%v:%v:",
			h.Version, h.Bits, h.Date, h.Resource,
			base64.StdEncoding.EncodeToString(h.Random))
	}

	return h.cache + base64.StdEncoding.EncodeToString(
		binary.LittleEndian.AppendUint64(nil, h.Counter))
}

func (h *HashCash) ReadHeaderRaw(header string) (*HashCash, error) {
	var randomB64, counterB64 string
	if _, err := fmt.Sscanf(
		strings.ReplaceAll(header, ":", " "),
		"%v %v %v %v %v %v",
		&h.Version, &h.Bits, &h.Date, &h.Resource, &randomB64, &counterB64,
	); err != nil {
		return nil, errors.Wrap(err, "could not scanf input")
	}

	var err error
	h.Random, err = base64.StdEncoding.DecodeString(randomB64)
	if err != nil {
		return nil, errors.Wrapf(err, "could not decode random value from %v", randomB64)
	}

	counterRaw, err := base64.StdEncoding.DecodeString(counterB64)
	if err != nil {
		return nil, errors.Wrapf(err, "could not decode counter value from %v", counterB64)
	}
	h.Counter = binary.LittleEndian.Uint64(counterRaw)

	return h, nil
}

func (h *HashCash) ReadHeader(header string) (*HashCash, error) {
	if _, err := h.ReadHeaderRaw(header); err != nil {
		return nil, err
	}

	if h.Version != 1 {
		return nil, errors.Errorf("invalid version %v, must be 1", h.Version)
	}

	if h.Bits < 20 {
		return nil, errors.Errorf("number of bits too low, %v < 20", h.Bits)
	}

	if diff := absi64(time.Now().Unix() - h.Date); diff > 3600*24*2 {
		return nil, errors.Errorf("time difference too big, %v seconds > 2 days", diff)
	}

	return h, nil
}

func (h *HashCash) Hash() []byte {
	sum := sha1.Sum([]byte(h.ShowHeader()))
	return sum[:]
}

func (h *HashCash) IsValid() bool {
	sum := h.Hash()

	i := 0
	for ; i < h.Bits/8; i++ {
		if sum[i] != 0 {
			return false
		}
	}

	return sum[i] < (1 << (8 - h.Bits%8))
}

func (h *HashCash) MakeValid() {
	for !h.IsValid() {
		h.Counter++
	}
}

func main() {
	h := errors.Must(new(HashCash).Init("root@example.org"))
	h.MakeValid()
	log.Print(h.ShowHeader())
	log.Print(h.Hash())

	h = errors.Must(new(HashCash).ReadHeader(h.ShowHeader()))
	log.Print(h.IsValid())
}
