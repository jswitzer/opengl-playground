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

$deps = @{
  glew = @{
    version = '2.1.0'
    tarfile = 'depsrc/glew-2.1.0.tgz'
    unpacks_as = 'glew-2.1.0'
    patch_list = 'depsrc/glew-2.1.0.patch.list'
    patched_files = 'depsrc/glew-2.1.0.patch.files'
    move_to = 'glew'
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
    git diff --no-index --name-only $unpacks_as $move_to *> $patch_list
    foreach($file in Get-Content $patch_list) {
      write "storing changed file $file"
      New-Item -Force -Path $patched_files/$file
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
    rm -r $move_to
  }
  $t = Test-Path -Path $unpacks_as
  if ($t) {
    rm -r $unpacks_as
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
      rm -r $unpacks_as
    }
    write "Using Tar"
    tar -xzf $tarfile
    mv $unpacks_as $move_to
    $t = Test-Path -Path $patch_list

    if ( $t) {
      foreach($file in Get-Content $patch_list) {
        write "patching $file"
        New-Item -Force -Path ./$file
        Copy-Item -Force $patched_files/$file ./$file
      }
    }
  } else {
    write "Already exists"
  }
}

$command=$args[0]
$mod=$args[1]

if ($command -eq "diff" ) {
  $store = $false
  if ($args[2] -eq 'store') {
    $store = $true
  }
  do_diff $mod $store
} elseif ($command -eq "clean") {
  clean $mod
} elseif ($command -eq "unpack") {
  unpack $mod
} else {
  write "Usage:     dep unpack <module_name>"
  write "Alt Usage: dep diff <module_name> print"
  write "Alt Usage: dep diff <module_name> store"
  write "Alt Usage: dep clean <module_name>"
  write "Alt Usage: dep clean all"
}


