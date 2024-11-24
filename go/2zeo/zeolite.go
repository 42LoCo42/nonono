package zeolite

import (
	"bytes"
	"encoding/base64"
	"io"
	"log"

	"github.com/jamesruan/sodium"
	"github.com/pkg/errors"
)

const Protocol = "zeolite2"

type (
	Identity struct {
		sodium.SignKP
	}

	Session struct {
		OtherPK sodium.SignPublicKey
		encoder sodium.SecretStreamEncoder
		decoder sodium.SecretStreamDecoder
	}

	Trust = func(sodium.SignPublicKey) (bool, error)
)

func B64(b []byte) string {
	return base64.StdEncoding.EncodeToString(b)
}

func NewIdentity() *Identity {
	return &Identity{sodium.MakeSignKP()}
}

func (i *Identity) NewSession(
	connection io.ReadWriter,
	trust Trust,
	server bool,
) (
	s *Session,
	err error,
) {
	s = &Session{}

	// create ephemeral key
	ephKP := sodium.MakeKXKP()

	// send protocol, public key, signed ephemeral key
	buf := bytes.NewBuffer(nil)
	buf.Write([]byte(Protocol))
	buf.Write(i.PublicKey.Bytes)
	buf.Write(ephKP.PublicKey.Bytes.Sign(i.SecretKey))
	if _, err := connection.Write(buf.Bytes()); err != nil {
		return nil, errors.Wrap(err, "could not send proto/key message")
	}

	// receive same things from other client
	rbuf := make([]byte, buf.Len())
	if _, err := connection.Read(rbuf); err != nil {
		return nil, errors.Wrap(err, "could not recv proto/key message")
	}

	otherProcol := string(rbuf[:len(Protocol)])
	s.OtherPK.Bytes = rbuf[len(Protocol) : len(Protocol)+s.OtherPK.Size()]
	otherSignedEphPK := rbuf[len(Protocol)+s.OtherPK.Size():]

	// compare protocol
	if Protocol != otherProcol {
		return nil, errors.Errorf(
			"protocol mismatch: want %s, got %s",
			Protocol,
			otherProcol,
		)
	}

	// check trust
	ok, err := trust(s.OtherPK)
	if err != nil {
		return nil, errors.Wrap(err, "error in trust check")
	}
	if !ok {
		return nil, errors.New("trust check failed")
	}

	// verify signed ephemeral public key
	otherEphPK, err := sodium.Bytes(otherSignedEphPK).SignOpen(s.OtherPK)
	if err != nil {
		return nil, errors.Wrap(err, "signature verification failed")
	}

	// select key exchange function
	var kx func(sodium.KXPublicKey) (*sodium.KXSessionKeys, error)
	if server {
		kx = ephKP.ServerSessionKeys
	} else {
		kx = ephKP.ClientSessionKeys
	}

	// perform key exchange
	sessionKeys, err := kx(sodium.KXPublicKey{Bytes: otherEphPK})
	if err != nil {
		return nil, errors.Wrap(err, "could not do key echange")
	}

	log.Print("rx: ", B64(sessionKeys.Rx.Bytes))
	log.Print("tx: ", B64(sessionKeys.Tx.Bytes))

	// init encoder
	s.encoder = sodium.MakeSecretStreamXCPEncoder(
		sodium.SecretStreamXCPKey(sessionKeys.Tx),
		connection,
	)

	// exchange header
	header := s.encoder.Header().Bytes
	log.Print("header 1: ", s.encoder.Header())
	if _, err := connection.Write(s.encoder.Header().Bytes); err != nil {
		return nil, errors.Wrap(err, "could not send header")
	}

	if _, err := connection.Read(header); err != nil {
		return nil, errors.Wrap(err, "could not recv header")
	}
	log.Print("header 2: ", header)

	s.decoder, err = sodium.MakeSecretStreamXCPDecoder(
		sodium.SecretStreamXCPKey(sessionKeys.Rx),
		connection,
		sodium.SecretStreamXCPHeader{Bytes: header},
	)
	if err != nil {
		return nil, errors.Wrap(err, "could not init decoder")
	}

	return s, nil
}

func (s *Session) Read(p []byte) (n int, err error) {
	return s.decoder.Read(p)
}

func (s *Session) Write(p []byte) (n int, err error) {
	return s.encoder.Write(p)
}

func (s *Session) Close() error {
	return s.encoder.Close()
}
