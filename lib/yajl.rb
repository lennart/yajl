
# Ensure this FILE NAME is the name you want for your library
# This is the primary criteria by which your library will be
# found by users of rubygems and sprouts
name = File.basename(__FILE__).split('.').shift

gem_wrap name do |t|
  # version is a dot-delimited, 3 digit version string
  t.version       = '0.0.1'
  # Short summary of your library or project
  t.summary       = "yajl JSON parser precompiled as SWC"
  # Your name
  t.author        = 'Lennart Melzer'
  # Your email or - better yet - the address of your project email list
  t.email         = 'l.melzer@tu-bs.de'
  # The homepage of your library
  t.homepage      = 'http://github.com/lennart/yajl/tree/alchemy'
  t.sprout_spec   =<<EOF
- !ruby/object:Sprout::RemoteFileTarget 
  platform: universal
  url: http://github.com/downloads/lennart/yajl/as3yajl-0.0.1.zip
  archive_path: as3yajl 
EOF
end

task :package => name
