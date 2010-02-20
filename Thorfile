require 'fileutils'

module Alchemy
  module Utils
    def sources_in path, options = {}
      files = File.join(path,%w{src *.c})
      options[:expand] ? Dir[files].map {|f| File.expand_path(f) } : Dir[files]
    end

    def headers_in root, path = nil
      Dir[File.join([root,"src",path,"*.h"].compact)]
    end

    def to_ext path, ext = "o" 
      "#{File.basename(path,File.extname(path))}.#{ext}"
    end
  end

  module ToolChain
    class Tool 
      include Thor::Actions
      argument :bin, :type => :string, :required => true, :banner => "Executable"
      argument :conf, :type => :array, :required => true, :banner => "Options"
      argument :args, :type => :array, :required => true, :banner => "Arguments"
      class_option :alchemy_home, :default => ENV["ALCHEMY_HOME"]

      def compile
        puts "Compiling with #{bin} #{conf} #{args}"
        run "#{File.join(options[:alchemy_home],"achacks",bin)} #{conf.join(" ")} #{args.join(" ")}"
      end
    end

    class GenComp < Thor::Group
      argument :bin, :required => true, :banner => "Compiler"
      argument :rules, :type => :array, :required => true, :banner => "Rules"
#      argument :sources, :type => :array, :required => true, :banner => "Sources"
#      argument :target, :type => :string, :required => true, :banner => "Target"
      argument :suffix, :type => :string

      class_option :warnings, :type => :array
      class_option :cflags, :type => :array
      class_option :ldflags, :type => :array
      class_option :defines, :type => :array
      class_option :includes, :type => :array
      class_option :libraries, :type => :array

      def conf
        [%w{warnings defines includes libraries}.map {|t| (values = options[t.to_sym]) ? values.map {|v| "-#{t[0..0].upcase}#{v}" } : nil },
        options[:cflags],
        options[:ldflags],
        "-o #{target}#{options[:suffix] ? ".#{options[:suffix]}" : ""}"].flatten.compact
      end

      def args
        sources.join(" ")
      end

      def compile 
        puts <<DEBUG
[#{self.class.to_s}]
Options: #{options.to_yaml} 
#{[conf, args].to_yaml}"
DEBUG
        sources.each do |file|
        end
          unless File.exists? object
            invoke "alchemy:tool_chain:gen_comp", ["gcc", [file], object, "o"], options.merge({:includes => "."})
          else
            puts "skipping compilation of #{object}"
          end
        invoke "alchemy:tool_chain:tool", [bin,conf,args]
      end
    end

    class Swc < GenComp
      argument :bin, :default => "gcc"
      argument :suffix, :type => :string, :default => "swc"

      class_option :warnings, :default => %w{all}
      class_option :cflags, :default => %w{-swc -O3 -I.}
      class_option :ldflags, :default => %w{-lyajl -L.}
      class_option :defines, :default => %w{VERBOSE}

      def compile 

      end
    end

    class Bc < Thor::Group
      include Thor::Actions
      include Utils
      argument :sources, :type => :array, :required => true, :banner => "Sources"
      argument :suffix, :type => :string, :default => "o"
      argument :bin, :default => "gcc"
      class_option :clean, :default => false
      remove_invocation "alchemy:tool_chain:gen_comp"

      def make_objects
        dst = options[:destination_root]
        rules = sources.map do |file|

          object = File.join(dst,to_ext(file, suffix))
          puts "Input File: #{file}"
          puts "Output file: #{object}"
          puts "[#{self.class.to_s}] Options: #{options.to_yaml}"

          remove_file object, :verbose => true if options[:clean]
          [file,object]
        end

        invoke "alchemy:tool_chain:gen_comp", ["gcc", rules, "o"], options.merge({:includes => "."})
      end

    end

    class Archive < Tool
      argument :sources, :type => :array, :required => true
      argument :target, :type => :string, :required => true
      argument :bin , :default => "ar"

      def gather_options
        conf = "r"
        args = [ [target].concat(source.map {|f| to_ext(f,"o") })].join " "
      end
    end
  end

  class Yajl < Thor
    include Thor::Actions
    include Utils

    class << self
      def source_root
        File.expand_path(File.dirname(__FILE__))
      end
    end
    class_option :debug, :default => true, :aliases => "-D"
    class_options :alchemy_home => ENV["ALCHEMY_HOME"],
      :destination_root => "build",
      :source_root => "src",
      :as3_src_root => "as3/src"

    desc :prepare, "Prepare for building SWC" 
    def prepare
      destination = options[:destination_root]
      cfiles = sources_in "." 
      headers = headers_in "."
      api_headers = headers_in ".", "api"
      if options[:debug]    
        puts "Destination Root: #{destination_root}"
        puts "Source Root: #{options[:source_root]}"
        puts "Source Path: #{source_paths}"
        puts "Sources: #{[cfiles,headers,api_headers].to_yaml}"
      end
      api = empty_directory(File.join(destination, "yajl"))

      api_headers.each do |f| 
        dst = File.join(api,File.basename(f))
        copy_file f, dst        
      end
      [cfiles,headers].flatten.each do |f|
        dst = File.join(destination, File.basename(f))
        copy_file f, dst
      end
    end


    desc :clean, "MUST BE REVIEWED BEFORE USE"
    def clean
      Dir.chdir(options[:destination_root]) do
        source_paths.each do |source_root|
          Dir["*.o","*.swc","*.a","*.bc"].to_a.each do |f|
            remove_file f
          end
        end
      end
    end

    desc :install, "DEPRECATED"
    def install
      invoke :swc
      copy_file File.join(source_root, "yajl.swc"), File.join(destination_root,"yajl.swc")
    end

    desc :objects, "Compiles all yajl objects"
    def objects
      source_paths.each do |source_root|
        inside(source_root, :verbose => true) do
          files = sources_in(".")
          puts "[#{self.class.to_s}] Options: #{options.to_yaml}"
          invoke "alchemy:tool_chain:bc", [files,"o"], {:destination_root => options[:destination_root]}
        end
      end
    end

    desc :swc, "Compiles the SWC for Yajl Extension"
    def swc 
      inside(dir, :verbose => true) do
        invoke "toolchain:swc", ["gcc"]
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
    include Utils
    argument :name
    class_option :debug, :default => true
    class << self
      def source_root 
        File.dirname(__FILE__)
      end
    end


  end
end

# vim:filetype=ruby
