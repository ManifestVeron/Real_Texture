// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Queue.h"

#include "CoreMinimal.h"

/**
 *  Sized Queue Class
 */
template<typename Type>
class ARA_NETWORKS_API SizedQueue : public TQueue<Type, EQueueMode::Spsc>
{
private:
	size_t m_size;
	size_t m_size_limit;

public:
	SizedQueue(size_t size_limit=1);
	~SizedQueue();
	bool Enqueue(const Type& data);
	bool Enqueue(Type&& data);
};

template<typename Type>
SizedQueue<Type>::SizedQueue(size_t size_limit):
	TQueue<Type, EQueueMode::Spsc>(),
	m_size(0),
	m_size_limit(size_limit)
{

}

template<typename Type>
SizedQueue<Type>::~SizedQueue()
{

}

template<typename Type>
bool SizedQueue<Type>::Enqueue(const Type& data)
{
	if(m_size < m_size_limit)
	{
		++m_size;
		return static_cast<TQueue<Type, EQueueMode::Spsc> *>(this)->Enqueue(data);
	}
	m_size = 0;
	static_cast<TQueue<Type, EQueueMode::Spsc> *>(this)->Empty();
	return static_cast<TQueue<Type, EQueueMode::Spsc> *>(this)->Enqueue(data);
}

template<typename Type>
bool SizedQueue<Type>::Enqueue(Type&& data)
{
	return Enqueue(data);
}
