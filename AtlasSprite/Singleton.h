#pragma once

template <typename Class>
class Singleton
{
private:
	static Class* m_inst;
	
public:
	static Class* GetSingletonInstance()
	{
		if (!m_inst)
			m_inst = new Class;
		return m_inst;
	}
	static void ReleaseSingletonInstance()
	{
		if (m_inst)
		{
			delete m_inst;
			m_inst = nullptr;
		}
	}

public:
	Singleton() {}
	virtual ~Singleton() {}
};


template <typename Class>
Class* Singleton<Class>::m_inst = nullptr;

#define SingletonInstance(Class) (Class::GetSingletonInstance())
#define SingletonRelease(Class) (Class::ReleaseSingletonInstance())


