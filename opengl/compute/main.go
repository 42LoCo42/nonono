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

	window, err := glfw.CreateWindow(800, 600, "Compute", nil, nil)
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

	vertices := []float32{
		+0.5, +0.5, // right top
		+0.5, -0.5, // right bottom
		-0.5, +0.5, // left  top
		-0.5, -0.5, // left  bottom
	}
	indices := []uint32{
		0, 1, 2,
		1, 2, 3,
	}

	var vao uint32
	gl.GenVertexArrays(1, &vao)
	gl.BindVertexArray(vao)

	var vbo uint32
	gl.GenBuffers(1, &vbo)
	gl.BindBuffer(gl.ARRAY_BUFFER, vbo)
	gl.BufferData(gl.ARRAY_BUFFER, len(vertices)*(int)(unsafe.Sizeof(vertices[0])), gl.Ptr(vertices), gl.STATIC_DRAW)

	var ebo uint32
	gl.GenBuffers(1, &ebo)
	gl.BindBuffer(gl.ELEMENT_ARRAY_BUFFER, ebo)
	gl.BufferData(gl.ELEMENT_ARRAY_BUFFER, len(indices)*(int)(unsafe.Sizeof(indices[0])), gl.Ptr(indices), gl.STATIC_DRAW)

	gl.VertexAttribPointer(0, 2, gl.FLOAT, false, 2*(int32)(unsafe.Sizeof(vertices[0])), nil)
	gl.EnableVertexAttribArray(0)

	gl.PolygonMode(gl.FRONT_AND_BACK, gl.LINE)

	for !window.ShouldClose() {
		gl.ClearColor(0.2, 0.3, 0.3, 1.0)
		gl.Clear(gl.COLOR_BUFFER_BIT)

		gl.DrawElements(gl.TRIANGLES, 6, gl.UNSIGNED_INT, nil)

		window.SwapBuffers()
		glfw.PollEvents()
	}
}

func loadShader(typ uint32, file string) (shader uint32, err error) {
	data, err := os.ReadFile(file)
	if err != nil {
		return shader, err
	}

	sources, free := gl.Strs(string(data))
	shader = gl.CreateShader(typ)
	gl.ShaderSource(shader, 1, sources, nil)
	free()
	gl.CompileShader(shader)

	var status int32
	gl.GetShaderiv(shader, gl.COMPILE_STATUS, &status)
	if status == gl.FALSE {
		var length int32
		gl.GetShaderiv(shader, gl.INFO_LOG_LENGTH, &length)

		log := strings.Repeat("\x00", int(length+1))
		gl.GetShaderInfoLog(shader, length, nil, gl.Str(log))

		return shader, fmt.Errorf("Failed to compile shader %v: %v", file, log)
	}

	return shader, nil
}

func loadProgram(shaders ...uint32) (program uint32, err error) {
	program = gl.CreateProgram()

	for _, shader := range shaders {
		gl.AttachShader(program, shader)
	}

	gl.LinkProgram(program)

	var status int32
	gl.GetProgramiv(program, gl.LINK_STATUS, &status)
	if status == gl.FALSE {
		var length int32
		gl.GetProgramiv(program, gl.INFO_LOG_LENGTH, &length)

		log := strings.Repeat("\x00", int(length+1))
		gl.GetProgramInfoLog(program, length, nil, gl.Str(log))

		return program, fmt.Errorf("Failed to link program: %v", log)
	}

	for _, shader := range shaders {
		gl.DeleteShader(shader)
	}

	return program, nil
}

func framebufferSizeCallback(window *glfw.Window, width, height int) {
	gl.Viewport(0, 0, int32(width), int32(height))
}
