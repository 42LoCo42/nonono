package main

import (
	"context"
	"log"
	"net/http"
	"sync"

	petstore "ogen-test/petstore"
)

//go:generate go run github.com/ogen-go/ogen/cmd/ogen@latest --target petstore --clean api.yaml

type PetstoreService struct {
	Pets map[int64]petstore.Pet
	ID   int64
	mux  sync.Mutex
}

func NewPetstoreService() *PetstoreService {
	svc := &PetstoreService{}
	svc.Pets = make(map[int64]petstore.Pet)
	return svc
}

// AddPet implements api.Handler.
func (svc *PetstoreService) AddPet(ctx context.Context, req *petstore.Pet) (*petstore.Pet, error) {
	svc.mux.Lock()
	defer svc.mux.Unlock()

	id := req.ID.Or(svc.ID + 1)
	svc.ID = id
	req.ID.SetTo(id)

	svc.Pets[id] = *req
	return req, nil
}

// DeletePet implements api.Handler.
func (svc *PetstoreService) DeletePet(ctx context.Context, params petstore.DeletePetParams) error {
	svc.mux.Lock()
	defer svc.mux.Unlock()

	delete(svc.Pets, params.PetId)
	return nil
}

// GetPetById implements api.Handler.
func (svc *PetstoreService) GetPetById(ctx context.Context, params petstore.GetPetByIdParams) (petstore.GetPetByIdRes, error) {
	svc.mux.Lock()
	defer svc.mux.Unlock()

	pet, ok := svc.Pets[params.PetId]
	if !ok {
		return &petstore.GetPetByIdNotFound{}, nil
	}
	return &pet, nil
}

// UpdatePet implements api.Handler.
func (*PetstoreService) UpdatePet(ctx context.Context, params petstore.UpdatePetParams) error {
	panic("unimplemented")
}

func main() {
	service := NewPetstoreService()

	srv, err := petstore.NewServer(service)
	if err != nil {
		log.Fatal("could not create server: ", err)
	}

	log.Fatal(http.ListenAndServe(":8080", srv))
}
