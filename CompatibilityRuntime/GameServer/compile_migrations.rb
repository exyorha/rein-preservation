require 'openssl'

if ARGV.empty?
    warn "Usage: compile_migrations <OUTPUT> [INPUT SQL FILES]..."
    exit 1
end

output = ARGV.shift

File.open(output, "wb") do |outf|
    outf.puts "#include <DataModel/Database.h>"

    outf.puts "const Database::DatabaseMigration Database::m_migrations[] = {";

    highest_version = 0

    ARGV.each do |input_file|
        basename = File.basename input_file, ".sql"

        unless basename =~ /\A([0-9]+)_([0-9]+)_/
            raise "the migration file is not named in the expected manner: #{input_file.inspect}"
        end

        from_version = Integer($1)
        to_version = Integer($2)

        contents = File.binread(input_file)

        delim = ("SQL" + OpenSSL::Digest::MD5.hexdigest(contents)).slice(0, 16)

        outf.write "  { .fromVersion = #{from_version}, .toVersion = #{to_version}, .sql = R\"#{delim}("

        outf.write contents

        outf.puts ")#{delim}\" },"

        if to_version > highest_version
            highest_version = to_version
        end
    end

    outf.puts "  { .fromVersion = 0, .toVersion = 0, .sql = nullptr }"
    outf.puts "};"

    outf.puts "const unsigned int Database::m_currentSchemaVersion = #{highest_version};"
end
