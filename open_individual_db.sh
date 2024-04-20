#!/bin/sh

exec sqlite3 -init open_individual_db.sql individual.db "$@"
