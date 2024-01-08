// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadySignal.h"

ReadySignal::ReadySignal(FReadySignal fready_signal)
{
	channel = fready_signal.channel;
	n_tick = fready_signal.n_tick;
}

FReadySignal::FReadySignal(ReadySignal ready_signal)
{
	channel = ready_signal.channel;
	n_tick = ready_signal.n_tick;
}

ReadySignal FReadySignal::GetRawStruct()
{
	ReadySignal ready_signal(*this);
	return ready_signal;
}
