#pragma once
#include "Core/Types.h"
#include <filesystem>
#include <optional>

namespace fw
{
	using IDType = u64;
	static constexpr IDType NullId = (IDType)-1;

	class ContentManager
	{
	public:
		static void Create();
		static void Destroy();
		static ContentManager* Get();

		IDType Register(const std::string& path);
		std::optional<std::filesystem::path> GetPath(IDType id);


	private:
		static ContentManager* s_Instance;

		std::unordered_map<IDType, std::filesystem::path> m_Paths;
	};
}