#pragma once
#include "const.h"
template<typename T>
class Singleton {
protected:
    Singleton() {};
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;
public:
    ~Singleton() { std::cout << "Singleton has been destroied!"; };

    static T& GetInstance() {
        static T _instance;
        return _instance;
    }

    void PrintAddress() {
        std::cout << &GetInstance();
    }
};