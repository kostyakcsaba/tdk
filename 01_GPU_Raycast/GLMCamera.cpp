#include "GLMCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

/// <summary>
/// Initializes a new instance of the <see cref="CGLMCamera"/> class.
/// </summary>
CGLMCamera::CGLMCamera(void) : m_eye(0.0f, 20.0f, 20.0f), m_at(0.0f), m_up(0.0f, 1.0f, 0.0f), m_speed(1)
{
	SetUp( glm::vec3(0,5,5), glm::vec3(0,0,0), glm::vec3(0,1,0));

	m_goFw = m_goRight = 0;

	m_dist = glm::length( m_at - m_eye );	
}

CGLMCamera::CGLMCamera(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up)
{
	m_speed = 1;
	SetUp(_eye, _at, _up);
	m_goFw = m_goRight = 0;
	m_dist = 10;
}

void CGLMCamera::SetUp(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up)
{
	m_eye	= _eye;
	m_at	= _at;
	m_up	= _up;

	m_fw  = glm::normalize( m_at - m_eye  );
	m_st = glm::normalize( glm::cross( m_fw, m_up ) );

	m_dist = glm::length( m_at - m_eye );	

	m_u = atan2f( m_fw.z, m_fw.x );
	m_v = acosf( m_fw.y );
}

CGLMCamera::~CGLMCamera(void)
{
}

glm::mat4 CGLMCamera::GetViewMatrix()
{
	return m_viewMatrix;
}

void CGLMCamera::Update(float _deltaTime)
{
	m_eye += (m_goFw*m_fw + m_goRight*m_st)*_deltaTime;
	m_at  += (m_goFw*m_fw + m_goRight*m_st)*_deltaTime;

	m_viewMatrix = glm::lookAt( m_eye, m_at, m_up);
	m_matViewProj = m_matProj * m_viewMatrix;

}

void CGLMCamera::UpdateFw(float _val)
{
	m_goFw		= _val*m_speed;
}

void CGLMCamera::UpdateRight(float _val)
{
	m_goRight	= _val*m_speed;
}


void CGLMCamera::UpdateUV(float du, float dv)
{
	m_u += du;
	m_v += dv;

	if ( m_v >= 3.1f )
		m_v = 3.1f;
	if ( m_v < 0.1f )
		m_v = 0.1f;

	m_at = m_eye + m_dist*glm::vec3(	cosf(m_u)*sinf(m_v), 
										cosf(m_v), 
										sinf(m_u)*sinf(m_v) );

	m_fw = glm::normalize( m_at - m_eye );
	m_st = glm::normalize( glm::cross( m_fw, m_up ) );
}

void CGLMCamera::SetSpeed(float _val)
{
	m_speed = _val;
}

void CGLMCamera::Resize(int _w, int _h)
{
	m_matProj = glm::perspective(	45.0f,				// 90 fokos nyilasszog
									_w/(float)_h,	// ablakmereteknek megfelelo nezeti arany
									0.001f,				// kozeli vagosik
									1000.0f);			// tavoli vagosik

	m_matViewProj = m_matProj * m_viewMatrix;
}