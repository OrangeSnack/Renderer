#include "pch.h"
#include "SkinRenderer.h"

#include "RenderManager.h"
#include "RenderCommand.h"
#include "GameObject.h"
#include "Transform.h"
#include "ShaderInfo.h"
#include "PShader.h"
#include "Material.h"

#include "SkeletalMesh.h"
#include "rttr/registration.h"

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace MMMEngine;

	registration::class_<SkinRenderer>("SkinRenderer")
		(rttr::metadata("wrapper_type_name", "ObjPtr<SkinRenderer>"))
		.property("Mesh", &SkinRenderer::GetMesh, &SkinRenderer::SetMesh)
		//.property("Materials", &SkinRenderer::GetMaterial, &SkinRenderer::SetMaterial)
		.property("CastShadow", &SkinRenderer::GetCastShadow, &SkinRenderer::SetCastShadow)
		.property("ReceiveShadow", &SkinRenderer::GetReceiveShadow, &SkinRenderer::SetReceiveShadow);

	registration::class_<ObjPtr<SkinRenderer>>("ObjPtr<SkinRenderer>")
		.constructor<>(
			[]() {
				return Object::NewObject<SkinRenderer>();
			})
		.method("Inject", &ObjPtr<SkinRenderer>::Inject);
}

namespace MMMEngine {
	void SkinRenderer::SetMesh(ResPtr<SkeletalMesh>& _mesh)
	{
		mesh = _mesh;
	}

	bool SkinRenderer::GetCastShadow()
	{
		if (!mesh)
			return false;

		return castShadows;
	}

	void SkinRenderer::SetCastShadow(bool _val)
	{
		if (!mesh)
			return;

		castShadows = _val;
	}

	void SkinRenderer::SetReceiveShadow(bool _val)
	{
		if (!mesh)
			return;

		receiveShadows = _val;
	}

	bool SkinRenderer::GetReceiveShadow()
	{
		if (!mesh)
			return false;

		return receiveShadows;
	}

	void SkinRenderer::Initialize()
	{
		renderIndex = RenderManager::Get().AddRenderer(this);
	}

	void SkinRenderer::UnInitialize()
	{
		RenderManager::Get().RemoveRenderer(renderIndex);
	}

	void SkinRenderer::Init()
	{

	}

	void SkinRenderer::Render()
	{
		// 유효성 확인
		if (!mesh || !GetTransform())
			return;

		for (auto& [matIdx, meshIndices] : mesh->meshGroupData) {
			if (mesh->materials.empty())
				continue;

			auto& material = mesh->materials[matIdx];

			if (!material)
				continue;

			for (const auto& idx : meshIndices) {
				RenderCommand command;
				auto& meshBuffer = mesh->gpuBuffer.vertexBuffers[idx];
				auto& indicesBuffer = mesh->gpuBuffer.indexBuffers[idx];

				command.vertexBuffer = meshBuffer.Get();
				command.indexBuffer = indicesBuffer.Get();
				command.material = material;
				command.worldMatIndex = RenderManager::Get().AddMatrix(GetTransform()->GetWorldMatrix());
				command.indiciesSize = mesh->indexSizes[idx];
				command.rendererID = renderIndex;
				command.castShadow = castShadows;
				command.receiveShadow = receiveShadows;
				command.offsetBuffer = &mesh->offsetBuffer;

				// TODO:: 임시!! 애니메이션 계산후 바꿔쳐야함!! (기본값)
				static Mesh_BoneBuffer dummy;
				command.animBuffer = &dummy;

				// TODO::TransCulant일시 CamDistance 보내줘야함!!
				command.camDistance = 0.0f;

				std::wstring shaderPath = material->GetPShader()->GetFilePath();
				RenderType type = ShaderInfo::Get().GetRenderType(shaderPath);

				RenderManager::Get().AddCommand(type, std::move(command));
			}
		}
	}
}
