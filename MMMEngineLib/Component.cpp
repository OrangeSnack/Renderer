#include "Component.h"
#include "GameObject.h"

void MMMEngine::Component::Dispose()
{
	if(GetGameObject().IsValid() && !GetGameObject()->IsDestroyed())
		GetGameObject()->UnRegisterComponent(SelfPtr(this));
	UnInitialize();
}
