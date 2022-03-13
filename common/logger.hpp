#pragma once

#include <cstdio>
#include <ctime>
#include <cstdint>
#include <cstdarg>
#include <cstdlib>
#include <stack>

#if LOGGING_ENABLE == 1

	#define loggerWrite(...) write(__func__, ##__VA_ARGS__)
	#define loggerStart() start(__func__)

#else
	
	#define loggerWrite(...)
	#define loggerStart()

#endif

namespace cel
{
	#if LOGGING_ENABLE == 1
	class Logger;
	
	class LoggerBug
	{
	private:
		Logger & l;
		const char * m_func;
		bool bCompleted = false;
		
		LoggerBug(Logger & logger, const char * func) noexcept
			: l{ logger }, m_func{ func }
		{
		}
		
		friend class Logger;

	public:
		~LoggerBug() noexcept
		{
			this->stop();
		}
		
		void stop() noexcept;
	};
	
	class Logger
	{
	private:
		FILE * f = nullptr;
		
		std::stack<std::clock_t> stack;
	
	public:
		Logger(const char * logfile)
		{
			fopen_s(&this->f, logfile, "a+");
			if (this->f == nullptr)
			{
				throw std::runtime_error("Error opening logging file!\n");
			}
		}
		Logger()
			: Logger("logger.txt")
		{
		}
		
		~Logger() noexcept
		{
			if (this->f != nullptr) [[likely]]
			{
				std::fclose(this->f);
				this->f = nullptr;
			}
		}
		
		void write(const char * func, const char * format, ...) const noexcept
		{
			if (this->f == nullptr)
			{
				std::fputs("Logging file not open!\n", stderr);
				return;
			}
			
			std::time_t rawtime;
			std::time(&rawtime);
			tm ti;
			localtime_s(&ti, &rawtime);
			std::fprintf(
				this->f,
				"[%.2d.%.2d.%d @%.2d:%.2d:%.2d] @%s<",
				ti.tm_mday, ti.tm_mon + 1, ti.tm_year + 1900,
				ti.tm_hour, ti.tm_min,     ti.tm_sec,
				func
			);
			
			// Write message
			va_list ap;
			va_start(ap, format);
			
			std::vfprintf(this->f, format, ap);
			
			va_end(ap);
			
			std::fprintf(this->f, ">\n");
			std::fflush(this->f);
		}
		
		[[nodiscard]] LoggerBug start(const char * func)
		{
			this->stack.push(std::clock());
			return { *this, func };
		}
		void stop(LoggerBug & bug) noexcept
		{
			float elapsed = float(std::clock() - this->stack.top()) / float(CLOCKS_PER_SEC);
			this->stack.pop();

			this->write(bug.m_func, "Elapsed %.3f s", elapsed);
		}
	};
	
	inline void LoggerBug::stop() noexcept
	{
		if (this->bCompleted)
		{
			return;
		}
		
		this->bCompleted = true;
		this->l.stop(*this);
	}
	
	#else
	
	class LoggerBug
	{
	private:
		LoggerBug() noexcept
		{
		}
		
	public:
		void stop() noexcept
		{
		}
	};
	
	class Logger
	{
	public:
		Logger(const char * func) noexcept
		{
		}
		Logger() noexcept
		{
		}
		
		void write(const char * func, const char * format, ...) const noexcept
		{
		}
		
		[[nodiscard]] LoggerBug start(const char * func) noexcept
		{
			return {};
		}
		void stop(LoggerBug & bug) noexcept
		{
		}
	};
	

	#endif

	#ifdef LOGGER_AUTO

	inline Logger g_logger;

	#endif
}

