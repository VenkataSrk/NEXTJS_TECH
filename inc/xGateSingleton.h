#pragma once

#include <memory>

// Mayers Singleton class template
template <typename T>
class XGSingleton
{
	public:
		static T &instance();
		XGSingleton(const T &) = delete;
		XGSingleton &operator=(const T &) = delete;
		XGSingleton(T &&) = delete;
		XGSingleton &operator=(T &&) = delete;

	protected:
		struct token
		{
		};
		XGSingleton() = default;
}; // end of 'XGSingleton' class

template <typename T>
inline T &XGSingleton<T>::instance()
{
	static const std::unique_ptr<T> instance = std::make_unique<T>(token{});
	return *instance;
}
