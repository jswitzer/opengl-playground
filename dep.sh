glew_version="2.1.0"
glew_tarfile="depsrc/glew-$glew_version.tar.gz"
glew_unpacks_as="glew-$glew_version"
glew_patch_list="depsrc/glew-$glew_version.patch.list"
glew_patched_files="depsrc/glew-$glew_version.patch.files"
glew_move_to="glew"

glfw_version="3.2.1"
glfw_tarfile="depsrc/glfw-$glfw_version.tar.gz"
glfw_unpacks_as="glfw-$glfw_version"
glfw_patch_list="depsrc/glfw-$glfw_version.patch.list"
glfw_patched_files="depsrc/glfw-$glfw_version.patch.files"
glfw_move_to="glfw"

lua_version="5.3.6"
lua_tarfile="depsrc/lua-$lua_version.tar.gz"
lua_unpacks_as="lua-$lua_version"
lua_patch_list="depsrc/lua-$lua_version.patch.list"
lua_patched_files="depsrc/lua-$lua_version.patch.files"
lua_move_to="lua"

readline_version="8.1"
readline_tarfile="depsrc/readline-$readline_version.tar.gz"
readline_unpacks_as="readline-$readline_version"
readline_patch_list="depsrc/readline-$readline_version.patch.list"
readline_patched_files="depsrc/readline-$readline_version.patch.files"
readline_move_to="readline"

soil_version="0.1.1"
soil_tarfile="depsrc/soil-$soil_version.tar.gz"
soil_unpacks_as="soil-$soil_version"
soil_patch_list="depsrc/soil-$soil_version.patch.list"
soil_patched_files="depsrc/soil-$soil_version.patch.files"
soil_move_to="soil"

tcc_version="0.9.27"
tcc_tarfile="depsrc/tcc-$tcc_version.tar.gz"
tcc_unpacks_as="tcc-$tcc_version"
tcc_patch_list="depsrc/tcc-$tcc_version.patch.list"
tcc_patched_files="depsrc/tcc-$tcc_version.patch.files"
tcc_move_to="tcc"

all=( glew glfw lua readline soil tcc )

getinfo() { 
  local array=$1 index=$2
  local i="${array}_$index"
  printf '%s' "${!i}"
}

function do_diff() {
  mod=$1
  store=$2
  tarfile=$(getinfo $mod "tarfile")
  unpacks_as=$(getinfo $mod "unpacks_as")
  move_to=$(getinfo $mod "move_to")
  patch_list=$(getinfo $mod "patch_list")
  patched_files=$(getinfo $mod "patched_files")
  if [ ! -d $unpacks_as ]; then
    tar -xzf $tarfile
  fi
  if [ "$store" = "store" ]; then
    echo "writing output to $patch_list"
    git diff --no-index --name-only $unpacks_as $move_to > $patch_list
    while read p; do
      echo "storing changed file $p"
      mkdir -p $patched_files/$(dirname $p)
      cp -f $p $patched_files/$(dirname $p)
    done <$patch_list
  else 
    git diff --no-index --name-only $unpacks_as $move_to 
  fi
}

function do_clean() {
  mod=$1
  unpacks_as=$(getinfo $mod "unpacks_as")
  move_to=$(getinfo $mod "move_to")
  if [ -d $move_to ]; then
    rm -rf $move_to
  fi
  if [ -d $unpacks_as ]; then
    rm -rf $unpacks_as
  fi

}

function do_unpack() {
  mod=$1
  move_to=$(getinfo $mod "move_to")
  unpacks_as=$(getinfo $mod "unpacks_as")
  tarfile=$(getinfo $mod "tarfile")
  version=$(getinfo $mod "version")
  patch_list=$(getinfo $mod "patch_list")
  patched_files=$(getinfo $mod "patched_files")
  echo "Unpacking $mod version $version .. " 
  if [ ! -d $move_to ]; then
    if [ -d $unpacks_as ]; then
      rm -r $unpacks_as
    fi
    echo "Using Tar"
    tar -xzf $tarfile
    mv $unpacks_as $move_to
    if [ -e $patch_list ]; then
      while read p; do
        echo "$patching $p"
        mkdir -p $(dirname $p)
        cp -f $patched_files/$p ./$p
      done <$patch_list
    fi
  else 
    echo "Already exists"
  fi
}

function do_unpack_all() {
  for m in "${all[@]}"; do
    do_unpack $m
  done
}

function do_clean_all() {
  for m in "${all[@]}"; do
    do_clean $m
  done
}
command=$1
mod=$2
if [ "$command" = "diff" ]; then
  do_diff $mod $3
elif [ "$command" = "clean" ]; then
  if [ "$mod" = "all" ]; then
    do_clean_all
  else
    do_clean $mod
  fi
elif [ "$command" = "unpack" ]; then
  if [ "$mod" = "all" ]; then
    do_unpack_all
  else
    do_unpack $mod
  fi
else 
  echo "Usage:     dep unpack <module_name>"
  echo "Alt Usage: dep diff <module_name> print"
  echo "Alt Usage: dep diff <module_name> store"
  echo "Alt Usage: dep clean <module_name>"
  echo "Alt Usage: dep clean all"
fi
