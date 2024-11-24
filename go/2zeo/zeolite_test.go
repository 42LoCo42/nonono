package zeolite_test

import (
	"log"
	"testing"

	"github.com/42LoCo42/go-zeolite"
	"github.com/jamesruan/sodium"
)

func TestHose(t *testing.T) {
	hose := zeolite.NewHose()

	log.Print(hose.One.Write([]byte("foobar")))
	hose.One.Close()

	buf := make([]byte, 5)
	log.Print(hose.Two.Read(buf))
	log.Print(hose.Two.Read(buf))
	log.Print(hose.Two.Read(buf))
	log.Print(string(buf))
	t.Fail()
}

func TestNewSession(t *testing.T) {
	hose := zeolite.NewHose()

	i1 := zeolite.NewIdentity()
	i2 := zeolite.NewIdentity()

	trust := func(spk sodium.SignPublicKey) (bool, error) {
		return true, nil
	}

	go func() {
		s1, err := i1.NewSession(hose.One, trust, true)
		if err != nil {
			t.Fatal(err)
		}

		log.Print("s1 me: ", zeolite.B64(i1.PublicKey.Bytes))
		log.Print("s1 ot: ", zeolite.B64(s1.OtherPK.Bytes))
		s1.Write([]byte("foobar"))
	}()

	s2, err := i2.NewSession(hose.Two, trust, false)
	if err != nil {
		t.Fatal(err)
	}

	log.Print("s2 me: ", zeolite.B64(i2.PublicKey.Bytes))
	log.Print("s2 ot: ", zeolite.B64(s2.OtherPK.Bytes))
}
