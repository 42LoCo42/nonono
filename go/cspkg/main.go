package main

// #include <sodium.h>
import "C"

import (
	"bufio"
	"bytes"
	"crypto/sha512"
	"encoding/binary"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"runtime"

	"github.com/42LoCo42/z85m"
	"github.com/jamesruan/sodium"
	"github.com/pkg/errors"
	"golang.org/x/crypto/argon2"
	"golang.org/x/term"
)

type stackTracer interface {
	StackTrace() errors.StackTrace
}

func printStackTrace(err error) {
	log.Print(err)
	if err, ok := err.(stackTracer); ok {
		for _, f := range err.StackTrace() {
			fmt.Fprintf(os.Stderr, "%+s:%d\n", f, f)
		}
	}
}

func failWithTrace(err error) {
	printStackTrace(err)
	os.Exit(1)
}

func toZ85M(b []byte) string {
	enc, err := z85m.Encode(b)
	if err != nil {
		return err.Error()
	}
	return string(enc)
}

type ZeoliteConn struct {
	r io.Reader
	w io.Writer
}

func (z *ZeoliteConn) NextLen() (uint32, error) {
	var l uint32
	if err := binary.Read(z.r, binary.LittleEndian, &l); err != nil {
		return 0, errors.Wrap(err, "Could not read length")
	}
	return l, nil
}

func (z *ZeoliteConn) Write(b []byte) (int, error) {
	buf := make([]byte, 4)
	binary.LittleEndian.PutUint32(buf, uint32(len(b)))
	buf = append(buf, b...)
	return z.w.Write(buf)
}

func (conn *ZeoliteConn) Connect(
	sgnKP sodium.SignKP,
	callback func(sodium.SignPublicKey) error,
) (client *ZeoliteClient, err error) {
	client = &ZeoliteClient{
		conn: conn,
	}

	// create keys
	ephKP := sodium.MakeKXKP()
	signedEphPK := ephKP.PublicKey.Sign(sgnKP.SecretKey)

	// create header
	protocol := []byte("zeolite2")
	buf := bytes.NewBuffer(protocol)
	if _, err := buf.Write(sgnKP.PublicKey.Bytes); err != nil {
		return nil, errors.Wrap(err, "Could not buffer static public key")
	}
	if _, err := buf.Write(signedEphPK); err != nil {
		return nil, errors.Wrap(err, "Could not buffer ephemeral public key")
	}

	// send header
	header := buf.Bytes()
	go func() {
		if _, err := conn.w.Write(header); err != nil {
			printStackTrace(errors.Wrap(err, "Could not send header"))
		}
	}()

	// receive other header
	if _, err := io.ReadFull(conn.r, header); err != nil {
		return nil, errors.Wrap(err, "Could not receive header")
	}

	otherProtocol := header[:len(protocol)]
	otherSgnKey := sodium.SignPublicKey{Bytes: header[len(protocol) : len(protocol)+len(sgnKP.PublicKey.Bytes)]}
	otherEphKey := header[len(protocol)+len(sgnKP.PublicKey.Bytes):]

	// validate other header
	if sodium.MemCmp(protocol, otherProtocol, len(protocol)) != 0 {
		return nil, errors.Errorf("Protocol mismatch: expected %s, got %s", protocol, otherProtocol)
	}

	if err := callback(otherSgnKey); err != nil {
		return nil, errors.Wrapf(err, "Untrusted public key %s", otherSgnKey)
	}

	verifiedEphKey, err := sodium.Bytes(otherEphKey).SignOpen(otherSgnKey)
	if err != nil {
		return nil, errors.Wrap(err, "Could not verify ephemeral key")
	}

	// create stream en/decoder
	symK := symStreamKX(ephKP.SecretKey, sodium.KXPublicKey{Bytes: verifiedEphKey})
	client.enc = sodium.MakeSecretStreamXCPEncoder(symK, conn)
	client.dec, err = sodium.MakeSecretStreamXCPDecoder(symK, conn.r, client.enc.Header())
	if err != nil {
		return nil, errors.Wrap(err, "Could not create decoder")
	}

	return client, err
}

type ZeoliteClient struct {
	conn *ZeoliteConn
	enc  sodium.SecretStreamEncoder
	dec  sodium.SecretStreamDecoder
}

func (c *ZeoliteClient) Send(msg string) error {
	if _, err := c.enc.Write([]byte(msg)); err != nil {
		return errors.Wrap(err, "Could not send message")
	}
	return nil
}

func (c *ZeoliteClient) Recv() (string, error) {
	l, err := c.conn.NextLen()
	if err != nil {
		return "", errors.Wrap(err, "Could not receive message length")
	}

	msg := make([]byte, l-(uint32)(C.crypto_secretstream_xchacha20poly1305_abytes()))
	if _, err := c.dec.Read(msg); err != nil {
		if err == io.EOF {
			return "", err
		}
		return "", errors.Wrap(err, "Could not receive message")
	}

	return string(msg), nil
}

func (c *ZeoliteClient) Close() error {
	return c.enc.Close()
}

func main() {
	server, err := net.Listen("tcp", ":37812")
	if err != nil {
		log.Fatal(err)
	}

	var accept net.Conn
	wait := make(chan int)
	go func() {
		accept, err = server.Accept()
		if err != nil {
			log.Fatal(err)
		}
		wait <- 0
	}()

	client, err := net.Dial("tcp", ":37812")
	if err != nil {
		log.Fatal(err)
	}
	<-wait

	conn := ZeoliteConn{
		r: accept,
		w: client,
	}

	user, pass, err := askCreds()
	if err != nil {
		failWithTrace(errors.Wrap(err, "Could not read credentials"))
	}

	sgnKP := genKP(user, pass)

	log.Print("Local key: ", toZ85M(sgnKP.PublicKey.Bytes))
	zeo, err := conn.Connect(sgnKP, func(pk sodium.SignPublicKey) error {
		log.Print("Other key: ", toZ85M(pk.Bytes))
		return nil
	})
	if err != nil {
		failWithTrace(errors.Wrap(err, "Could not connect client"))
	}

	go func() {
		for i := 0; i < 5; i++ {
			if err := zeo.Send("foobar"); err != nil {
				failWithTrace(err)
			}
		}
		zeo.Close()
	}()

	for {
		msg, err := zeo.Recv()
		if err != nil {
			if err == io.EOF {
				break
			}
			failWithTrace(err)
		}
		log.Print(msg)
	}
}

func askCreds() (user, pass string, err error) {
	fmt.Fprint(os.Stderr, "Username: ")

	reader := bufio.NewReader(os.Stdin)
	user, err = reader.ReadString('\n')
	if err != nil {
		return "", "", errors.Wrap(err, "Could not read username")
	}

	fmt.Fprint(os.Stderr, "Password: ")
	passB, err := term.ReadPassword(int(os.Stdin.Fd()))
	fmt.Fprintln(os.Stderr)
	if err != nil {
		return "", "", errors.Wrap(err, "Could not read passsword")
	}
	pass = string(passB)

	return user, pass, nil
}

func genKP(user, pass string) sodium.SignKP {
	return sodium.SeedSignKP(sodium.SignSeed{
		Bytes: argon2.IDKey(
			[]byte(pass),
			sha512.New().Sum([]byte(user+"\x00"+pass)),
			uint32(sodium.CryptoPWHashOpsLimitInteractive),
			uint32(sodium.CryptoPWHashMemLimitInteractive>>10),
			uint8(runtime.NumCPU()),
			(uint32)(C.crypto_sign_seedbytes()),
		),
	})
}

func symStreamKX(secret sodium.KXSecretKey, public sodium.KXPublicKey) sodium.SecretStreamXCPKey {
	q := sodium.CryptoScalarmult(sodium.Scalar(secret), sodium.Scalar(public)).Bytes
	return sodium.SecretStreamXCPKey{
		Bytes: sodium.NewGenericHash(
			(int)(C.crypto_secretstream_xchacha20poly1305_keybytes()),
		).Sum(q)[len(q):],
	}
}
