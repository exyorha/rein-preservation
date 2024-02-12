require 'openssl'

def write_sql(outf, input_filename)
    contents = File.binread(input_filename)

    delim = ("SQL" + OpenSSL::Digest::MD5.hexdigest(contents)).slice(0, 16)

    outf.write "R\"#{delim}("
    outf.write contents
    outf.write ")#{delim}\""
end


if ARGV.size < 2
    warn "Usage: compile_migrations <OUTPUT> <INPUT SQL INIT FILE> [INPUT SQL MIGRATION FILES]..."
    exit 1
end

output = ARGV.shift

File.open(output, "wb") do |outf|
    outf.puts "#include <DataModel/Database.h>"

    outf.write "const char *const Database::m_initSQL = "

    init_file = ARGV.shift

    write_sql outf, init_file

    outf.puts ";"

    outf.puts "const Database::DatabaseMigration Database::m_migrations[] = {";

    highest_version = 0

    ARGV.each do |input_file|
        basename = File.basename input_file, ".sql"

        unless basename =~ /\A([0-9]+)_([0-9]+)_/
            raise "the migration file is not named in the expected manner: #{input_file.inspect}"
        end

        from_version = Integer($1)
        to_version = Integer($2)

        outf.write "  { .fromVersion = #{from_version}, .toVersion = #{to_version}, .sql = "

        write_sql outf, input_file

        outf.puts " },"

        if to_version > highest_version
            highest_version = to_version
        end
    end

    outf.puts "  { .fromVersion = 0, .toVersion = 0, .sql = nullptr }"
    outf.puts "};"

    outf.puts "const unsigned int Database::m_currentSchemaVersion = #{highest_version};"
end
