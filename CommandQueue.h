#pragma once

#include "Fence.h"
#include "CommandBuffer.h"

#include "Common.h"

class CommandQueue {

public:

	CommandQueue(Handle<Fence> fence, Handle<CommandBuffer> command):
		pFence(fence), pCommand(command) { }

	void reset() const;
	void submit() const;

private:

	Handle<Fence> pFence;
	Handle<CommandBuffer> pCommand;

};