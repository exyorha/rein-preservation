require 'json'

tables = {}

def convert_case(instr)
    outstr = ""

    instr.each_char do |ch|
        if ch >= 'A' && ch <= 'Z'
            unless outstr.empty?
                outstr << "_"
            end

            outstr << "#{ch.downcase}"
        else
            outstr << ch
        end
    end

    outstr
end

def convert_type(type)
    case type
    when 'System.Int32'
        'integer'

    when 'System.Int64'
        'bigint'

    when 'System.Single'
        'real'

    when 'System.Decimal'
        'decimal'

    when 'System.String'
        'text'

    when 'System.Boolean'
        'boolean'

    when 'MessagePack.MPDateTime'
        'timestamp'

    else
        raise "unsupported .NET type: #{type}"
    end
end

File.open('interrogation_3_7_0.txt', 'rb') do |inf|

    classname = nil
    waiting_for_class_attributes = false
    table = nil

    inf.each_line do |line|
        line.strip!

        case line
        when /\Aclass no. [0-9]+: (.*?)#(.*)\Z/
            if $1.empty?
                classname = $2
            else
                classname = "#{$1}.#{$2}"
            end
            waiting_for_class_attributes = true
            table = nil

        when /\Aattribute (.*)\Z/
            if waiting_for_class_attributes and $1 == "MasterMemory.MemoryTableAttribute"
                table = []
                tables[classname] = table
            end

        when /\Afield: '<(.*?)>k__BackingField', flags: 0x[0-9a-fA-F]+, at [0-9]+, '(.+)'/

            waiting_for_class_attributes = false

            unless table.nil?
                table.push [ $1, $2 ]
                puts "table field: #{classname}, #{$1}, #{$2}"
            end

        when /\Amethod /
            waiting_for_class_attributes = false


        end
    end
end

File.open('schema.sql', 'wb') do |outf|

    tables.each do |table_name, columns|
        unless table_name =~ /\ADark\.Entity(.+)\Z/
            raise "malformed table class name: #{table_name}"
        end

        table_name = convert_case $1

        if table_name.start_with? "m_"
            outf.puts "CREATE VIRTUAL TABLE #{table_name} USING masterdatabase ("
        else
            outf.puts "CREATE TABLE #{table_name} ("
        end

        columns.each_with_index do |(column_name, column_type), index|
            outf.write "  #{convert_case column_name} #{convert_type column_type}"

            if index != columns.size - 1
                outf.puts ","
            else
                outf.puts ""
            end
        end

        outf.puts ");"
        outf.puts ""
    end
end
#
#File.open('schema.json', 'wb') do |outf|
#    outf.write JSON.pretty_generate(tables)
#end
