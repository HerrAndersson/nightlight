#include "GameObject.h"


GameObject::GameObject()
{
}


GameObject::~GameObject()
{
}

void* GameObject::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void GameObject::operator delete(void* p)
{
	_mm_free(p);
}
