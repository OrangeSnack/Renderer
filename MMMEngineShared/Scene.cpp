#include "Scene.h"
#include "rttr/registration"
#include "rttr/detail/policies/ctor_policies.h"

RTTR_REGISTRATION
{
    using namespace rttr;
    using namespace MMMEngine;

    registration::class_<Scene>("Scene")
        .property("MUID", &Scene::GetMUID, &Scene::SetMUID, registration::private_access);
}


void MMMEngine::Scene::SetMUID(const Utility::MUID& muid)
{
    m_muid = muid;
}

const MMMEngine::Utility::MUID & MMMEngine::Scene::GetMUID()
{
    return m_muid;
}
