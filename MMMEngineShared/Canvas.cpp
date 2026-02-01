#include "Canvas.h"
#include "Graphic.h"
#include "RectTransform.h"
#include "RenderManager.h"
#include "rttr/registration"
#include <algorithm>

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace MMMEngine;

	registration::enumeration<CanvasScaleMode>("CanvasScaleMode")
			(
				rttr::value("ConstantPixelSize", CanvasScaleMode::ConstantPixelSize),
				rttr::value("ScaleWithScreenSize", CanvasScaleMode::ScaleWithScreenSize)
			);

	registration::class_<Canvas>("Canvas")
		(rttr::metadata("wrapper_type_name", "ObjPtr<Canvas>"))
		.property("SortOrder", &Canvas::GetSortOrder, &Canvas::SetSortOrder)
		.property("ScaleMode", &Canvas::GetScaleMode, &Canvas::SetScaleMode)
		.property("ReferenceResolution", &Canvas::GetReferenceResolution, &Canvas::SetReferenceResolution);

	registration::class_<ObjPtr<Canvas>>("ObjPtr<Canvas>")
		.constructor<>([]() {
			return Object::NewObject<Canvas>();
		})
		.method("Inject", &ObjPtr<Canvas>::Inject);
}

void MMMEngine::Canvas::Initialize()
{
	Behaviour::Initialize();
	RenderManager::Get().RegisterCanvas(this);
}

void MMMEngine::Canvas::UnInitialize()
{
	RenderManager::Get().UnRegisterCanvas(this);
	m_graphics.clear();
	Behaviour::UnInitialize();
}

DirectX::SimpleMath::Vector2 MMMEngine::Canvas::GetCanvasSize()
{
	if (m_scaleMode == CanvasScaleMode::ScaleWithScreenSize)
		return m_referenceResolution;

	UINT w = 0, h = 0;
	RenderManager::Get().GetSceneSize(w, h);
	return { static_cast<float>(w), static_cast<float>(h) };
}

DirectX::SimpleMath::Vector2 MMMEngine::Canvas::GetScaleToScene() const
{
	if (m_scaleMode == CanvasScaleMode::ConstantPixelSize)
		return { 1.0f, 1.0f };

	UINT sceneW = 0, sceneH = 0;
	RenderManager::Get().GetSceneSize(sceneW, sceneH);
	if (m_referenceResolution.x <= 0.0f || m_referenceResolution.y <= 0.0f)
		return { 1.0f, 1.0f };
	return {
		static_cast<float>(sceneW) / m_referenceResolution.x,
		static_cast<float>(sceneH) / m_referenceResolution.y
	};
}

void MMMEngine::Canvas::RegisterGraphic(ObjPtr<Graphic> graphic)
{
	if (!graphic)
		return;

	auto it = std::find(m_graphics.begin(), m_graphics.end(), graphic);
	if (it != m_graphics.end())
		return;

	m_graphics.push_back(graphic);
}

void MMMEngine::Canvas::UnregisterGraphic(ObjPtr<Graphic> graphic)
{
	auto it = std::find(m_graphics.begin(), m_graphics.end(), graphic);
	if (it == m_graphics.end())
		return;

	*it = m_graphics.back();
	m_graphics.pop_back();
}

const std::vector<MMMEngine::ObjPtr<MMMEngine::Graphic>>& MMMEngine::Canvas::GetGraphics() const
{
	return m_graphics;
}

void MMMEngine::Canvas::RenderUI(RenderManager& renderer)
{
	if (!IsActiveAndEnabled())
		return;

	for (auto& graphic : m_graphics)
	{
		if (!graphic.IsValid())
			continue;
		if (!graphic->IsActiveAndEnabled())
			continue;
		graphic->RenderUI(renderer);
	}
}
