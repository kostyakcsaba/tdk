#pragma once

// SDL
#include <SDL.h>
//#include "GL/glu.h"

// GLEW - for Windows
#include <GL/glew.h>
#include <GL/wglew.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

// mesh util
#include "ObjParser_OGL3.h"

// camera util
#include "GLMCamera.h"

// lib fájlok, amiket az exe-hez kell linkelni
#pragma comment(lib, "glu32.lib")

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void Mouse(SDL_MouseMotionEvent&);
	void Resize(SDL_ResizeEvent&);

private:
	void LoadShaderDuo( const char*, const char*, GLuint& );
	void DrawSphere();
	GLuint GenTexture();
	CGLMCamera	m_camera;

	// programs
	GLuint m_programSphereShader, m_programPointLight;

	// mesh
	Mesh	*mesh;

	GLuint	m_vboID, m_vaoID;
	
	// texture
	GLuint	m_suzTexture;

struct Vertex
	{
		glm::vec3 p;
		glm::vec2 t;
	};

};
