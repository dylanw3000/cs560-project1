#include "bone.h"
#include <glm\gtx\transform.hpp>
// #include <glm/glm.hpp>

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
	: m_Name(name),	m_ID(ID), m_LocalTransform(1.0f)
{
	m_NumPositions = channel->mNumPositionKeys;

	for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
	{
		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = channel->mPositionKeys[positionIndex].mTime;
		KeyPosition data;
		data.position = AssimpInterpret::GetGLMVec(aiPosition);
		data.timeStamp = timeStamp;
		m_Positions.push_back(data);
	}

	m_NumRotations = channel->mNumRotationKeys;
	for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
	{
		aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
		float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
		KeyQuaternion data;
		data.orientation = AssimpInterpret::InterpretQuat(aiOrientation);
		data.timeStamp = timeStamp;
		m_Rotations.push_back(data);
	}
}

/***********************/

void Bone::Update(float animationTime)
{
	glm::mat4 translation = InterpolatePosition(animationTime);
	glm::mat4 rotation = InterpolateRotation(animationTime);
	m_LocalTransform = translation * rotation;
}

/**********************/

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float keyTime = animationTime - lastTimeStamp;
	float keyDuration = nextTimeStamp - lastTimeStamp;
	return keyTime / keyDuration;
}

glm::mat4 Bone::InterpolatePosition(float animationTime)
{
	if (1 == m_NumPositions)
		return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

	int p0Index = GetPositionIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
		m_Positions[p1Index].timeStamp, animationTime);
	glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position
		, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

// Make our "quaternion" (a vec4) into a 4x4 matrix for rendering
glm::mat4 Bone::quatToMat(glm::vec4 q)
{
	glm::mat4 Result(1.0f);

	float x = q.x;
	float y = q.y;
	float z = q.z;
	float w = q.w;

	Result[0][0] = 1.f - 2.f * (y * y + z * z);
	Result[0][1] = 2.f * (x * y + w * z);
	Result[0][2] = 2.f * (x * z - w * y);

	Result[1][0] = 2.f * (x * y - w * z);
	Result[1][1] = 1.f - 2.f * (x * x + z * z);
	Result[1][2] = 2.f * (y * z + w * x);

	Result[2][0] = 2.f * (x * z + w * y);
	Result[2][1] = 2.f * (y * z - w * x);
	Result[2][2] = 1.f - 2.f * (x * x + y * y);
	return Result;
}

// Slerp
glm::vec4 Bone::slerp(glm::vec4 p0, glm::vec4 p1, float t) {
	float alpha = glm::dot(p0, p1);

	// If it would go the inverse direction
	if (alpha < 0.f)
	{
		p1 = -p1;
		alpha = -alpha;
	}

	// If approaching a singularity, like division by zero
	if (alpha > 0.995f)
	{
		return glm::vec4(
			glm::mix(p0.x, p1.x, t),
			glm::mix(p0.y, p1.y, t),
			glm::mix(p0.z, p1.z, t),
			glm::mix(p0.w, p1.w, t)
		);
	}
	else
	{
		float angle = acos(alpha);
		return (sin((1.f - t) * angle) * p0 + sin(t * angle) * p1) / sin(angle);
	}
}

glm::mat4 Bone::InterpolateRotation(float animationTime)
{
	if (1 == m_NumRotations)
	{
		auto rotation = glm::normalize(m_Rotations[0].orientation);
		return quatToMat(rotation);
	}

	int p0Index = GetRotationIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
		m_Rotations[p1Index].timeStamp, animationTime);
	// this is where the quaternion math happens
	glm::vec4 finalRotation = slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);

	// return glm::mat4(1.0f);
	return quatToMat(finalRotation);

}
