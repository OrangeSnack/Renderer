#include "HandleGage.h"
#include "RectTransform.h"
#include "Canvas.h"
#include "RenderManager.h"
#include "Texture2D.h"
#include "rttr/registration"

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace MMMEngine;

	registration::class_<HandleGage>("HandleGage")
		(rttr::metadata("wrapper_type_name", "ObjPtr<HandleGage>"))
		.property("Color", &HandleGage::GetColor, &HandleGage::SetColor)
		.property("BackgroundTexture", &HandleGage::GetBackgroundTexture, &HandleGage::SetBackgroundTexture)
		.property("FillTexture", &HandleGage::GetFillTexture, &HandleGage::SetFillTexture)
		.property("Value", &HandleGage::GetValue, &HandleGage::SetValue)
		.property("FillDirection", &HandleGage::GetFillDirection, &HandleGage::SetFillDirection)
		.property("OnValueChanged", &HandleGage::GetOnValueChanged, &HandleGage::SetOnValueChanged);

	registration::class_<ObjPtr<HandleGage>>("ObjPtr<HandleGage>")
		.constructor<>([]() { return Object::NewObject<HandleGage>(); })
		.method("Inject", &ObjPtr<HandleGage>::Inject);
}

void MMMEngine::HandleGage::SetValue(float v)
{
	m_value = (v < 0.0f) ? 0.0f : (v > 1.0f) ? 1.0f : v;
}

void MMMEngine::HandleGage::RenderUI(RenderManager& renderer)
{
	if (!GetCanvas().IsValid())
		return;

	auto rectTransform = GetRectTransform();
	if (!rectTransform.IsValid())
		return;

	auto canvasSize = GetCanvas()->GetCanvasSize();
	auto rect = rectTransform->GetRectInCanvas(canvasSize);

	auto scale = GetCanvas()->GetScaleToScene();
	rect.x *= scale.x;
	rect.y *= scale.y;
	rect.z *= scale.x;
	rect.w *= scale.y;

	renderer.DrawUIElement(rect, { 0.0f, 0.0f, 1.0f, 1.0f }, GetColor(),
		m_backgroundTexture ? m_backgroundTexture : m_fillTexture);

	float v = (m_value < 0.0f) ? 0.0f : (m_value > 1.0f) ? 1.0f : m_value;
	if (v <= 0.0f || !m_fillTexture)
		return;

	using namespace DirectX::SimpleMath;
	Vector4 fillRect = rect;
	Vector4 fillUV = { 0.0f, 0.0f, 1.0f, 1.0f };

	switch (m_fillDirection)
	{
	case GageFillDirection::LeftToRight:
		fillRect.z = rect.z * v;
		fillUV.z = v;
		break;
	case GageFillDirection::RightToLeft:
		fillRect.x = rect.x + rect.z * (1.0f - v);
		fillRect.z = rect.z * v;
		fillUV.x = 1.0f - v;
		fillUV.z = 1.0f;
		break;
	case GageFillDirection::BottomToTop:
		fillRect.y = rect.y + rect.w * (1.0f - v);
		fillRect.w = rect.w * v;
		fillUV.y = 1.0f - v;
		fillUV.w = 1.0f;
		break;
	case GageFillDirection::TopToBottom:
		fillRect.w = rect.w * v;
		fillUV.w = v;
		break;
	}

	renderer.DrawUIElement(fillRect, fillUV, GetColor(), m_fillTexture);
}

static bool PointInRect(float px, float py, float rx, float ry, float rw, float rh)
{
	return px >= rx && px <= rx + rw && py >= ry && py <= ry + rh;
}

void MMMEngine::HandleGage::UpdatePointer(const DirectX::SimpleMath::Vector2& canvasSize,
	const DirectX::SimpleMath::Vector2& pointerInCanvas,
	bool isMouseDown)
{
	auto rectTr = GetRectTransform();
	if (!rectTr.IsValid())
		return;

	auto rect = rectTr->GetRectInCanvas(canvasSize);
	bool inside = PointInRect(pointerInCanvas.x, pointerInCanvas.y, rect.x, rect.y, rect.z, rect.w);

	if (isMouseDown && inside)
		m_isDragging = true;
	if (!isMouseDown)
		m_isDragging = false;

	if (!m_isDragging)
		return;

	float newValue = GetValue();
	switch (GetFillDirection())
	{
	case GageFillDirection::LeftToRight:
	case GageFillDirection::RightToLeft:
		if (rect.z > 0.0f)
			newValue = (pointerInCanvas.x - rect.x) / rect.z;
		break;
	case GageFillDirection::BottomToTop:
	case GageFillDirection::TopToBottom:
		if (rect.w > 0.0f)
			newValue = (pointerInCanvas.y - rect.y) / rect.w;
		break;
	}

	if (newValue < 0.0f) newValue = 0.0f;
	if (newValue > 1.0f) newValue = 1.0f;

	if (GetFillDirection() == GageFillDirection::RightToLeft || GetFillDirection() == GageFillDirection::BottomToTop)
		newValue = 1.0f - newValue;

	if (newValue != GetValue())
	{
		SetValue(newValue);
		m_onValueChanged.Invoke(newValue);
	}
}
