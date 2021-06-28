glew_version="2.1.0"
glew_tarfile="depsrc/glew-2.1.0.tgz"
glew_unpacks_as="glew-2.1.0"
glew_patch_list="depsrc/glew-2.1.0.patch.list"
glew_patched_files="depsrc/glew-2.1.0.patch.files"
glew_move_to="glew"

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
    rm -r $move_to
  fi
  if [ -d $unpacks_as ]; then
    rm -r $unpacks_as
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
command=$1
mod=$2
if [ "$command" = "diff" ]; then
  do_diff $mod $3
elif [ "$command" = "clean" ]; then
  do_clean $mod
elif [ "$command" = "unpack" ]; then
  do_unpack $mod
else 
  echo "Usage:     dep unpack <module_name>"
  echo "Alt Usage: dep diff <module_name> print"
  echo "Alt Usage: dep diff <module_name> store"
  echo "Alt Usage: dep clean <module_name>"
  echo "Alt Usage: dep clean all"
fi
