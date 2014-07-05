#include "MyApp.h"
#include "GLUtils.hpp"
#include <Wingdi.h>

//
//seged fuggvenyek a tavolsag meghatarozasahoz es reszletesseg n�velesehez
//
const int iterations=20;
const float scale=1.5;
const float fixedRadius2=10;
const float minRadius2=1;
const float foldingLimit=1.0;

//mandelbox seg�d f�ggv�nyei
void boxFold(glm::vec3 &z,  float &dz)
{
    glm::vec3 z1 = glm::clamp(z, -foldingLimit, foldingLimit) ;
    z=glm::vec3(z1.x*2,z1.y*2,z1.z*2)-z;
}

void sphereFold(glm::vec3 &z,float &dz)
{
    float r2 = glm::dot(z,z);
    if (r2<minRadius2)
    {
        // line�ris bels� m�retez�s
        float temp = (fixedRadius2/minRadius2);
        z *= temp;
        dz*= temp;
    } else if (r2<fixedRadius2)
    {
        // ez a t�nyleges g�mb invert�l�s
        float temp =(fixedRadius2/r2);
        z *= temp;
        dz*= temp;
    }
}


CMyApp::CMyApp(void)
{
	mesh = 0;
}


CMyApp::~CMyApp(void)
{
}

GLuint CMyApp::GenTexture()
{
     static float tex[256][256][256][3];
 glm::vec3 p;
    for (int i=0; i<256; ++i){
        for (int j=0; j<256; ++j){
			for (int z=0; z<256; ++z)
        {

	p = glm::vec3((float)i/50.0,(float)j/50.0,(float)z/50.0);
    p.x-=2.5;
	p.y-=2.5;
	p.z-=2.5;
        /////////////////////////

	if ((sqrt((float)(p.x*p.x + p.y*p.y + p.z*p.z)) - 2) < 0.1)
	{

		tex[i][j][z][0] = 0;
	}
	else{
		tex[i][j][z][0] = (sqrt((float)(p.x*p.x + p.y*p.y + p.z*p.z)) - 2);
	}
			tex[i][j][z][1] = 0;
			tex[i][j][z][2] = 0;
			}}}
	GLuint tmpID;
	 glGenTextures(1, &tmpID);
	// aktiv�ljuk a most gener�lt nev� text�r�t
    glBindTexture(GL_TEXTURE_3D, tmpID);

	
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP); glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// download 3D volume texture for pre-classification 
	glTexImage3D( GL_TEXTURE_3D, 0, GL_RGB8, 256, 256, 256, 0, GL_RGB, GL_FLOAT, tex );
	// t�lts�k fel adatokkal az...
    
	glBindTexture(GL_TEXTURE_3D, 0);

	return tmpID;
}


bool CMyApp::Init()
{
	glEnable(GL_TEXTURE_3D);
	PFNGLTEXIMAGE3DPROC glTexImage3D;
	glTexImage3D = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");


	// t�rl�si sz�n legyen k�kes
	glClearColor(0, 0, 0, 1.0f);
	

	// kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// geometria kialak�t�sa
	Vertex geom[] =
	{
		//			 p.x	p.y	  p.z
		{glm::vec3( -1.0f, -1.0f, 0.0f), glm::vec2(0,0)},
		{glm::vec3(  1.0f, -1.0f, 0.0f), glm::vec2(1,0)},
		{glm::vec3( -1.0f,  1.0f, 0.0f), glm::vec2(0,1)},
		{glm::vec3(  1.0f,  1.0f, 0.0f), glm::vec2(1,1)},
	};
	
	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(m_vaoID);

	// gener�ljunk egy puffert
	glGenBuffers(1, &m_vboID);
	// puffer objektum haszn�latba v�tele
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	// a haszn�latba vett puffer objektum l�trehoz�sa
	// GL_STREAM_DRAW jelzi, hogy kirajzol�shoz sz�ks�ges adatokat t�rol
	glBufferData(GL_ARRAY_BUFFER,	// toltsunk adatok a vertexpufferre raallitott taroloba
				 sizeof(geom),		// ennyi bajtnyi adatot
				 geom,				// innen vegyuk az adatot amiket attoltunk
				 GL_STATIC_DRAW);	// ilyen felhasznalasi moddal

	// VAO-ban �ll�tsuk be a 0-�s csatorna hogyan szedje az adatait a feljegyzett VBO-b�l
	glEnableVertexAttribArray(0); // ez lesz majd a poz�ci�
	glVertexAttribPointer(
		(GLuint)0,		// a VB-ben tal�lhat� adatok k�z�l a 0. "index�" attrib�tumait �ll�tjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. index� attrib�tum hol kezd�dik a sizeof(Vertex)-nyi ter�leten bel�l
	); 

	// VAO deaktiv�l�sa, ne jegyezzen fel ezut�ni m�veleteket
	glBindVertexArray(0);
	// VBO deaktiv�l�sa
	glBindBuffer(GL_ARRAY_BUFFER, 0); 


	//
	// shaderek bet�lt�se
	//
	LoadShaderDuo("PointLight.vert", "PointLight.frag", m_programPointLight);
	LoadShaderDuo("sphereShader.vert", "sphereShader.frag", m_programSphereShader);

	// modellek bet�lt�se
	mesh = ObjParser::parse("Suzanne.obj");
	mesh->initBuffers();

	// textura betoltese
	m_suzTexture	= GenTexture();

	return true;
}

void CMyApp::Clean()
{
	delete mesh;

	glDeleteShader(m_programPointLight);
	glDeleteShader(m_programSphereShader);

	glDeleteTextures( 1, &m_suzTexture );

	// OGL takaritas
	glDeleteBuffers( 1, &m_vboID );
	glDeleteVertexArrays( 1, &m_vaoID );

}

void CMyApp::LoadShaderDuo( const char* _vsFileName, const char* _fsFileName, GLuint& _programID)
{
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		_vsFileName);
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	_fsFileName);

	// a shadereket t�rol� program l�trehoz�sa
	_programID = glCreateProgram();

	// adjuk hozz� a programhoz a shadereket
	glAttachShader(_programID, vs_ID);
	glAttachShader(_programID, fs_ID);

	// attributomok osszerendelese a VAO es shader kozt
	glBindAttribLocation( _programID, 0, "vs_in_pos");
	glBindAttribLocation( _programID, 1, "vs_in_normal");
	glBindAttribLocation( _programID, 2, "vs_in_tex");

	// illessz�k �ssze a shadereket (kimen�-bemen� v�ltoz�k �sszerendel�se stb.)
	glLinkProgram(_programID);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 1 && !result )
	{
		std::vector<char> ProgramErrorMessage( infoLogLength );
		glGetProgramInfoLog(_programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
	}

	// mar nincs ezekre szukseg
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );
}

void CMyApp::Update()
{
	// kamera update
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time)/1000.0f;

	last_time = SDL_GetTicks();

	// nezeti trafo frissitese
	m_camera.Update(delta_time);
}

void CMyApp::Render()
{
	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Suzanner kirajzolasa
	glUseProgram(m_programPointLight);

	glm::mat4 sWorld = glm::translate<float>(2,0,0);

	GLint location = -1;
	location = glGetUniformLocation(m_programPointLight, "ModelViewProjection");
	glm::mat4 mvp = m_camera.GetViewProj() * sWorld;
	glUniformMatrix4fv(location, 1, false, &mvp[0][0]);

	location = glGetUniformLocation(m_programPointLight, "Model");
	glUniformMatrix4fv(location, 1, false, &sWorld[0][0]);

	location = glGetUniformLocation(m_programPointLight, "InvModel");
	glm::mat4 sWI = glm::inverse( sWorld );
	glUniformMatrix4fv(location, 1, true, &sWI[0][0]);	

	//location = glGetUniformLocation(m_programPointLight, "lightPosition");
	//glUniform3f(location, m_camera.GetEye().x, m_camera.GetEye().y, m_camera.GetEye().z);

	location = glGetUniformLocation(m_programPointLight, "eyePosition");
	glUniform3f(location, m_camera.GetEye().x, m_camera.GetEye().y, m_camera.GetEye().z);

	mesh->draw();

	
	glUseProgram( 0 );

	//
	// gomb kirajzolasa
	//
	DrawSphere();

	SDL_GL_SwapBuffers();
}

void CMyApp::DrawSphere()
{
	// kapcsoljuk be a shaderunket
	glUseProgram(m_programSphereShader);

	GLint location = -1;

	/*
uniform mat4 projI;
uniform mat4 viewI;
uniform mat4 modelI;
uniform mat4 model;
uniform mat4 view;
	*/

	glm::mat4 model(1.0f);

	location = glGetUniformLocation(m_programSphereShader, "viewProj");
	glUniformMatrix4fv(location, 1, false, &m_camera.GetViewProj()[0][0]);

	location = glGetUniformLocation(m_programSphereShader, "model");
	glUniformMatrix4fv(location, 1, false, &model[0][0]);

	location = glGetUniformLocation(m_programSphereShader, "modelI");
	glm::mat4 mit = glm::inverse( model );
	glUniformMatrix4fv(location, 1, false, &mit[0][0]);

	location = glGetUniformLocation(m_programSphereShader, "view");
	glUniformMatrix4fv(location, 1, false, &m_camera.GetViewMatrix()[0][0]);

	location = glGetUniformLocation(m_programSphereShader, "viewI");
	glm::mat4 vi = glm::inverse( m_camera.GetViewMatrix() );
	glUniformMatrix4fv(location, 1, false, &vi[0][0]);

	location = glGetUniformLocation(m_programSphereShader, "projI");
	glm::mat4 pi = glm::inverse( m_camera.GetProj() );
	glUniformMatrix4fv(location, 1, false, &pi[0][0]);

	location = glGetUniformLocation(m_programSphereShader, "time");
	glUniform3f(location, SDL_GetTicks() , m_camera.GetEye().y, m_camera.GetEye().z);

	//TEXT�RA
	glActiveTexture(GL_TEXTURE0);
	// aktiv�ljuk a gener�lt text�r�nkat 
	glBindTexture(GL_TEXTURE_3D, m_suzTexture);

	location = glGetUniformLocation( m_programPointLight, "tex" );


	// text�ra mintav�telez� �s shader-beli sampler2D �sszerendel�se
	glUniform1i(	location,	// ezen azonos�t�j� sampler 2D
					0);		

	// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
	glBindVertexArray(m_vaoID);

	// kirajzol�s
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram( 0 );

}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	switch ( key.keysym.sym )
	{
	case SDLK_w:
			m_camera.UpdateFw(1);
		break;
	case SDLK_s:
			m_camera.UpdateFw(-1);
		break;
	case SDLK_a:
			m_camera.UpdateRight(-1);
		break;
	case SDLK_d:
			m_camera.UpdateRight(1);
		break;
	}
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
	switch ( key.keysym.sym )
	{
	case SDLK_w:
			m_camera.UpdateFw(0);
		break;
	case SDLK_s:
			m_camera.UpdateFw(0);
		break;
	case SDLK_a:
			m_camera.UpdateRight(0);
		break;
	case SDLK_d:
			m_camera.UpdateRight(0);
		break;
	}
}

void CMyApp::Mouse(SDL_MouseMotionEvent& mouse)
{
	if ( mouse.state & SDL_BUTTON_LMASK )
	{
		m_camera.UpdateUV(mouse.xrel/100.0f, mouse.yrel/100.0f);
	}
}

void CMyApp::Resize(SDL_ResizeEvent& ev)
{
	glViewport(0, 0, ev.w, ev.h );
	
	m_camera.Resize(ev.w, ev.h);
}