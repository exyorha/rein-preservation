-- This is run on *every connection* to the database.

PRAGMA journal_mode = WAL;

PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS schema_version (id INTEGER NOT NULL PRIMARY KEY, version INTEGER NOT NULL);

INSERT OR IGNORE INTO schema_version (id, version) VALUES (1, 0);

RUN MIGRATIONS;

CREATE TEMPORARY TABLE internal_sessions (
    session_id TEXT NOT NULL PRIMARY KEY,
    user_id UNIQUE NOT NULL,
    expire_datetime BIGINT NOT NULL -- This is UNIX time, not .NET time: seconds, not microseconds
);

CREATE TEMPORARY TABLE database_viewer_text (
    language TEXT NOT NULL,
    message_key TEXT NOT NULL,
    message TEXT NOT NULL,

    PRIMARY KEY(language, message_key)
);
