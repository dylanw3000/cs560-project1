#include "animator.h"

Animator::Animator(Animation* animation)
{
	m_CurrentTime = 0.0;
	m_CurrentAnimation = animation;

	m_FinalBoneMatrices.reserve(100);

	for (int i = 0; i < 100; i++)
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

void Animator::UpdateAnimation(float dt)
{
	m_DeltaTime = dt;

	boneVerts.clear();
	boneInds.clear();
	indNum = 0;

	if (m_CurrentAnimation)
	{
		m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
	// std::cout << boneVerts.size() << std::endl;

	// bind the positions and hierarchy of skeleton for debug view
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, boneVerts.size() * sizeof(float), &boneVerts[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, boneInds.size() * sizeof(GLuint), &boneInds[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void Animator::DrawSkelly() {
	// glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, boneInds.size(), GL_UNSIGNED_INT, 0);
	glPointSize(5);
	glDrawArrays(GL_POINTS, boneVerts[0], boneVerts.size());
	//
	glBindVertexArray(0);
}

void Animator::PlayAnimation(Animation* pAnimation)
{
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

	if (Bone)
	{
		Bone->Update(m_CurrentTime);
		nodeTransform = Bone->GetLocalTransform();
	}

	// current node in world space
	glm::mat4 rootSpaceTransform = parentTransform * nodeTransform;

	// add the root space xyz to the debug view
	glm::vec3 a = rootSpaceTransform * glm::vec4(0, 0, 0, 1);
	boneVerts.push_back(a.x);
	boneVerts.push_back(a.y);
	boneVerts.push_back(a.z);

	auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = rootSpaceTransform * offset;
	}

	GLuint mine = indNum;

	//boneInds.push_back(boneInfoMap[nodeName].id);
	//boneInds.push_back(0);
	for (int i = 0; i < node->childrenCount; i++) {
		boneInds.push_back(mine);
		boneInds.push_back(++indNum);

		CalculateBoneTransform(&node->children[i], rootSpaceTransform);
	}

}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices()
{
	return m_FinalBoneMatrices;
}
