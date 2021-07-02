# Usage: 
# dep clean <name>
# Removes dependency folder
# Removes the 'orig' folder if one exists for diff-creating reasons
#
# dep unpack <name>
# Unpack dependency and apply patch (if one exists)
# Does nothing if the dependency is already unpacked
#
# dep diff <name>
# Looks at the currently unpacked directory and creates a diff 

$glew_version = '2.1.0'
$glfw_version='3.2.1'
$lua_version='5.3.6'
$soil_version='0.1.1'
$tcc_version='0.9.27'
$wineditline_version='2.206'

$all = @('glew', 'glfw', 'lua', 'soil', 'tcc', 'wineditline')

$deps = @{
  glew = @{
    version = "$glew_version"
    tarfile = "depsrc/glew-$glew_version.tar.gz"
    unpacks_as = "glew-$glew_version"
    patch_list = "depsrc/glew-$glew_version.patch.list"
    patched_files = "depsrc/glew-$glew_version.patch.files"
    move_to = "glew"
  }
  glfw = @{
    version = "$glfw_version"
    tarfile = "depsrc/glfw-$glfw_version.tar.gz"
    unpacks_as = "glfw-$glfw_version"
    patch_list = "depsrc/glfw-$glfw_version.patch.list"
    patched_files = "depsrc/glfw-$glfw_version.patch.files"
    move_to = "glfw"
  }
  lua = @{
    version = "$lua_version"
    tarfile = "depsrc/lua-$lua_version.tar.gz"
    unpacks_as = "lua-$lua_version"
    patch_list = "depsrc/lua-$lua_version.patch.list"
    patched_files = "depsrc/lua-$lua_version.patch.files"
    move_to = "lua"
  }
  soil = @{
    version = "$soil_version"
    tarfile = "depsrc/soil-$soil_version.tar.gz"
    unpacks_as = "soil-$soil_version"
    patch_list = "depsrc/soil-$soil_version.patch.list"
    patched_files = "depsrc/soil-$soil_version.patch.files"
    move_to = "soil"
  }
  tcc = @{
    version = "$tcc_version"
    tarfile = "depsrc/tcc-$tcc_version.tar.gz"
    unpacks_as = "tcc-$tcc_version"
    patch_list = "depsrc/tcc-$tcc_version.patch.list"
    patched_files = "depsrc/tcc-$tcc_version.patch.files"
    move_to = "tcc"
  }
  wineditline = @{
    version = "$wineditline_version"
    tarfile = "depsrc/wineditline-$wineditline_version.tar.gz"
    unpacks_as = "wineditline-$wineditline_version"
    patch_list = "depsrc/wineditline-$wineditline_version.patch.list"
    patched_files = "depsrc/wineditline-$wineditline_version.patch.files"
    move_to = "wineditline"
  }
}


function do_diff($mod, $store) {
  write "diffing $mod, store: $store"
  $tarfile = $deps[$mod].tarfile
  $move_to = $deps[$mod].move_to
  $unpacks_as = $deps[$mod].unpacks_as
  $patch_list = $deps[$mod].patch_list
  $patched_files = $deps[$mod].patched_files
  $t = Test-Path -Path $unpacks_as
  if ( -not $t) {
    tar -xzf $tarfile
  }

  if ( $store -eq "True" ) {
    write "Writing output to $patch_list"
    git diff --no-index --name-only $unpacks_as $move_to > $patch_list
    #git diff --no-index --name-only $unpacks_as $move_to *> $patch_list
    foreach($file in Get-Content $patch_list) {
      write "storing changed file $file"
      New-Item -Force -Path $patched_files/$file -ItemType "file"
      Copy-Item -Force $file $patched_files/$file
    }
  } else {
    git diff --no-index --name-only $unpacks_as $move_to
  }
}

function clean($mod) {
  $move_to = $deps[$mod].move_to
  $unpacks_as = $deps[$mod].unpacks_as
  $t = Test-Path -Path $move_to
  if ($t) {
    rm -r -force $move_to
  }
  $t = Test-Path -Path $unpacks_as
  if ($t) {
    rm -r -force $unpacks_as
  }
}

function unpack($mod) {
  $move_to = $deps[$mod].move_to
  $unpacks_as = $deps[$mod].unpacks_as
  $tarfile = $deps[$mod].tarfile
  $version = $deps[$mod].version
  $patch_list = $deps[$mod].patch_list
  $patched_files = $deps[$mod].patched_files
  write "Unpacking $mod version $version .. "
  $t = Test-Path -Path $move_to
  if ( -not $t) {
    $t = Test-Path -Path $unpacks_as
    if ($t) {
      rm -r -force $unpacks_as
    }
    write "Using Tar"
    tar -xzf $tarfile
    try {
    mv $unpacks_as $move_to
    } catch {
      write "Error, waiting then retrying"
      Start-Sleep -Seconds 2
      mv $unpacks_as $move_to
    }
    $t = Test-Path -Path $patch_list

    if ( $t) {
      foreach($file in Get-Content $patch_list) {
        write "patching $file"
        New-Item -Force -Path ./$file -ItemType "file"
        Copy-Item -Force $patched_files/$file ./$file
      }
    }
  } else {
    write "Already exists"
  }
}

function unpack_all() {
  foreach($mod in $all) {
    unpack $mod
  }
}

function clean_all() {
  foreach($mod in $all) {
    clean $mod
  }
}


$command=$args[0]
$mod=$args[1]


function Invoke-Environment {
    param
    (
        # Any cmd shell command, normally a configuration batch file.
        [Parameter(Mandatory=$true)]
        [string] $Command
    )

    $Command = "`"" + $Command + "`""
    cmd /c "$Command > nul 2>&1 && set" | . { process {
        if ($_ -match '^([^=]+)=(.*)') {
            [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2])
        }
    }}

}

function build_glfw() {
  cd glfw
  cmake .
  msbuild ./src/glfw.vcxproj /property:Configuration=Release
  cd ..
  $source_lib = "./glfw/src/Release/glfw3.lib"
  $target_lib = "./lib/glfw3.lib"
  New-Item -Force -Path $target_lib -ItemType "file"
  Copy-Item -Force $source_lib $target_lib
}

function build_glew() {
  cd glew
  cmake build/cmake 
  msbuild ./glew_s.vcxproj /property:Configuration=Release
  cd ..
  $source_lib = "./glew/lib/Release/libglew32.lib"
  $target_lib = "./lib/libglew32.lib"
  New-Item -Force -Path $target_lib -ItemType "file"
  Copy-Item -Force $source_lib $target_lib
}

function build_soil() {
  cd soil
  cmake .
  msbuild ./soil_s.vcxproj /property:Configuration=Release
  cd ..
  $source_lib = "./soil/Release/libSOIL.lib"
  $target_lib = "./lib/libSOIL.lib"
  New-Item -Force -Path $target_lib -ItemType "file"
  Copy-Item -Force $source_lib $target_lib

}

function build_tcc() {
  cd tcc/win32
  .\build-tcc.bat -c cl -t 32
  cd ../..
  $source_lib = "./tcc/win32/libtcc.lib"
  $target_lib = "./lib/libtcc.lib"
  New-Item -Force -Path $target_lib -ItemType "file"
  Copy-Item -Force $source_lib $target_lib
  cp -Force "./tcc/win32/libtcc.dll" "./lib/libtcc.dll"
  New-Item -Force -Path "./tcc/child_lib" -ItemType "directory"
  New-Item -Force -Path "./tcc/child_include" -ItemType "directory"
  New-Item -Force -Path "./tcc/parent_include" -ItemType "directory"
  cp "./tcc/win32/lib/libtcc1-32.a" "./tcc/child_lib/"
  cp "./tcc/include/*.h" "./tcc/child_include/"
  cp "./tcc/tcclib.h" "./tcc/child_include/"
  cp "./tcc/libtcc.h" "./tcc/parent_include/"

}

function build_wineditline() {
  cd wineditline
  cmake .
  msbuild ./src/edit_static.vcxproj /property:Configuration=Release
  cd ..
  $source_lib = "./wineditline/src/Release/edit_static.lib"
  $target_lib = "./lib/edit_static.lib"
  New-Item -Force -Path $target_lib -ItemType "file"
  Copy-Item -Force $source_lib $target_lib
}

function build_lua() {
  cd lua/src
  cmake .
  msbuild ./liblua.vcxproj /property:Configuration=Release
  cd ../..
  $source_lib = "./lua/src/Release/liblua.lib"
  $target_lib = "./lib/liblua.lib"
  New-Item -Force -Path $target_lib -ItemType "file"
  Copy-Item -Force $source_lib $target_lib
  New-Item -Force -Path "./lua/include" -ItemType "directory"

  $headers = @('lualib.h', 'luaconf.h', 'lua.h', 'lauxlib.h')
  foreach ($header in $headers) {
    Copy-Item -Force "./lua/src/$header" "./lua/include/$header"
  }
}

function do_build($mod) {
  unpack $mod
  if ("$mod" -eq "glfw") {
    build_glfw
  }
  if ("$mod" -eq "glew") {
    build_glew
  }
  if ("$mod" -eq "soil") {
    build_soil
  }
  if ("$mod" -eq "wineditline") {
    build_wineditline
  }
  if ("$mod" -eq "tcc") {
    build_tcc
  }
  if ("$mod" -eq "lua") {
    build_lua
  }
}

function build_all() {
  foreach($mod in $all) {
    do_build $mod
  }
}

function vsvars32() {
  Invoke-Environment $env:VS110COMNTOOLS\vsvars32.bat
}



if ($command -eq "diff" ) {
  $store = $false
  if ($args[2] -eq 'store') {
    $store = $true
  }
  do_diff $mod $store
} elseif ($command -eq "clean") {
  if ($mod -eq "all") {
    clean_all
  } else {
    clean $mod
  }
} elseif ($command -eq "build") {
  vsvars32
  if ($mod -eq "all") {
    build_all
  } else {
    do_build $mod
  }

} elseif ($command -eq "unpack") {
  if ($mod -eq "all") {
    unpack_all
  } else {
    unpack $mod
  }
} elseif ($command -eq "vsvars32") {
  vsvars32
} else {
  write "Usage:     dep unpack <module_name>"
  write "Alt Usage: dep diff <module_name> print"
  write "Alt Usage: dep diff <module_name> store"
  write "Alt Usage: dep clean <module_name>"
  write "Alt Usage: dep clean all"
  write "Alt Usage: dep vsvars32"
}

