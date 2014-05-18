#pragma once

#include <glm/glm.hpp>

class CGLMCamera
{
public:
	CGLMCamera(void);
	CGLMCamera(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up);
	~CGLMCamera(void);

	/// <summary>
	/// Gets the view matrix.
	/// </summary>
	/// <returns>The 4x4 view matrix</returns>
	glm::mat4 GetViewMatrix();

	/// <summary>
	/// Updates the UV.
	/// </summary>
	/// <param name="du">The du, i.e. the change of spherical coordinate u.</param>
	/// <param name="dv">The dv, i.e. the change of spherical coordinate v.</param>
	void UpdateUV(float du, float dv);

	void UpdateRight(float _val);
	void UpdateFw(float _val);

	void Update(float _deltaTime);

	void SetUp(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up);

	void SetSpeed(float _val);

	glm::vec3 GetEye()
	{
		return m_eye;
	}

	glm::vec3 GetAt()
	{
		return m_at;
	}

	glm::vec3 GetUp()
	{
		return m_up;
	}

	glm::mat4 GetProj()
	{
		return m_matProj;
	}

	glm::mat4 GetViewProj()
	{
		return m_matViewProj;
	}

	void Resize(int _w, int _h);
private:
	float		m_speed;
	/// <summary>
	/// The view matrix of the camera
	/// </summary>
	glm::mat4	m_viewMatrix;

	glm::mat4	m_matViewProj;

	/// <summary>
	/// The camera position.
	/// </summary>
	glm::vec3	m_eye;

	/// <summary>
	/// The vector pointing upwards
	/// </summary>
	glm::vec3	m_up;

	/// <summary>
	/// The camera look at point.
	/// </summary>
	glm::vec3	m_at;

	/// <summary>
	/// The u spherical coordinate of the spherical coordinate pair (u,v) denoting the
	/// current viewing direction from the view position m_eye. 
	/// </summary>
	float	m_u;

	/// <summary>
	/// The v spherical coordinate of the spherical coordinate pair (u,v) denoting the
	/// current viewing direction from the view position m_eye. 
	/// </summary>
	float	m_v;

	/// <summary>
	/// The distance of the look at point from the camera. 
	/// </summary>
	float	m_dist;

	/// <summary>
	/// The unit vector pointing towards the viewing direction.
	/// </summary>
	glm::vec3	m_fw;
	/// <summary>
	/// The unit vector pointing to the 'right'
	/// </summary>
	glm::vec3	m_st;

	glm::mat4	m_matProj;

	float	m_goFw;
	float	m_goRight;
};

