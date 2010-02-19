require 'fileutils'
QUIET=ENV["QUIET"]
HOME=ENV["ALCHEMY_HOME"]
CC=File.join(HOME,%w{achacks gcc})
AR=File.join(HOME,%w{achacks ar})
CFILES=%w{yajl.c yajl_alloc.c yajl_buf.c yajl_encode.c yajl_lex.c yajl_gen.c yajl_parser.c}

PREFIX="lib/yajl/"
WORKING_DIR=File.join(PREFIX,"build")
YAJL_SRC_DIR=File.expand_path "src"
to_o = lambda do |input|
  "#{File.basename(input,File.extname(input))}.o"
end

  module Alchemy
    class Utils < Thor
      include Thor::Actions

      destination_root = "bin"
      method_option :debug, :aliases => "-D"

      desc :clean, "MUST BE REVIEWED BEFORE USE"
      def clean dir
        inside(dir) do
          files = FileList["*.o","*.swc","*.a","*.bc"].to_a
          FileUtils.rm files, :verbose => true unless files.empty?
        end
      end

      desc :install, "DEPRECATED"
      def install 
        invoke :swc
        path = File.expand_path(ENV["TO"] || STDIN.gets.chomp)

        FileUtils.cp File.join(WORKING_DIR,"yajl.swc"), File.join(path, "yajl.swc")
      end
      
      desc :skeleton, "Prepare folder for yajl build"
      def build_directory name
        FileUtils.mkdir_p WORKING_DIR+"/yajl" unless File.exists?(WORKING_DIR)
        run "find #{YAJL_SRC_DIR} -maxdepth 1 -type f -iname '*.h' -exec cp \\{\\} #{WORKING_DIR}/ \\;;" 
        run "find #{YAJL_SRC_DIR}/api -maxdepth 1 -type f -iname '*.h' -exec cp \\{\\} #{WORKING_DIR}/yajl \\;;"
      end

      desc :sources, "Find Sources"
      def sources path
        Dir[File.join(path,%w{src ** /})].map {|f| File.expand_path f }
      end


      desc :swc, "Compiles the SWC for Yajl Extension"
      def swc dir
        inside(dir, :verbose => true) do
          run "#{CC} -Wall -I. -swc#{options[:debug] ? " -DVERBOSE" : " -O3"} #{File.join("yajl-as3.c")} -L. -lyajl -o yajl.swc"
        end
      end
    end

    class Leave < Thor::Group
      include Thor::Actions
      desc "Remove all clutter that was produced"
      class << self
        def source_root
          File.expand_path(File.dirname(__FILE__))
        end
      end

      def folders
        remove_dir "bin-debug"
        remove_dir "lib"
      end

    end

    class Setup < Thor::Group
      include Thor::Actions
      argument :name
      class_option :debug
      class << self
        def source_root 
          File.dirname(__FILE__)
        end
      end


      desc "Prepare for building SWC" 
      def prepare
        source_paths = invoke("alchemy:utils:sources", ["."])
        puts source_paths if options[:debug]
        api = %w{yajl_gen yajl_parse yajl_common}
        api_header = api.map do |f|
          find_in_source_paths("#{f}.h")
        end

        empty_directory(name)
        inside(name, :verbose => true) do |path|
          empty_directory("yajl")
          api_header.each {|f| copy_file f, "yajl" }
        #  copy_file find_in_source_paths("yajl-as3.c"), "."
        end
      end

      def objects 
        #        invoke "utils:build_directory", [path]
        #        Dir.chdir(path) do
        #          CFILES.each do |file|
        #            o_file = to_o.call file
        #            FileUtils.rm o_file, :verbose => true, :noop => true if File.exists?(o_file)
        #            run "#{CC} -c -I. #{File.join(YAJL_SRC_DIR,file)} -o #{o_file}"
        #          end
        #        end
      end

      def lib 
        #        Dir.chdir(WORKING_DIR) do
        #          run "#{AR} r libyajl.a #{CFILES.map{|f| "#{to_o.call(f)}" }.join(" ")}"
        #        end
      end




    end
  end

  # vim:filetype=ruby
