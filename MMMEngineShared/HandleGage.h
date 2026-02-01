#pragma once

#include "Export.h"
#include "Graphic.h"
#include "Gage.h"
#include "ResourceManager.h"
#include "SerializableEvent.h"
#include "SimpleMath.h"

namespace MMMEngine
{
	class Texture2D;

	/// 핸들 게이지 UI. Graphic 직접 상속. 드래그로 value 조절 + OnValueChanged.
	class MMMENGINE_API HandleGage : public Graphic
	{
	private:
		RTTR_ENABLE(Graphic)
		RTTR_REGISTRATION_FRIEND

		ResPtr<Texture2D> m_backgroundTexture = nullptr;
		ResPtr<Texture2D> m_fillTexture = nullptr;
		float m_value = 1.0f;
		GageFillDirection m_fillDirection = GageFillDirection::LeftToRight;
		bool m_isDragging = false;
		SerializableEventT<float> m_onValueChanged;

	public:
		HandleGage() = default;
		virtual ~HandleGage() = default;

		const ResPtr<Texture2D>& GetBackgroundTexture() const { return m_backgroundTexture; }
		void SetBackgroundTexture(const ResPtr<Texture2D>& tex) { m_backgroundTexture = tex; }

		const ResPtr<Texture2D>& GetFillTexture() const { return m_fillTexture; }
		void SetFillTexture(const ResPtr<Texture2D>& tex) { m_fillTexture = tex; }

		float GetValue() const { return m_value; }
		void SetValue(float v);

		GageFillDirection GetFillDirection() const { return m_fillDirection; }
		void SetFillDirection(GageFillDirection dir) { m_fillDirection = dir; }

		const SerializableEventT<float>& GetOnValueChanged() const { return m_onValueChanged; }
		void SetOnValueChanged(const SerializableEventT<float>& ev) { m_onValueChanged = ev; }
		SerializableEventT<float>& OnValueChanged() { return m_onValueChanged; }

		void RenderUI(RenderManager& renderer) override;

		void UpdatePointer(const DirectX::SimpleMath::Vector2& canvasSize,
			const DirectX::SimpleMath::Vector2& pointerInCanvas,
			bool isMouseDown);
	};
}
