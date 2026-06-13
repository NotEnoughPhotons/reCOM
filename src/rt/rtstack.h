#pragma once
#include <stack>

template<class Object>
class RTStack
{
public:
    RTStack();

    Object* Pop();
    void Push(Object* object);
    Object* Top() const;
};