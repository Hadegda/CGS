#ifndef _SINGLETON_HPP_
#define _SINGLETON_HPP_
#include <memory>
#include <vector>

class SingletonBase {
public:
	virtual ~SingletonBase() = default;
};

template <class T>
class Singleton : public SingletonBase {
public:
	Singleton(const Singleton&) = delete;
	void operator=(const Singleton&) = delete;

	static T* Get() {
		static size_t index = SingletonManager::Register<T>();
		return SingletonManager::Get<T>(index);
	}
protected:
	Singleton() = default;
	~Singleton() = default;
};

class SingletonManager {
public:
	static void Shutdown() {
		for (auto& iter = singletons.rbegin(); iter != singletons.rend(); ++iter) {
			delete *iter;
		}
		singletons.clear();
	}
private:
	template<typename T>
	static size_t Register() {
		singletons.push_back(static_cast<SingletonBase*>(new T()));
		return singletons.size() - 1;
	}

	template<typename T>
	static T* Get(size_t index) {
		return static_cast<T*>(singletons[index]);
	}
	static std::vector<SingletonBase*> singletons;

	template<typename T>
	friend class Singleton;
};

#endif