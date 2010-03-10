require 'rubygems'
require 'fileutils'
require 'rake'
require 'rake/clean'
require 'sprout'
require 'sprout/tasks/sftp_task'
ROOT= File.dirname(__FILE__)

VERSION= File.exists?(version_file = File.join(ROOT,"SPROUT_VERSION")) ? File.read(version_file).strip : "0.0.0"
FILES = File.join(File.dirname(__FILE__), 'lib')
PKG = File.join(File.dirname(__FILE__), 'pkg')
ARTIFACTS = ENV['CC_BUILD_ARTIFACTS'] || 'artifact'
QUIET=(ENV["QUIET"] == "yes") || false
DEBUG=(ENV["DEBUG"] == "yes") || false
HOME=ENV["ALCHEMY_HOME"]
CC=File.join(HOME,%w{achacks gcc})
AR=File.join(HOME,%w{achacks ar})
CFILES=%w{yajl.c yajl_alloc.c yajl_buf.c yajl_encode.c yajl_lex.c yajl_gen.c yajl_parser.c}

WORKING_DIR="build"
AS3_SRC_DIR=File.expand_path "src/as3-bindings"
YAJL_SRC_DIR=File.expand_path "src"
to_o = lambda do |input|
  "#{File.basename(input,File.extname(input))}.o"
end

Dir.glob("#{FILES}/*.rb").each do |file|
  load file

  name = File.basename(file).split('.rb').join('')
  task :package => name
end

CLEAN.add(PKG)

desc "Package all libraries as gems"
task :package do
  Dir.glob("#{PKG}/**").each do |file|
    if(File.directory?(file))
      FileUtils.rm_rf(file)
    end
  end
end

def run cmd
  puts cmd
  puts `#{cmd}`
end
namespace :sprout do
  desc "Zip files for Precompiled Sprout"
  task :zip => :swc do
#    zip_me = Dir[File.join(ROOT,"as3","src","**","*"] << File.join(WORKING_DIR,"yajl.swc")
    target_dir =  File.join(ROOT,"sprout","as3yajl")                          
    FileUtils.rm_rf target_dir, :verbose => true if File.exists?(target_dir)
    FileUtils.mkdir_p target_dir , :verbose => true
    FileUtils.cp_r File.join(ROOT,"as3","src","cc"), target_dir, :verbose => true
    FileUtils.cp  File.join(WORKING_DIR,"yajl.swc"), target_dir, :verbose => true
    Dir.chdir(File.join(target_dir,"..")) do
      name = File.basename(target_dir)
      run "zip -r #{name}-#{VERSION}.zip #{name}"
    end
  end
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
    lib = "libyajl.a"
    unless File.exists?(lib)
      run "#{AR} r #{lib} #{CFILES.map{|f| "#{to_o.call(f)}" }.join(" ")}"
    else
      puts "Skipping #{lib}"
    end
  end
end


desc "Compile Library Objects"
task :objects => :build_directory do
  Dir.chdir(WORKING_DIR) do
    CFILES.each do |file|
      o_file = to_o.call file
      unless File.exists?(o_file)
        run "#{CC} -c -I. #{File.join(YAJL_SRC_DIR,file)} -o #{o_file}"
      else
        puts "Skipping #{o_file}"
      end
    end
  end
end



desc "Compile SWC"
task :swc do
  Dir.chdir(WORKING_DIR) do
    run "#{CC} -Wall -I. -swc#{DEBUG ? " -DDEBUG_LOG" : ""}#{" -DVERBOSE_LOG" unless QUIET} -O3 #{AS3_SRC_DIR+"/yajl-as3.c"} -L. -lyajl -o yajl.swc"
  end
end

desc "Install yajl.swc somewhere"
task :install => :swc do
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
