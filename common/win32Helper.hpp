#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <shellapi.h>
#include <io.h>
#include <cstdio>
#include <cstring>
#include <iostream>

namespace win32
{
    /*************************************************************************
     * CommandLineToArgvA            [SHELL32.@]
     *
     * We must interpret the quotes in the command line to rebuild the argv
     * array correctly:
     * - arguments are separated by spaces or tabs
     * - quotes serve as optional argument delimiters
     *   '"a b"'   -> 'a b'
     * - escaped quotes must be converted back to '"'
     *   '\"'      -> '"'
     * - consecutive backslashes preceding a quote see their number halved with
     *   the remainder escaping the quote:
     *   2n   backslashes + quote -> n backslashes + quote as an argument delimiter
     *   2n+1 backslashes + quote -> n backslashes + literal quote
     * - backslashes that are not followed by a quote are copied literally:
     *   'a\b'     -> 'a\b'
     *   'a\\b'    -> 'a\\b'
     * - in quoted strings, consecutive quotes see their number divided by three
     *   with the remainder modulo 3 deciding whether to close the string or not.
     *   Note that the opening quote must be counted in the consecutive quotes,
     *   that's the (1+) below:
     *   (1+) 3n   quotes -> n quotes
     *   (1+) 3n+1 quotes -> n quotes plus closes the quoted string
     *   (1+) 3n+2 quotes -> n+1 quotes plus closes the quoted string
     * - in unquoted strings, the first quote opens the quoted string and the
     *   remaining consecutive quotes follow the above rule.
     */
    /* Credit to GitHub repo: https://github.com/futurist/CommandLineToArgvA */
    inline LPSTR * WINAPI CommandLineToArgvA_wine(LPSTR lpCmdline, int * numargs) noexcept
    {
        DWORD argc;
        LPSTR * argv;
        LPSTR s;
        LPSTR d;
        LPSTR cmdline;
        int qcount, bcount;

        if (!numargs || lpCmdline == nullptr || *lpCmdline == 0)
        {
            ::SetLastError(ERROR_INVALID_PARAMETER);
            return nullptr;
        }

        /* --- First count the arguments */
        argc = 1;
        s = lpCmdline;
        /* The first argument, the executable path, follows special rules */
        if (*s == '"')
        {
            /* The executable path ends at the next quote, no matter what */
            s++;
            while (*s)
            {
                if (*s++ == '"')
                {
                    break;
                }
            }
        }
        else
        {
            /* The executable path ends at the next space, no matter what */
            while (*s && *s != ' ' && *s != '\t')
            {
                s++;
            }
        }
        /* skip to the first argument, if any */
        while (*s == ' ' || *s == '\t')
        {
            s++;
        }
        if (*s)
        {
            argc++;
        }

        /* Analyze the remaining arguments */
        qcount = bcount = 0;
        while (*s)
        {
            if ((*s == ' ' || *s == '\t') && qcount == 0)
            {
                /* skip to the next argument and count it if any */
                while (*s == ' ' || *s == '\t')
                {
                    s++;
                }
                if (*s)
                {
                    argc++;
                }
                bcount = 0;
            }
            else if (*s == '\\')
            {
                /* '\', count them */
                bcount++;
                s++;
            }
            else if (*s == '"')
            {
                /* '"' */
                if ((bcount & 1) == 0)
                {
                    qcount++; /* unescaped '"' */
                }
                s++;
                bcount = 0;
                /* consecutive quotes, see comment in copying code below */
                while (*s == '"')
                {
                    qcount++;
                    s++;
                }
                qcount = qcount % 3;
                if (qcount == 2)
                {
                    qcount = 0;
                }
            }
            else
            {
                /* a regular character */
                bcount = 0;
                s++;
            }
        }

        /* Allocate in a single lump, the string array, and the strings that go
         * with it. This way the caller can make a single LocalFree() call to free
         * both, as per MSDN.
         */
        auto lpCmdlineLen = std::strlen(lpCmdline);
        argv = static_cast<LPSTR *>(::LocalAlloc(LMEM_FIXED, (argc + 1) * sizeof(LPSTR) + (lpCmdlineLen + 1) * sizeof(char)));
        if (argv == nullptr) [[unlikely]]
        {
            return nullptr;
        }
        cmdline = (LPSTR)(argv + argc + 1);
        strcpy_s(cmdline, (lpCmdlineLen + 1) * sizeof(char), lpCmdline);

        /* --- Then split and copy the arguments */
        argv[0] = d = cmdline;
        argc = 1;
        /* The first argument, the executable path, follows special rules */
        if (*d == '"')
        {
            /* The executable path ends at the next quote, no matter what */
            s = d + 1;
            while (*s)
            {
                if (*s == '"')
                {
                    s++;
                    break;
                }
                *d++ = *s++;
            }
        }
        else
        {
            /* The executable path ends at the next space, no matter what */
            while (*d && *d != ' ' && *d != '\t')
            {
                d++;
            }
            s = d;
            if (*s)
            {
                s++;
            }
        }
        /* close the executable path */
        *d++ = 0;
        /* skip to the first argument and initialize it if any */
        while (*s == ' ' || *s == '\t')
        {
            s++;
        }
        if (!*s)
        {
            /* There are no parameters so we are all done */
            argv[argc] = nullptr;
            *numargs = argc;
            return argv;
        }

        /* Split and copy the remaining arguments */
        argv[argc++] = d;
        qcount = bcount = 0;
        while (*s)
        {
            if ((*s == ' ' || *s == '\t') && qcount == 0)
            {
                /* close the argument */
                *d++ = 0;
                bcount = 0;

                /* skip to the next one and initialize it if any */
                do
                {
                    s++;
                } while (*s == ' ' || *s == '\t');
                if (*s)
                {
                    argv[argc++] = d;
                }
            }
            else if (*s == '\\')
            {
                *d++ = *s++;
                bcount++;
            }
            else if (*s == '"')
            {
                if ((bcount & 1) == 0)
                {
                    /* Preceded by an even number of '\', this is half that
                     * number of '\', plus a quote which we erase.
                     */
                    d -= bcount / 2;
                    qcount++;
                }
                else
                {
                    /* Preceded by an odd number of '\', this is half that
                     * number of '\' followed by a '"'
                     */
                    d = d - bcount / 2 - 1;
                    *d++ = '"';
                }
                s++;
                bcount = 0;
                /* Now count the number of consecutive quotes. Note that qcount
                 * already takes into account the opening quote if any, as well as
                 * the quote that lead us here.
                 */
                while (*s == '"')
                {
                    if (++qcount == 3)
                    {
                        *d++ = '"';
                        qcount = 0;
                    }
                    s++;
                }
                if (qcount == 2)
                {
                    qcount = 0;
                }
            }
            else
            {
                /* a regular character */
                *d++ = *s++;
                bcount = 0;
            }
        }
        *d = '\0';
        argv[argc] = nullptr;
        *numargs = argc;

        return argv;
    }

	class ArgcArgvW
	{
	public:
		LPWSTR o_lpCmdArgs{ nullptr };
		int argc;
		wchar_t ** argv;

		ArgcArgvW() noexcept
		{
			this->o_lpCmdArgs = ::GetCommandLineW();
			this->argv = ::CommandLineToArgvW(this->o_lpCmdArgs, &this->argc);
		}
		ArgcArgvW(LPWSTR lpcmdArgs) noexcept
		{
			this->argv = ::CommandLineToArgvW(lpcmdArgs, &this->argc);
		}
		~ArgcArgvW() noexcept
		{
			::LocalFree(this->argv);
		}
	};

	class ArgcArgv
	{
	public:
		LPSTR o_cmdArgs{ nullptr };
		int argc;
		char ** argv;

		ArgcArgv() noexcept
		{
			this->o_cmdArgs = ::GetCommandLineA();
			this->argv = win32::CommandLineToArgvA_wine(this->o_cmdArgs, &this->argc);
		}
		ArgcArgv(LPSTR cmdArgs) noexcept
		{
			this->argv = win32::CommandLineToArgvA_wine(cmdArgs, &this->argc);
		}
		~ArgcArgv() noexcept
		{
			::LocalFree(this->argv);
		}
	};

    class WinConsole
    {
    public:
        bool m_bConsoleCreated{ false };

        WinConsole() = default;
        ~WinConsole() noexcept
        {
            // Try freeing console
            this->detach();
        }

        bool attach() noexcept
        {
            if (this->m_bConsoleCreated)
            {
                return true;
            }

            this->m_bConsoleCreated = ::AllocConsole() != 0;

            if (this->m_bConsoleCreated) [[likely]]
            {
                FILE * fp;
                freopen_s(&fp, "CONOUT$", "w", stdout);
                freopen_s(&fp, "CONOUT$", "w", stderr);
                freopen_s(&fp, "CONIN$", "r", stdin);

                std::cout.clear();
                std::clog.clear();
                std::cerr.clear();
                std::cin.clear();

                // std::wcout, std::wclog, std::wcerr, std::wcin
                HANDLE hConOut = ::CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                HANDLE hConIn = ::CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                ::SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
                ::SetStdHandle(STD_ERROR_HANDLE, hConOut);
                ::SetStdHandle(STD_INPUT_HANDLE, hConIn);

                std::wcout.clear();
                std::wclog.clear();
                std::wcerr.clear();
                std::wcin.clear();
            }

            return this->m_bConsoleCreated;
        }
        void detach() noexcept
        {
            if (this->m_bConsoleCreated)
            {
                ::FreeConsole();
                this->m_bConsoleCreated = false;
            }
        }
    };
}
