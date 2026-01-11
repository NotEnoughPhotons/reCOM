#include "zvid.h"

#include "Apps/FTS/gamever.h"
#include "gamez/zReader/zrdr.h"

void zVid_CreateWindow()
{
    auto settings_rdr_file = zrdr_read("./data/zrdr/settings.rdr", NULL, ZRDR_FLAG_RAW);
    theWindow = new CWindow(settings_rdr_file);
}

CWindow::CWindow()
{
	m_name = NULL;
	m_width = 0;
	m_height = 0;
	m_window = NULL;
	m_renderer = NULL;
	m_context = NULL;
}

CWindow::CWindow(CRdrFile* reader)
{
	auto tag = zrdr_findtag(reader, "settings");
	auto window_settings = zrdr_findtag(tag, "window_settings");
	auto game_specific_settings = zrdr_findtag(tag, "game_specific_settings");

	zrdr_finduint(window_settings, "width", &m_width, 1);
	zrdr_finduint(window_settings, "height", &m_height, 1);

	auto window_flags_tag = zrdr_findtag(window_settings, "window_flags");
	
	u32 window_flags = 0;
	
	if (window_flags_tag)
	{
		for (u32 i = 1; i < window_flags_tag->array->integer; i++)
		{
			char* flag = window_flags_tag->array[i].string;

			if (SDL_strcasecmp(flag, "FULLSCREEN") == 0)
				window_flags |= SDL_WINDOW_FULLSCREEN;
			else if (SDL_strcasecmp(flag, "BORDERLESS") == 0)
				window_flags |= SDL_WINDOW_BORDERLESS;
			else if (SDL_strcasecmp(flag, "MINIMIZED") == 0)
				window_flags |= SDL_WINDOW_MINIMIZED;
			else if (SDL_strcasecmp(flag, "MAXIMIZED") == 0)
				window_flags |= SDL_WINDOW_MAXIMIZED;
			else if (SDL_strcasecmp(flag, "OPENGL") == 0)
				window_flags |= SDL_WINDOW_OPENGL;
		}
	}
	
	if (game_specific_settings)
	{
		auto name_tag = zrdr_findtag(game_specific_settings, game_info.name);
		if (name_tag)
		{
			m_name = name_tag->array[1].string;
		}
		else
		{
			m_name = "GameZ";
		}
	}
	
	m_window = SDL_CreateWindow(m_name, m_width, m_height, window_flags);

	zVid_Assert(m_window != NULL, LONG_MAX, __FILE__, __LINE__);

	m_renderer = SDL_CreateRenderer(m_window, NULL);

	if ((SDL_GetWindowFlags(m_window) & SDL_WINDOW_OPENGL) != 0)
	{
		m_context = SDL_GL_CreateContext(m_window);
	}
	
	zrdr_free(reader);
}

CWindow::CWindow(const char* name, u32 width, u32 height)
{
	m_name = zstrdup(name);
	m_width = width;
	m_height = height;
	
	m_window = SDL_CreateWindow(m_name, m_width, m_height, SDL_WINDOW_OPENGL);

	zVid_Assert(m_window != NULL, LONG_MAX, __FILE__, __LINE__);

	m_renderer = SDL_CreateRenderer(m_window, NULL);

	if ((SDL_GetWindowFlags(m_window) & SDL_WINDOW_OPENGL) != 0)
	{
		SDL_GL_CreateContext(m_window);
	}
}

SDL_Window* CWindow::GetWindow() const
{
	return m_window;
}

SDL_Renderer* CWindow::GetRenderer() const
{
	return m_renderer;
}

SDL_GLContext CWindow::GetContext() const
{
	return m_context;
}