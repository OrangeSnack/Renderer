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
		.property("HandleTexture", &HandleGage::GetHandleTexture, &HandleGage::SetHandleTexture)
		.property("HandleSize", &HandleGage::GetHandleSize, &HandleGage::SetHandleSize)
		.property("HandleOffset", &HandleGage::GetHandleOffset, &HandleGage::SetHandleOffset)
		.property("HandlePivot", &HandleGage::GetHandlePivot, &HandleGage::SetHandlePivot)
		.property("HandleClampPadding", &HandleGage::GetHandleClampPadding, &HandleGage::SetHandleClampPadding)
		.property("HideHandleWhenZero", &HandleGage::GetHideHandleWhenZero, &HandleGage::SetHideHandleWhenZero)
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

static DirectX::SimpleMath::Vector2 ComputeHandleSize(const MMMEngine::HandleGage& gage,
	const DirectX::SimpleMath::Vector4& rectCanvas)
{
	using namespace DirectX::SimpleMath;
	using MMMEngine::GageFillDirection;

	const bool horizontal =
		(gage.GetFillDirection() == GageFillDirection::LeftToRight) ||
		(gage.GetFillDirection() == GageFillDirection::RightToLeft);

	Vector2 handleSize = gage.GetHandleSize();
	const float thickness = horizontal ? rectCanvas.w : rectCanvas.z;
	if (handleSize.x <= 0.0f)
		handleSize.x = thickness;
	if (handleSize.y <= 0.0f)
		handleSize.y = thickness;
	return handleSize;
}

static DirectX::SimpleMath::Vector4 ComputeHandleRectCanvas(const MMMEngine::HandleGage& gage,
	const DirectX::SimpleMath::Vector4& rectCanvas,
	float value)
{
	using namespace DirectX::SimpleMath;
	using MMMEngine::GageFillDirection;

	float v = (value < 0.0f) ? 0.0f : (value > 1.0f) ? 1.0f : value;
	const bool horizontal =
		(gage.GetFillDirection() == GageFillDirection::LeftToRight) ||
		(gage.GetFillDirection() == GageFillDirection::RightToLeft);

	const Vector2 handleSize = ComputeHandleSize(gage, rectCanvas);
	const Vector2 pivot = gage.GetHandlePivot();
	const Vector2 offset = gage.GetHandleOffset();
	const Vector2 clampPad = gage.GetHandleClampPadding();

	Vector4 handleRect = {};
	handleRect.z = handleSize.x;
	handleRect.w = handleSize.y;

	if (horizontal)
	{
		const float minPivotX = rectCanvas.x + clampPad.x;
		float maxPivotX = rectCanvas.x + rectCanvas.z - clampPad.y;
		if (maxPivotX < minPivotX)
			maxPivotX = minPivotX;

		float t = v;
		if (gage.GetFillDirection() == GageFillDirection::RightToLeft)
			t = 1.0f - v;

		const float pivotX = minPivotX + (maxPivotX - minPivotX) * t;
		handleRect.x = pivotX - handleSize.x * pivot.x + offset.x;

		const float centerY = rectCanvas.y + rectCanvas.w * 0.5f;
		handleRect.y = centerY - handleSize.y * pivot.y + offset.y;
	}
	else
	{
		const float minPivotY = rectCanvas.y + clampPad.x;
		float maxPivotY = rectCanvas.y + rectCanvas.w - clampPad.y;
		if (maxPivotY < minPivotY)
			maxPivotY = minPivotY;

		float t = v;
		if (gage.GetFillDirection() == GageFillDirection::BottomToTop)
			t = 1.0f - v;

		const float pivotY = minPivotY + (maxPivotY - minPivotY) * t;
		handleRect.y = pivotY - handleSize.y * pivot.y + offset.y;

		const float centerX = rectCanvas.x + rectCanvas.z * 0.5f;
		handleRect.x = centerX - handleSize.x * pivot.x + offset.x;
	}
	return handleRect;
}

void MMMEngine::HandleGage::RenderUI(RenderManager& renderer)
{
	if (!GetCanvas().IsValid())
		return;

	auto rectTransform = GetRectTransform();
	if (!rectTransform.IsValid())
		return;

	auto canvasSize = GetCanvas()->GetCanvasSize();
	auto rectCanvas = rectTransform->GetRectInCanvas(canvasSize);

	auto scale = GetCanvas()->GetScaleToScene();
	Vector4 rect = rectCanvas;
	rect.x *= scale.x;
	rect.y *= scale.y;
	rect.z *= scale.x;
	rect.w *= scale.y;

	renderer.DrawUIElement(rect, { 0.0f, 0.0f, 1.0f, 1.0f }, GetColor(),
		m_backgroundTexture ? m_backgroundTexture : m_fillTexture);

	float v = (m_value < 0.0f) ? 0.0f : (m_value > 1.0f) ? 1.0f : m_value;
	if (v > 0.0f && m_fillTexture)
	{
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

	if (!m_handleTexture)
		return;
	if (m_hideHandleWhenZero && v <= 0.0f)
		return;

	Vector4 handleRectCanvas = ComputeHandleRectCanvas(*this, rectCanvas, v);
	Vector4 handleRect = handleRectCanvas;
	handleRect.x *= scale.x;
	handleRect.y *= scale.y;
	handleRect.z *= scale.x;
	handleRect.w *= scale.y;

	renderer.DrawUIElement(handleRect, { 0.0f, 0.0f, 1.0f, 1.0f }, GetColor(), m_handleTexture);
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
	if (!inside && m_handleTexture && !(m_hideHandleWhenZero && GetValue() <= 0.0f))
	{
		auto handleRect = ComputeHandleRectCanvas(*this, rect, GetValue());
		inside = PointInRect(pointerInCanvas.x, pointerInCanvas.y,
			handleRect.x, handleRect.y, handleRect.z, handleRect.w);
	}

	if (isMouseDown && inside)
		m_isDragging = true;
	if (!isMouseDown)
		m_isDragging = false;

	if (!m_isDragging)
		return;

	float newValue = GetValue();
	const bool horizontal =
		(GetFillDirection() == GageFillDirection::LeftToRight) ||
		(GetFillDirection() == GageFillDirection::RightToLeft);

	if (m_handleTexture)
	{
		const auto handleSize = ComputeHandleSize(*this, rect);
		const auto pivot = GetHandlePivot();
		const auto offset = GetHandleOffset();
		const auto clampPad = GetHandleClampPadding();

		if (horizontal)
		{
			float minPivotX = rect.x + clampPad.x;
			float maxPivotX = rect.x + rect.z - clampPad.y;
			if (maxPivotX < minPivotX)
				maxPivotX = minPivotX;

			const float denom = maxPivotX - minPivotX;
			if (denom > 0.0f)
			{
				const float pivotX = pointerInCanvas.x - offset.x;
				newValue = (pivotX - minPivotX) / denom;
			}
			else
			{
				newValue = 0.0f;
			}
		}
		else
		{
			float minPivotY = rect.y + clampPad.x;
			float maxPivotY = rect.y + rect.w - clampPad.y;
			if (maxPivotY < minPivotY)
				maxPivotY = minPivotY;

			const float denom = maxPivotY - minPivotY;
			if (denom > 0.0f)
			{
				const float pivotY = pointerInCanvas.y - offset.y;
				newValue = (pivotY - minPivotY) / denom;
			}
			else
			{
				newValue = 0.0f;
			}
		}
	}
	else
	{
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
