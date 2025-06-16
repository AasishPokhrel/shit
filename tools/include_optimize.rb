#!/usr/bin/ruby -w

def write_file_without_lines(source, lines, without_lines)
  out = File.new(source, 'w')
  lines.each_with_index { |v, i|
    if without_lines.member? i
      out.print "//", v
    else
      out.print v
    end
  }
  out.close()
end

def optimize(source, target)
  lines = File.new(source).readlines()
  includes = []
  lines.each_with_index { |v, i|
    if v =~ /^#include/
      # puts v
      includes << i
    end
  }

  unneeded_includes = []

  includes.each{|i|
    # puts i
    write_file_without_lines(source, lines, [i])

    ret = system("scons", "-u", target)
    if ret
      puts "INCUNNEEDED #{source} #{lines[i].chop}" 
      unneeded_includes << i
    end
  }

  write_file_without_lines(source, lines, unneeded_includes)
end

ARGV.each{ |filename|
  source = filename
  target = "build/#{filename.gsub(/\..pp$/, '.o')}"
  puts "#{source} => #{target}"

  optimize(source, target)
}

# EOF #
