#ifndef SMARTPTR_H
#define SMARTPTR_H

template <typename T>
class SmartRtr {
    T* obj;
public:
    SmartRtr(T* obj) : obj(obj) {}
    ~SmartRtr() { delete obj; }

    T* operator->() { return obj; }
    T& operator*() { return *obj; }

    // Запрет копирования
    SmartRtr(const SmartRtr&) = delete;
    SmartRtr& operator=(const SmartRtr&) = delete;
};

#endif // SMARTPTR_H
