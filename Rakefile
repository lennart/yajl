require 'fileutils'
def run cmd
  puts cmd unless QUIET
  `#{cmd}`
end
QUIET=ENV["QUIET"]
HOME=ENV["ALCHEMY_HOME"]
CC=File.join(HOME,%w{achacks gcc})
AR=File.join(HOME,%w{achacks ar})
CFILES=%w{yajl.c yajl_alloc.c yajl_buf.c yajl_encode.c yajl_lex.c yajl_gen.c yajl_parser.c}

WORKING_DIR="build"
AS3_SRC_DIR=File.expand_path "as3/src"
YAJL_SRC_DIR=File.expand_path "src"
  to_o = lambda do |input|
    "#{File.basename(input,File.extname(input))}.o"
  end

desc "Adds the build directory"
task :build_directory do
  FileUtils.mkdir_p WORKING_DIR+"/yajl" unless File.exists?(WORKING_DIR)
  run "find #{YAJL_SRC_DIR} -maxdepth 1 -type f -iname '*.h' -exec cp \\{\\} #{WORKING_DIR}/ \\;;"
  run "find #{YAJL_SRC_DIR}/api -maxdepth 1 -type f -iname '*.h' -exec cp \\{\\} #{WORKING_DIR}/yajl \\;;"
end

desc "Build static Yajl Library"
task :lib  => :objects do
  Dir.chdir(WORKING_DIR) do
    run "#{AR} r libyajl.a #{CFILES.map{|f| "#{to_o.call(f)}" }.join(" ")}"
  end
end


desc "Compile Library Objects"
task :objects => :build_directory do
  Dir.chdir(WORKING_DIR) do
    CFILES.each do |file|
      o_file = to_o.call file
      FileUtils.rm o_file, :verbose => true if File.exists?(o_file)
      run "#{CC} -c -I. #{File.join(YAJL_SRC_DIR,file)} -o #{o_file}"
    end
  end
end



desc "Compile SWC"
task :swc do
  Dir.chdir(WORKING_DIR) do
    run "#{CC} -Wall -I. -swc -O3 #{AS3_SRC_DIR+"/yajl-as3.c"} -L. -lyajl -o yajl.swc"
  end
end

desc "Install yajl.swc somewhere"
task :install do

  path = File.expand_path(ENV["TO"] || STDIN.gets.chomp)

  FileUtils.cp File.join(WORKING_DIR,"yajl.swc"), File.join(path, "yajl.swc")
end

desc "Clean all build products"
task :clean do
  Dir.chdir(WORKING_DIR) do
  files = FileList["*.o","*.swc","*.a","*.bc"].to_a
  FileUtils.rm files, :verbose => true unless files.empty?
  end
end
