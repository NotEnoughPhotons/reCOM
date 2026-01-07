#include "gamez/zSystem/zsys.h"

CSched_Manager zTaskScheduler;

void CSched_Manager::AddTask(const char* name, bool(*func)(f32, void*), f32 dT, void* registrar)
{
	CSched_Task* task = NULL;

	// Create a new task if pool is empty
	if (size() == 0)
	{
		task = new CSched_Task();
		task->m_name = NULL;
		task->m_active = false;
		task->m_dT = 0.0f;
		task->m_func = NULL;
		task->m_registrar = NULL;
	}
	else
	{
		// Otherwise, retrieve an available task from the pool
		task = front();
		pop_front();
	}

	if (task->m_name)
		zfree(task->m_name);

	task->m_name = NULL;

	if (!name)
		task->m_name = NULL;
	else
		task->m_name = zstrdup(name);

	task->m_active = false;
	task->m_dT = dT;
	task->m_func = func;
	task->m_registrar = registrar;

	insert(begin(), task);
}