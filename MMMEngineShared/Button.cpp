#include "Button.h"
#include "RectTransform.h"
#include "Canvas.h"
#include "Texture2D.h"
#include "rttr/registration"

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace MMMEngine;

	registration::class_<Button>("Button")
		(rttr::metadata("wrapper_type_name", "ObjPtr<Button>"))
		.property("Color", &Button::GetColor, &Button::SetColor)
		.property("Texture", &Button::GetTexture, &Button::SetTexture)
		.property("OnHoverEnter", &Button::GetOnHoverEnter, &Button::SetOnHoverEnter)
		.property("OnHoverStay", &Button::GetOnHoverStay, &Button::SetOnHoverStay)
		.property("OnHoverExit", &Button::GetOnHoverExit, &Button::SetOnHoverExit)
		.property("OnClick", &Button::GetOnClick, &Button::SetOnClick)
		.property("OnRelease", &Button::GetOnRelease, &Button::SetOnRelease);

	registration::class_<ObjPtr<Button>>("ObjPtr<Button>")
		.constructor<>([]() { return Object::NewObject<Button>(); })
		.method("Inject", &ObjPtr<Button>::Inject);
}

static bool PointInRect(float px, float py, float rx, float ry, float rw, float rh)
{
	return px >= rx && px <= rx + rw && py >= ry && py <= ry + rh;
}

void MMMEngine::Button::UpdatePointer(const DirectX::SimpleMath::Vector2& canvasSize,
	const DirectX::SimpleMath::Vector2& pointerInCanvas,
	bool isMouseDown)
{
	auto rectTr = GetRectTransform();
	if (!rectTr.IsValid())
		return;

	auto rect = rectTr->GetRectInCanvas(canvasSize);
	bool inside = PointInRect(pointerInCanvas.x, pointerInCanvas.y, rect.x, rect.y, rect.z, rect.w);

	if (inside)
	{
		if (!m_isHovered)
		{
			m_isHovered = true;
			m_onHoverEnter.Invoke();
		}
		else
			m_onHoverStay.Invoke();

		if (isMouseDown)
		{
			if (!m_isPressed)
			{
				m_isPressed = true;
				m_onClick.Invoke();
			}
		}
		else
		{
			if (m_isPressed)
			{
				m_isPressed = false;
				m_onRelease.Invoke();
			}
		}
	}
	else
	{
		if (m_isHovered)
		{
			m_isHovered = false;
			m_onHoverExit.Invoke();
		}
		if (m_isPressed)
			m_isPressed = false;
	}
}
