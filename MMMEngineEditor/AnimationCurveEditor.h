#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "AnimationCurve.h"

namespace MMMEngine::Editor
{
	/// 그래프 뷰포트: X = 시간, Y = 값
	struct AnimationCurveEditorView
	{
		ImVec2 min = ImVec2(0.0f, 0.0f);
		ImVec2 max = ImVec2(1.0f, 1.0f);
		bool autoFit = true;
		bool lockAspect = false;
	};

	/// 인스펙터용 미리보기. 클릭 시 true 반환.
	bool DrawAnimationCurvePreview(AnimationCurve& curve, const ImVec2& size, const AnimationCurveEditorView& view, bool* outChanged = nullptr);

	/// 커브 편집기 창 내부: 시간(X) vs 값(Y) 2D 그래프만 그림. view는 표시 범위(수정됨).
	void DrawAnimationCurveGraph(AnimationCurve& curve, const ImVec2& size, AnimationCurveEditorView& view);
}
