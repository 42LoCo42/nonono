package main

import (
	"context"
	"log"

	"github.com/minio/minio-go/v7"
	"github.com/minio/minio-go/v7/pkg/credentials"
)

func main() {
	mc, err := minio.New("localhost:9000", &minio.Options{
		Creds: credentials.NewStaticV4(
			"RF6ha6fb4DrsEqF2Mqbm",
			"pVKCQY8fp8oUHVOLq5jkyl37ZZ8QB4LoAQjoEDMo",
			"",
		),
	})
	if err != nil {
		log.Fatal(err)
	}

	ctx := context.Background()
	if ok, err := mc.BucketExists(ctx, "songs"); err != nil {
		log.Fatal(err)
	} else if !ok {
		log.Fatal("bucket songs does not exist")
	}

	info, err := mc.FPutObject(ctx, "songs", "flake.lock", "flake.lock", minio.PutObjectOptions{
		UserMetadata: map[string]string{
			"metadata": "is cool",
		},
		UserTags: map[string]string{
			"pog": "yes",
		},
		ContentType: "application/json",
	})
	if err != nil {
		log.Fatal(err)
	}

	log.Print("uploaded successfully: ", info)

	stat, err := mc.StatObject(ctx, "songs", "flake.lock", minio.StatObjectOptions{})
	if err != nil {
		log.Fatal(err)
	}

	log.Print(stat.UserMetadata)

	tags, err := mc.GetObjectTagging(ctx, "songs", "flake.lock", minio.GetObjectTaggingOptions{})
	if err != nil {
		log.Fatal(err)
	}

	log.Print(tags.ToMap())

}
