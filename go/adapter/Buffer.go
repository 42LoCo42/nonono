package main

type Buffer struct {
	Data []byte
}

func (b *Buffer) Read9(offset, count uint64) []byte {
	ulen := uint64(len(b.Data))

	if offset >= ulen {
		return nil
	}

	if offset+count > ulen {
		count = ulen - offset
	}

	return b.Data[offset : offset+count]
}

func (b *Buffer) Write9(offset uint64, data []byte) uint32 {
	ulen := uint64(len(b.Data))
	count := uint64(len(data))
	newlen := offset + count

	if newlen > ulen {
		b.Data = append(b.Data, make([]byte, newlen-ulen)...)
	}

	copy(b.Data[offset:newlen], data)
	return uint32(len(data))
}

func (b *Buffer) Write(p []byte) (n int, err error) {
	b.Data = p
	return len(p), nil
}
