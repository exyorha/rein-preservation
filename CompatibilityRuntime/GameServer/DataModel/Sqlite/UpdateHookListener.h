#ifndef DATA_MODEL_SQLITE_UPDATE_HOOK_LISTENER_H
#define DATA_MODEL_SQLITE_UPDATE_HOOK_LISTENER_H

#include <stdint.h>

namespace sqlite {
	class UpdateHookListener {
	protected:
		inline UpdateHookListener() {}
		inline ~UpdateHookListener() {}

	public:
		UpdateHookListener(const UpdateHookListener& other) = delete;
		UpdateHookListener &operator =(const UpdateHookListener& other) = delete;

		virtual void updateEvent(int event, const char* database, const char* table, int64_t rowid) = 0;
	};
}

#endif
