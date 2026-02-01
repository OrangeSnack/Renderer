#include "Graphic.h"
#include "Canvas.h"
#include "RectTransform.h"
#include "RenderManager.h"
#include "rttr/registration"

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace MMMEngine;

	registration::class_<Graphic>("Graphic")
		(rttr::metadata("wrapper_type_name", "ObjPtr<Graphic>"), rttr::metadata("INSPECTOR","DONT_ADD_COMP"))
		.property("RenderOrder", &Graphic::GetRenderOrder, &Graphic::SetRenderOrder);

	registration::class_<ObjPtr<Graphic>>("ObjPtr<Graphic>")
		.constructor<>([]() {
			return Object::NewObject<Graphic>();
		})
		.method("Inject", &ObjPtr<Graphic>::Inject);
}

void MMMEngine::Graphic::Initialize()
{
	Behaviour::Initialize();

	auto tr = GetTransform();
	if (tr.IsValid())
		tr->onUpdateTransformTree.AddListener<Graphic, &Graphic::HandleTransformParentChanged>(this);

	RefreshCanvas(tr.IsValid() ? tr->GetParent() : ObjPtr<Transform>());
}

void MMMEngine::Graphic::UnInitialize()
{
	auto tr = GetTransform();
	if (tr.IsValid())
		tr->onUpdateTransformTree.RemoveListener<Graphic, &Graphic::HandleTransformParentChanged>(this);

	if (m_canvas.IsValid())
		m_canvas->UnregisterGraphic(SelfPtr(this));
	m_canvas = nullptr;

	Behaviour::UnInitialize();
}

void MMMEngine::Graphic::RefreshCanvas(ObjPtr<Transform> newParent)
{
	ObjPtr<Canvas> foundCanvas = nullptr;

	if (auto selfCanvas = GetGameObject()->GetComponent<Canvas>(); selfCanvas.IsValid())
	{
		foundCanvas = selfCanvas;
	}
	else
	{
		for (auto t = newParent; t != nullptr; t = t->GetParent())
		{
			auto go = t->GetGameObject();
			if (!go.IsValid())
				continue;

			if (auto canvas = go->GetComponent<Canvas>(); canvas.IsValid())
			{
				foundCanvas = canvas;
				break;
			}
		}
	}

	if (foundCanvas == m_canvas)
		return;

	if (m_canvas.IsValid())
		m_canvas->UnregisterGraphic(SelfPtr(this));

	m_canvas = foundCanvas;

	if (m_canvas.IsValid())
		m_canvas->RegisterGraphic(SelfPtr(this));
}

void MMMEngine::Graphic::HandleTransformParentChanged(ObjPtr<Transform> newParent)
{
	RefreshCanvas(newParent);
}

MMMEngine::ObjPtr<MMMEngine::RectTransform> MMMEngine::Graphic::GetRectTransform()
{
	auto tr = GetTransform();
	if (!tr.IsValid())
		return nullptr;

	return tr.Cast<RectTransform>();
}

void MMMEngine::Graphic::RenderUI(RenderManager& renderer)
{
	if (!m_canvas.IsValid())
		return;

	auto rectTransform = GetRectTransform();
	if (!rectTransform.IsValid())
		return;

	auto canvasSize = m_canvas->GetCanvasSize();
	auto rect = rectTransform->GetRectInCanvas(canvasSize);

	// 캔버스 좌표를 씬 픽셀로 변환 (ConstantPixelSize=1:1, ScaleWithScreenSize=ref기준 스케일)
	auto scale = m_canvas->GetScaleToScene();
	rect.x *= scale.x;
	rect.y *= scale.y;
	rect.z *= scale.x;
	rect.w *= scale.y;

	renderer.DrawUIElement(rect, GetUVRect(), m_color, m_texture);
}
