#include "AnimationCurveEditor.h"
#include <algorithm>
#include <limits>

namespace MMMEngine::Editor
{
	namespace
	{
		AnimationCurveEditorView AutoFitViewFromCurve(const AnimationCurve& curve, const AnimationCurveEditorView& base)
		{
			AnimationCurveEditorView view = base;
			if (!view.autoFit)
				return view;

			const auto& keys = curve.GetKeyframes();
			if (keys.empty())
				return view;

			float minTime = keys.front().time;
			float maxTime = keys.front().time;
			float minValue = keys.front().value;
			float maxValue = keys.front().value;

			for (const auto& kf : keys)
			{
				minTime = std::min(minTime, kf.time);
				maxTime = std::max(maxTime, kf.time);
				minValue = std::min(minValue, kf.value);
				maxValue = std::max(maxValue, kf.value);
			}

			if (minTime == maxTime)
			{
				minTime -= 0.5f;
				maxTime += 0.5f;
			}
			if (minValue == maxValue)
			{
				minValue -= 0.5f;
				maxValue += 0.5f;
			}

			view.min = ImVec2(minTime, minValue);
			view.max = ImVec2(maxTime, maxValue);
			return view;
		}

		static void SortKeyframes(std::vector<CurveKeyframe>& keyframes)
		{
			std::sort(keyframes.begin(), keyframes.end(),
				[](const CurveKeyframe& a, const CurveKeyframe& b)
				{
					return a.time < b.time;
				});
		}
	}

	void AnimationCurveEditorContext::Bind(AnimationCurve* curve)
	{
		if (mCurve == curve)
			return;

		mCurve = curve;
		SyncFromCurve();
	}

	bool AnimationCurveEditorContext::IsBound() const
	{
		return mCurve != nullptr;
	}

	void AnimationCurveEditorContext::SetView(const AnimationCurveEditorView& view)
	{
		mView = view;
	}

	AnimationCurveEditorView& AnimationCurveEditorContext::GetView()
	{
		return mView;
	}

	void AnimationCurveEditorContext::SyncFromCurve()
	{
		mKeyframes.clear();
		mPoints.clear();
		if (!mCurve)
			return;

		for (const auto& kf : mCurve->GetKeyframes())
		{
			mKeyframes.push_back(kf);
			mPoints.emplace_back(kf.time, kf.value);
		}
	}

	void AnimationCurveEditorContext::ApplyToCurve()
	{
		if (!mCurve)
			return;

		SortKeyframes(mKeyframes);
		mCurve->SetKeyframes(mKeyframes);
		SyncFromCurve();
	}

	bool AnimationCurveEditorContext::IsDirty() const
	{
		return mDirty;
	}

	void AnimationCurveEditorContext::ClearDirty()
	{
		mDirty = false;
	}

	size_t AnimationCurveEditorContext::GetCurveCount()
	{
		return 1;
	}

	ImCurveEdit::CurveType AnimationCurveEditorContext::GetCurveType(size_t /*curveIndex*/) const
	{
		return ImCurveEdit::CurveSmooth;
	}

	ImVec2& AnimationCurveEditorContext::GetMin()
	{
		return mView.min;
	}

	ImVec2& AnimationCurveEditorContext::GetMax()
	{
		return mView.max;
	}

	size_t AnimationCurveEditorContext::GetPointCount(size_t /*curveIndex*/)
	{
		return mPoints.size();
	}

	uint32_t AnimationCurveEditorContext::GetCurveColor(size_t /*curveIndex*/)
	{
		return 0xFF4AC7FF;
	}

	ImVec2* AnimationCurveEditorContext::GetPoints(size_t /*curveIndex*/)
	{
		return mPoints.empty() ? nullptr : mPoints.data();
	}

	int AnimationCurveEditorContext::EditPoint(size_t /*curveIndex*/, int pointIndex, ImVec2 value)
	{
		if (pointIndex < 0 || static_cast<size_t>(pointIndex) >= mPoints.size())
			return pointIndex;

		if (mView.clampTime)
		{
			value.x = std::clamp(value.x, mView.min.x, mView.max.x);
		}
		if (mView.clampValue)
		{
			value.y = std::clamp(value.y, mView.min.y, mView.max.y);
		}

		const size_t idx = static_cast<size_t>(pointIndex);
		mPoints[idx] = value;
		if (idx < mKeyframes.size())
		{
			mKeyframes[idx].time = value.x;
			mKeyframes[idx].value = value.y;
		}
		mDirty = true;
		return pointIndex;
	}

	void AnimationCurveEditorContext::AddPoint(size_t /*curveIndex*/, ImVec2 value)
	{
		if (mView.clampTime)
		{
			value.x = std::clamp(value.x, mView.min.x, mView.max.x);
		}
		if (mView.clampValue)
		{
			value.y = std::clamp(value.y, mView.min.y, mView.max.y);
		}

		mKeyframes.emplace_back(value.x, value.y, 0.0f, 0.0f, 1);
		mPoints.emplace_back(value.x, value.y);
		mDirty = true;
	}

	void AnimationCurveEditorContext::BeginEdit(int /*index*/)
	{
		mIsEditing = true;
	}

	void AnimationCurveEditorContext::EndEdit()
	{
		mIsEditing = false;
	}

	bool DrawAnimationCurvePreview(const AnimationCurve& curve, const ImVec2& size, const AnimationCurveEditorView& view)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 cursor = ImGui::GetCursorScreenPos();
		ImVec2 end = ImVec2(cursor.x + size.x, cursor.y + size.y);
		ImVec2 rectMin = cursor;
		ImVec2 rectMax = end;

		const ImU32 border = ImGui::GetColorU32(ImGuiCol_Border);
		const ImU32 bg = ImGui::GetColorU32(ImGuiCol_FrameBg);
		drawList->AddRectFilled(rectMin, rectMax, bg, 4.0f);
		drawList->AddRect(rectMin, rectMax, border, 4.0f);

		AnimationCurveEditorView fitted = AutoFitViewFromCurve(curve, view);
		const float width = rectMax.x - rectMin.x;
		const float height = rectMax.y - rectMin.y;

		auto toScreen = [&](float time, float value)
		{
			const float t = (time - fitted.min.x) / (fitted.max.x - fitted.min.x);
			const float v = (value - fitted.min.y) / (fitted.max.y - fitted.min.y);
			const float x = rectMin.x + t * width;
			const float y = rectMax.y - v * height;
			return ImVec2(x, y);
		};

		const auto& keys = curve.GetKeyframes();
		if (keys.empty())
		{
			ImGui::SetCursorScreenPos(ImVec2(cursor.x + 6.0f, cursor.y + 6.0f));
			ImGui::TextUnformatted("Empty");
		}
		else
		{
			const int samples = 64;
			const float minTime = fitted.min.x;
			const float maxTime = fitted.max.x;
			const float dt = (maxTime - minTime) / static_cast<float>(samples - 1);
			ImU32 lineColor = ImGui::GetColorU32(ImGuiCol_PlotLines);

			ImVec2 prev = toScreen(minTime, curve.Evaluate(minTime));
			for (int i = 1; i < samples; ++i)
			{
				const float t = minTime + dt * static_cast<float>(i);
				const float v = curve.Evaluate(t);
				ImVec2 curr = toScreen(t, v);
				drawList->AddLine(prev, curr, lineColor, 1.5f);
				prev = curr;
			}
		}

		ImGui::InvisibleButton("##curve_preview", size);
		return ImGui::IsItemClicked();
	}

	bool DrawAnimationCurveEditor(AnimationCurve& curve, AnimationCurveEditorContext& context, const ImVec2& size)
	{
		context.Bind(&curve);

		AnimationCurveEditorView fitted = AutoFitViewFromCurve(curve, context.GetView());
		context.SetView(fitted);
		context.SyncFromCurve();

    ImCurveEdit::Edit(context, size, ImGui::GetID(&curve));

		if (context.IsDirty())
		{
			context.ApplyToCurve();
			context.ClearDirty();
			return true;
		}

		return false;
	}
}
