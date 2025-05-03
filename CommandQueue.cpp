#include "CommandQueue.h"

void CommandQueue::reset() const
{

	pFence->wait();
	pFence->reset();
}

void CommandQueue::submit() const
{
}
