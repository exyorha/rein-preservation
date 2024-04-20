#!/bin/sh

previous=

for dir in reincarnation/content/database_*.bin; do
    version="$(basename "$dir" ".bin")"
    echo $version

    sqlite3 individual.db > ${version}_3_7_0.sql <<EOF
.load 'build/GameServer/master_database_ext/libmaster_database_ext.so'
INSERT INTO masterdatabase_ctrl VALUES ('${dir}');
.dump --data-only m_%
EOF
done

