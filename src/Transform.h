#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>

class Transform {
public:
	glm::vec3 position;
	glm::quat orientation;
	glm::vec3 scale;

	/// <summary>
	/// If the transform is a child of another transform then it is expressed in local space. 
	/// This means all transformations are applied in local space too.
	/// If this is null then the transform is in global space.
	/// </summary>
	Transform *parent;

	Transform();
	Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);

	/// <summary>
	/// Sets the current position to a set of euler angles
	/// </summary>
	/// <param name="eulerAngles">Euler angles in the form (pitch, yaw, roll)</param>
	void SetEulerAngles(glm::vec3 eulerAngles);

	/// <summary>
	/// Gives the current rotation as euler angles.
	/// </summary>
	/// <returns>Euler angles in the form (pitch, yaw, roll)</returns>
	glm::vec3 GetEulerAngles();

	/// <summary>
	/// Rotates the transform by the euler angles given.
	/// </summary>
	/// <param name="eulerAngles">Euler angles (pitch, yaw, roll)</param>
	void Rotate(glm::vec3 eulerAngles);

	/// <summary>
	/// Rotates the transform by the euler angles given.
	/// </summary>
	/// <param name="pitch">Pitch to rotate</param>
	/// <param name="yaw">Yaw to rotate</param>
	/// <param name="roll">Roll to rotate</param>
	void Rotate(float pitch, float yaw, float roll);


	/// <summary>
	/// Translates the transform by the vector given.
	/// </summary>
	/// <param name="vector">The amount to move</param>
	void Translate(glm::vec3 vector);

	/// <summary>
	/// Translates the transform relative to the direction it is facing. Positive x is right.
	/// </summary>
	/// <param name="vector">The amount to move</param>
	void Move(glm::vec3 vector);

	/// <summary>
	/// Get the world space position of a point relative to the tranform
	/// </summary>
	/// <param name="pos"></param>
	/// <returns></returns>
	glm::vec3 GetWorldPosition(glm::vec3 pos=glm::vec3(0));

	glm::mat4 GetPositionMatrix();
	glm::mat4 GetModelMatrix();
};