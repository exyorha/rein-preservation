#ifndef SQLITE_ERROR_H
#define SQLITE_ERROR_H

#include <string>
#include <stdexcept>

struct sqlite3;

namespace sqlite {
	class Error final : public std::runtime_error {
	public:
		Error(const std::string &message, int code);
		~Error();

		[[noreturn]] static void raiseError(int status, sqlite3 *database);

		inline int code() const {
			return m_code;
		}

	private:
		int m_code;
	};
}

#endif

