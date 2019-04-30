{
  "targets": [
    {
      "target_name": "addon",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions", "-lstdc++fs" ],
      "sources": [ "fs.cc" ],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      # "include_dirs": [
      #   "/node_modules/node-addon-api",
      #   # "<!@(node -p \"require('node-addon-api').include\")"
      # ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}
