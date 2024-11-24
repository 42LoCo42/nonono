package zeolite

import (
	"errors"
)

type half struct {
	send chan []byte
	recv chan []byte
	buf  []byte
}

// Read implements io.Reader
func (h *half) Read(p []byte) (n int, err error) {
	if h.buf == nil {
		var ok bool
		h.buf, ok = <-h.recv

		if !ok {
			return 0, errors.New("channel is closed")
		}
	}

	count := copy(p, h.buf)

	if count < len(h.buf) {
		h.buf = h.buf[count:]
	} else {
		h.buf = nil
	}

	return count, nil
}

// Write implements io.Writer
func (h *half) Write(p []byte) (n int, err error) {
	if h.send == nil {
		return 0, errors.New("channel is closed")
	}

	h.send <- p
	return len(p), nil
}

// Close implements io.Closer
func (h *half) Close() error {
	close(h.send)
	h.send = nil
	return nil
}

type Hose struct {
	One *half
	Two *half
}

func NewHose() *Hose {
	oneChan := make(chan []byte, 100)
	twoChan := make(chan []byte, 100)

	return &Hose{
		One: &half{
			send: oneChan,
			recv: twoChan,
		},
		Two: &half{
			send: twoChan,
			recv: oneChan,
		},
	}
}
