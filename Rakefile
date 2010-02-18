require 'fileutils'
HOME=ENV["ALCHEMY_HOME"]
CC=File.join(HOME,%w{achacks gcc})
AR=File.join(HOME,%w{achacks ar})
CFILES=%w{yajl.c yajl_alloc.c yajl_buf.c yajl_encode.c yajl_lex.c yajl_gen.c yajl_parser.c}

desc "Build static Yajl Library"
task :lib do
  puts `#{AR} r libyajl.a #{CFILES.map{|f| "#{File.basename(f,File.extname(f))}.o" }.join(" ")}`
end

desc "Compile Library Objects"
task :objects do
  CFILES.each do |file|
    puts `#{CC} -c -I. #{file} -o #{File.basename(file,File.extname(file))}.o`
  end
end

desc "Compile SWC"
task :swc do
  puts `#{CC} -Wall -I. -swc -O3 yajl-as3.c -L. -lyajl -o yajl.swc`
end

desc "Install yajl.swc somewhere"
task :install do

  path = File.expand_path(ENV["TO"] || STDIN.gets.chomp)

  FileUtils.cp "yajl.swc", File.join(path, "yajl.swc")
end

desc "Clean all build products"
task :clean do
  files = FileList["*.o","*.swc","*.a","*.bc"].to_a
  FileUtils.rm files, :verbose => true unless files.empty?
end
