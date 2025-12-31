#include <iostream>

#include "App.h"
#include "GUID.h"

using namespace MMMEngine;


void Render() { std::cout << "drawcall" << std::endl; }

int main()
{
	App app;

	Action<> act;
	act += Render;

	act -= Render;

	app.onRender += act;

	app.onRender += Render;

	act.Invoke();

	app.Run();

	MMMEngine::GUID id = MMMEngine::GUID::NewGuid();

	std::cout << "Generated GUID: " << id.ToString() << std::endl;
}