#ifndef SINGLETON_H
#define SINGLETON_H
#include "global.h"

// template<typename T>
// class Singleton {
// protected:
//     Singleton() {};
//     Singleton(const Singleton<T> &) = delete;
//     Singleton& operator=(const Singleton<T> &) = delete;
// public:
//     ~Singleton() {qDebug() << "Singleton has been destroied!";};
//     static T& GetInstance() {
//         static T _instance;
//         return _instance;
//     }

//     void PrintAddress() {
//         qDebug() << &GetInstance();
//     }
// };
template <typename T>
class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;
    static std::shared_ptr<T> _instance;
public:
    static std::shared_ptr<T> GetInstance() {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
            _instance = std::shared_ptr<T>(new T);
        });
        return _instance;
    }

    void PrintAddress() {
        std::cout << "The address of the singleton is: " << _instance.get() << std::endl;
    }

    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    };
};
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;
#endif // SINGLETON_H
