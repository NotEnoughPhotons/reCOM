#include "gamez/zSystem/zsys.h"

CSched_Manager zTaskScheduler;

void CSched_Manager::AddTask(const char* name, bool(*func)(f32, void*), f32 priority, void* registrar)
{
	CSched_Task* task = NULL;

	// Create a new task if pool is empty
	if (size() == 0)
	{
		task = new CSched_Task();
		task->m_name = NULL;
		task->m_removed = false;
		task->m_priority = 0.0f;
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

	task->m_removed = false;
	task->m_priority = priority;
	task->m_func = func;
	task->m_registrar = registrar;

	insert(begin(), task);
}

CSched_Task* CSched_Manager::GetTask(u32 id)
{
	u32 index = 0;

	for (auto i = begin(); i != end(); ++i)
	{
		if (index == id)
			return *i;

		index++;
	}

	return NULL;
}

bool CSched_Manager::RemoveTask(CSched_Task* task, bool child)
{
	for (auto i = begin(); i != end(); ++i)
	{
		if (task == *i)
		{
			task->m_removed = true;
			return true;
		}
	}

	return true;
}

bool CSched_Manager::RemoveTask(const char* name, bool child)
{
	for (auto i = begin(); i != end(); ++i)
	{
		if (!strcmp((*i)->m_name, name))
		{
			(*i)->m_removed = true;
			return true;
		}
	}
}

bool CSched_Manager::Tick(f32 dT)
{
	m_inTick = true;
	for (auto i = begin(); i != end(); ++i)
	{
		CSched_Task* task = *i;
		bool ran = false;

		if (!task->m_removed)
		{
			m_priority = task->m_priority;

			if (!task->m_func)
				ran = true;
			else
				ran = task->m_func(dT, task->m_registrar);
		}
	}
	m_inTick = false;
	return true;
}