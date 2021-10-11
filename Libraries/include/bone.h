#pragma once

/* Container for bone data */

#include <vector>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "assimpInterpret.h"

struct KeyPosition
{
	glm::vec3 position;
	float timeStamp;
};

struct KeyQuaternion
{
	// this vec4 represents a quaternion
	glm::vec4 orientation;
	float timeStamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};

class Bone
{
public:
	Bone(const std::string& name, int ID, const aiNodeAnim* channel);
	
	void Update(float animationTime);

	glm::mat4 GetLocalTransform() { return m_LocalTransform; }
	std::string GetBoneName() const { return m_Name; }
	int GetBoneID() { return m_ID; }
	


	int GetPositionIndex(float animationTime)
	{
		for (int index = 0; index < m_NumPositions - 1; ++index)
		{
			if (animationTime < m_Positions[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}

	int GetRotationIndex(float animationTime)
	{
		for (int index = 0; index < m_NumRotations - 1; ++index)
		{
			if (animationTime < m_Rotations[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}


private:

	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
	glm::mat4 InterpolatePosition(float animationTime);
	glm::mat4 quatToMat(glm::vec4 q);
	glm::vec4 slerp(glm::vec4 p0, glm::vec4 p1, float t);
	glm::mat4 InterpolateRotation(float animationTime);

	std::vector<KeyPosition> m_Positions;
	std::vector<KeyQuaternion> m_Rotations;
	std::vector<KeyScale> m_Scales;
	int m_NumPositions;
	int m_NumRotations;

	glm::mat4 m_LocalTransform;
	std::string m_Name;
	int m_ID;
};

