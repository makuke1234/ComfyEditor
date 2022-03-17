#pragma once

#include <concepts>
#include <cstdint>

namespace ce::concepts
{
	template<typename T>
	concept numeric = !std::is_class_v<T> && (std::is_integral_v<T> || std::is_floating_point_v<T>);

	template<typename T>
	concept signed_integral_until32 = std::signed_integral<T> && !std::is_same_v<T, std::int64_t>;

	template<typename T>
	concept int64_t = std::is_same_v<T, std::int64_t>;

	template<typename T>
	concept unsigned_integral_until32 = std::unsigned_integral<T> && !std::is_same_v<T, std::uint64_t>;

	template<typename T>
	concept uint64_t = std::is_same_v<T, std::uint64_t>;
}
