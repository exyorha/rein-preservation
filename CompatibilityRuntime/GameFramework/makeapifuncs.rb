def split_arg_list(string)
    result = []

    current = nil
    nest_level = 0

    string.each_char do |ch|
        if ch == '('
            nest_level += 1
        elsif ch == ')'
            nest_level -= 1
        end

        if ch == ',' && nest_level == 0
            if current.nil?
                current = ''
            end
            result.push current
            current = ''
        else
            if ch == ' ' && (current.nil? || current.empty?)
                next
            end

            if current.nil?
                current = ch.dup
            else
                current << ch
            end
        end
    end

    unless current.nil?
        result.push current
    end

    result
end

def arg_name(arg)
    if arg =~ /\(\*\s*(.+?)\)/
        return $1
    elsif arg =~ /[\* ]([^\s\*\[\]]+)(?:\[\])?$/
        return $1
    else
        raise "unable to parse the argument name out of a C argument: #{arg.inspect}"
    end
end

if ARGV.size != 2
    raise "Usage: <il2cpp-api-functions.h input header path> <output source path>"
end

File.open(ARGV[1], "wb") do |outf|

    outf.puts "#include <il2cpp-api.h>"
    outf.puts "#include \"support.h\""
    outf.puts "#include <ELF/Image.h>"
    outf.puts "#include <Translator/thunking.h>"
    outf.puts "#include \"GlobalContext.h\""
    outf.puts ""

    File.open(ARGV[0], "rb") do |inf|
        inf.each_line do |line|
            line.strip!


            if line =~ /DO_API(_NO_RETURN)?\((.+?),\s*(.+?)\s*,\s*\((.*)\)\)/

                func = $3

                if func == "il2cpp_add_internal_call" || $3 == "il2cpp_resolve_icall" || $3 == "il2cpp_init" ||
                    $3 == "il2cpp_init_utf16"
                   next
                end

                args = split_arg_list($4)

                argpack = ""

                args.each do |arg|
                    argpack << ", #{arg_name arg}"
                end


                outf.puts "#{$2} #{func}(#{$4}) {"
                outf.puts "  typedef #{$2}(*FunctionPointer)(#{$4});"
                outf.puts "  static FunctionPointer arm_#{$3} = reinterpret_cast<FunctionPointer>(GlobalContext::get().il2cpp().getSymbolChecked(#{$3.inspect}));"
                if $1.nil?
                    outf.puts "  return armcall(arm_#{$3}#{argpack});"
                else
                    outf.puts "  armcall_noreturn(arm_#{$3}#{argpack});"
                end
                outf.puts "}"
                outf.puts ""
            end

        end
    end
end
