#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "animation.h"
#include "bone.h"

class Animator
{
public:
	Animator(Animation* animation);

	void UpdateAnimation(float dt, AssimpNodeData* joint);
	void PlayAnimation(Animation* pAnimation);
	void CalculateBoneTransform(AssimpNodeData* node, glm::mat4 parentTransform);
	std::vector<glm::mat4> GetFinalBoneMatrices();
	Animation* GetCurrentAnimation();

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
};

