#include <DataModel/Sqlite/Transaction.h>
#include <DataModel/Sqlite/Database.h>

namespace sqlite {
	Transaction::Transaction(Database *db) : m_database(db), m_commit(false) {
		db->createSavepoint("s" + std::to_string(reinterpret_cast<uintptr_t>(this)));
	}

	Transaction::~Transaction() {
		if (!m_commit) {
			m_database->rollbackToSavepoint("s" + std::to_string(reinterpret_cast<uintptr_t>(this)));
		}

		m_database->releaseSavepoint("s" + std::to_string(reinterpret_cast<uintptr_t>(this)));
	}
}
