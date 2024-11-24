package main

import (
	"fmt"
	"os"
	"runtime"
	"strings"
	"unsafe"

	"github.com/go-gl/gl/v4.3-core/gl"
	"github.com/go-gl/glfw/v3.3/glfw"
)

const WINDOW_WIDTH = 1000
const WINDOW_HEIGHT = 600

func init() {
	runtime.LockOSThread()
}

func main() {
	if err := glfw.Init(); err != nil {
		panic(err)
	}
	defer glfw.Terminate()

	glfw.WindowHint(glfw.ContextVersionMajor, 4)
	glfw.WindowHint(glfw.ContextVersionMinor, 3)
	glfw.WindowHint(glfw.OpenGLProfile, glfw.OpenGLCoreProfile)
	glfw.WindowHint(glfw.OpenGLForwardCompatible, glfw.True)

	window, err := glfw.CreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Test", nil, nil)
	if err != nil {
		panic(err)
	}
	window.MakeContextCurrent()
	window.SetFramebufferSizeCallback(framebufferSizeCallback)

	if err := gl.Init(); err != nil {
		panic(err)
	}

	vertexShader, err := loadShader(gl.VERTEX_SHADER, "vertex.glsl")
	if err != nil {
		panic(err)
	}

	fragmentShader, err := loadShader(gl.FRAGMENT_SHADER, "fragment.glsl")
	if err != nil {
		panic(err)
	}

	shaderProgram, err := loadProgram(vertexShader, fragmentShader)
	if err != nil {
		panic(err)
	}

	gl.UseProgram(shaderProgram)
	gl.DeleteShader(vertexShader)
	gl.DeleteShader(fragmentShader)

	vertices := []float32{
		//X     Y    R    G    B
		/*
		+0.5, +0.5, 1.0, 0.0, 0.0, // right top
		+0.5, -0.5, 1.0, 0.0, 1.0, // right bottom
		-0.5, +0.5, 0.0, 1.0, 0.0, // left  top
		-0.5, -0.5, 0.0, 0.0, 1.0, // left  bottom
		*/

		+1.0, +1.0, 255/255.0, 27/255.0, 141/255.0, // right top, magenta
		-1.0, +1.0, 255/255.0, 27/255.0, 141/255.0, // left  top, magenta
		+1.0, +0.3, 255/255.0, 27/255.0, 141/255.0, // right up,  magenta
		-1.0, +0.3, 255/255.0, 27/255.0, 141/255.0, // left  up,  magenta

		+1.0, +0.3, 255/255.0, 217/255.0, 0/255.0, // right up,   yellow
		-1.0, +0.3, 255/255.0, 217/255.0, 0/255.0, // left  up,   yellow
		+1.0, -0.3, 255/255.0, 217/255.0, 0/255.0, // right down, yellow
		-1.0, -0.3, 255/255.0, 217/255.0, 0/255.0, // left  down, yellow

		+1.0, -0.3, 27/255.0, 179/255.0, 255/255.0, // right down,   cyan
		-1.0, -0.3, 27/255.0, 179/255.0, 255/255.0, // left  down,   cyan
		+1.0, -1.0, 27/255.0, 179/255.0, 255/255.0, // right bottom, cyan
		-1.0, -1.0, 27/255.0, 179/255.0, 255/255.0, // left  bottom, cyan
	}

	indices := []uint32{
		0, 1, 2, 1, 2, 3,
		2, 3, 6, 3, 6, 7,
		6, 7, 10, 7, 10, 11,
	}

	var vao uint32
	gl.GenVertexArrays(1, &vao)
	gl.BindVertexArray(vao)

	var vbo uint32
	gl.GenBuffers(1, &vbo)
	gl.BindBuffer(gl.ARRAY_BUFFER, vbo)
	gl.BufferData(gl.ARRAY_BUFFER, arrSize(vertices), gl.Ptr(vertices), gl.STATIC_DRAW)

	var ebo uint32
	gl.GenBuffers(1, &ebo)
	gl.BindBuffer(gl.ELEMENT_ARRAY_BUFFER, ebo)
	gl.BufferData(gl.ELEMENT_ARRAY_BUFFER, arrSize(indices), gl.Ptr(indices), gl.STATIC_DRAW)

	gl.VertexAttribPointer(0, 2, gl.FLOAT, false,
		5*(int32)(unsafe.Sizeof(vertices[0])),
		nil,
	)
	gl.EnableVertexAttribArray(0)

	gl.VertexAttribPointer(1, 3, gl.FLOAT, false,
		5*(int32)(unsafe.Sizeof(vertices[0])),
		gl.PtrOffset(2*(int)(unsafe.Sizeof(vertices[0]))),
	)
	gl.EnableVertexAttribArray(1)

	for !window.ShouldClose() {
		gl.ClearColor(0.2, 0.3, 0.3, 1.0)
		gl.Clear(gl.COLOR_BUFFER_BIT)

		gl.DrawElements(gl.TRIANGLES, 18, gl.UNSIGNED_INT, nil)

		window.SwapBuffers()
		glfw.PollEvents()
	}
}

func framebufferSizeCallback(window *glfw.Window, width, height int) {
	gl.Viewport(0, 0, int32(width), int32(height))
}

func loadShader(typ uint32, file string) (shader uint32, err error) {
	data, err := os.ReadFile(file)
	if err != nil {
		return shader, err
	}

	src, free := gl.Strs(string(data))
	defer free()

	shader = gl.CreateShader(typ)
	gl.ShaderSource(shader, 1, src, nil)
	gl.CompileShader(shader)

	var success int32
	gl.GetShaderiv(shader, gl.COMPILE_STATUS, &success)
	if success == gl.FALSE {
		var logLength int32
		gl.GetShaderiv(shader, gl.INFO_LOG_LENGTH, &logLength)

		log := strings.Repeat("\x00", int(logLength+1))
		gl.GetShaderInfoLog(shader, logLength, nil, gl.Str(log))
		return shader, fmt.Errorf("could not compile shader %v: %v", file, log)
	}

	return shader, nil
}

func loadProgram(shaders ...uint32) (program uint32, err error) {
	program = gl.CreateProgram()

	for _, shader := range shaders {
		gl.AttachShader(program, shader)
	}
	gl.LinkProgram(program)

	var success int32
	gl.GetProgramiv(program, gl.LINK_STATUS, &success)
	if success == gl.FALSE {
		var logLength int32
		gl.GetProgramiv(program, gl.INFO_LOG_LENGTH, &logLength)

		log := strings.Repeat("\x00", int(logLength+1))
		gl.GetProgramInfoLog(program, logLength, nil, gl.Str(log))
		return program, fmt.Errorf("could not link program: %v", log)
	}

	return program, nil
}

func arrSize[T any](arr []T) int {
	return len(arr) * (int)(unsafe.Sizeof(arr[0]))
}
