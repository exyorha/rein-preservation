#ifndef DATA_MODEL_SQLITE_TRANSACTION_H
#define DATA_MODEL_SQLITE_TRANSACTION_H

namespace sqlite {
	class Database;

	class Transaction {
	public:
		Transaction(Database *database);
		~Transaction();

		Transaction(const Transaction &other) = delete;
		Transaction &operator =(const Transaction &other) = delete;

		inline void commit() {
			m_commit = true;
		}

	private:
		Database *m_database;
		bool m_commit;
	};
}

#endif
