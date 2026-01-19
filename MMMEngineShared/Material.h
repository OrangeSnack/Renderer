#pragma once

#include "Export.h"
#include "Resource.h"
#include <variant>
#include <SimpleMath.h>
#include <wrl/client.h>
#include <d3d11_4.h>
#include "Texture2D.h"
#include <filesystem>
#include "ResourceManager.h"

#include "json/json.hpp"

namespace MMMEngine {
	using PropertyValue = std::variant<
		int, float, DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Matrix,
		ResPtr<MMMEngine::Texture2D>
	>;

	class PShader;
	class VShader;
	class MaterialSerealizer;
	class MMMENGINE_API Material : public Resource
	{
		friend class MaterialSerealizer;
	private:
		std::unordered_map<std::wstring, PropertyValue> m_properties;
		ResPtr<VShader> m_pVShader;
		ResPtr<PShader> m_pPShader;

		std::wstring name = L"Material";

	public:
		std::wstring GetName() const { return name; }
		void SetName(std::wstring _name) { name = _name; }

		void SetProperty(const std::wstring& _name, const PropertyValue& value);
		PropertyValue GetProperty(const std::wstring& name) const;
		const std::unordered_map<std::wstring, PropertyValue>& GetProperties() { return m_properties; }

		void SetVShader(const std::wstring& _filePath);
		void SetPShader(const std::wstring& _filePath);
		ResPtr<VShader> GetVShader() const;
		ResPtr<PShader> GetPShader() const;

		void LoadTexture(const std::wstring& _name, const std::wstring& _filePath);

		bool LoadFromFilePath(const std::wstring& _filePath) override;
	};
}


