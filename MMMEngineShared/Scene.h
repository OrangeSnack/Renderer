#pragma once
#include "Export.h"
#include "Object.h"
#include "MUID.h"
#include "rttr/type"
#include "rttr/registration_friend.h"

namespace MMMEngine
{
	class GameObject;
	class MMMENGINE_API Scene final
	{
	private:
		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
		Utility::MUID m_muid;
		std::vector<ObjPtr<GameObject>> m_gameObjects;
		void SetMUID(const Utility::MUID& muid);
	public:
		const Utility::MUID& GetMUID();
	};
}