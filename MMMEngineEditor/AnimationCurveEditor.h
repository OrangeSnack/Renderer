#pragma once
#include <vector>
#include <string>

#include "imgui.h"
#include "ImGuizmo/ImCurveEdit.h"
#include "AnimationCurve.h"

namespace MMMEngine::Editor
{
	struct AnimationCurveEditorView
	{
		ImVec2 min = ImVec2(0.0f, 0.0f);
		ImVec2 max = ImVec2(1.0f, 1.0f);
		bool autoFit = true;
		bool clampTime = false;
		bool clampValue = false;
	};

	class AnimationCurveEditorContext final : public ImCurveEdit::Delegate
	{
	public:
		void Bind(AnimationCurve* curve);
		bool IsBound() const;
		void SetView(const AnimationCurveEditorView& view);
		AnimationCurveEditorView& GetView();

		void SyncFromCurve();
		void ApplyToCurve();

		bool IsDirty() const;
		void ClearDirty();

		size_t GetCurveCount() override;
		ImCurveEdit::CurveType GetCurveType(size_t curveIndex) const override;
		ImVec2& GetMin() override;
		ImVec2& GetMax() override;
		size_t GetPointCount(size_t curveIndex) override;
		uint32_t GetCurveColor(size_t curveIndex) override;
		ImVec2* GetPoints(size_t curveIndex) override;
		int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value) override;
		void AddPoint(size_t curveIndex, ImVec2 value) override;
		void BeginEdit(int index) override;
		void EndEdit() override;

	private:
		AnimationCurve* mCurve = nullptr;
		AnimationCurveEditorView mView;
		std::vector<CurveKeyframe> mKeyframes;
		std::vector<ImVec2> mPoints;
		bool mDirty = false;
		bool mIsEditing = false;
	};

	bool DrawAnimationCurvePreview(const AnimationCurve& curve, const ImVec2& size, const AnimationCurveEditorView& view);
	bool DrawAnimationCurveEditor(AnimationCurve& curve, AnimationCurveEditorContext& context, const ImVec2& size);
}
