#!/bin/sh

exec sqlite3 -init open_individual_db.sql ${HOME}/.config/unity3d/SQUARE\ ENIX\ Co_,Ltd_/NieR/individual.db "$@"
