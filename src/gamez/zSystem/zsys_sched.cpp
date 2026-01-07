#include "gamez/zSystem/zsys.h"

CSched_Manager zTaskScheduler;

void CSched_Manager::AddTask(const char* name, bool(*func)(f32, void*), f32 dT, void* registrar)
{
	_task* task = NULL;

	// Create a new task if pool is empty
	if (size() == 0)
	{
		task = new _task();
		task->name = NULL;
		task->active = false;
		task->dT = 0.0f;
		task->func = NULL;
		task->registrar = NULL;
	}
	else
	{
		// Otherwise, retrieve an available task from the pool
		task = front();
		pop_front();
	}

	if (task->name)
		zfree(task->name);

	task->name = NULL;

	if (!name)
		task->name = NULL;
	else
		task->name = zstrdup(name);

	task->active = false;
	task->dT = dT;
	task->func = func;
	task->registrar = registrar;

	insert(begin(), task);
}