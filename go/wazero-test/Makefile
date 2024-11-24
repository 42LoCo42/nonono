run: wasm/test.wasm
	go run .

wasm/test.wasm: wasm/test.go
	tinygo build -o $@ -target=wasi $<
