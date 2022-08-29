# pretty text awaits
RED=`tput setaf 1`
GRN=`tput setaf 2`
YEL=`tput setaf 3`
MAG=`tput setaf 5`
CYN=`tput setaf 6`
RST=`tput sgr0`

COMPILER="g++"
STDOPTS="-I. -std=c++11 -O3"
ALL_OBJS=""

SRC_FOLDERS=(    \
    x86_emit     \
    x86_emit/xmm \
    abi-map      \
)

function gen_build_for {

    local cur_dir=${1}

    mkdir -p obj/$cur_dir

    files_in_dir=( `ls $cur_dir | grep ".cpp$"` )
    for fname in "${files_in_dir[@]}"; do

        local base_name="${fname%%.*}"
        local obj_file="obj/$cur_dir/$base_name.o"
        local src_file="$cur_dir/$fname"
        local header_file="$cur_dir/$base_name.h"


        echo "${CYN}Generating build cmd for ${YEL}$cur_dir/$fname ${RST}"
        
        ALL_OBJS="$obj_file $ALL_OBJS"

        echo "$obj_file: $src_file $header_file" >> Makefile
        printf "\t$COMPILER -c $STDOPTS -o $obj_file $src_file\n" >> Makefile
        echo "" >> Makefile

    done

}

echo "" > Makefile
echo "# this file was automatically generated" >> Makefile
echo "" >> Makefile

echo "all: main" >> Makefile
echo "" >> Makefile

for dirname in "${SRC_FOLDERS[@]}"; do gen_build_for $dirname; done

echo "main: main.cpp $ALL_OBJS" >> Makefile
printf "\t$COMPILER $STDOPTS -o main main.cpp $ALL_OBJS\n" >> Makefile
echo "" >> Makefile

echo "clean:" >> Makefile
printf "\trm $ALL_OBJS\n\n" >> Makefile

echo "run:" >> Makefile
printf "\t./main\n" >> Makefile
printf "\tobjdump -D -b binary -mi386:x86-64 -M intel output.txt\n" >> Makefile

