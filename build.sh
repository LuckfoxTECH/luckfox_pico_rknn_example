#!/bin/bash

ROOT_PWD=$(cd "$(dirname $0)" && cd -P "$(dirname "$SOURCE")" && pwd)

if [ "$1" = "clean" ]; then
	if [ -d "${ROOT_PWD}/build" ]; then
		rm -rf "${ROOT_PWD}/build"
		echo " ${ROOT_PWD}/build has been deleted!"
	fi

	if [ -d "${ROOT_PWD}/install" ]; then
		rm -rf "${ROOT_PWD}/install"
		echo " ${ROOT_PWD}/install has been deleted!"
	fi

	exit
fi

options=("luckfox_pico_retinaface_facenet"
	"luckfox_pico_retinaface_facenet_spidev"
	"luckfox_pico_yolov5")

options_device=("LUCKFOX_PICO_PLUS"
	"LUCKFOX_PICO_PRO_MAX")

PS3="Enter your choice [1-${#options[@]}]: "

select opt in "${options[@]}"; do
	if [[ -n "$opt" ]]; then
		echo "You selected: $opt"
		echo "你选择了: $opt"

		if [ "$opt" == "luckfox_pico_retinaface_facenet_spidev" ]; then
			PS3="Enter your choice [1-${#options_device[@]}]: "
			select dev_opt in "${options_device[@]}"; do
				if [[ -n "$opt" ]]; then
					echo "You selected: $dev_opt"
					echo "你选择了: $dev_opt"

					DEVICE=$dev_opt
					break
				fi
			done
		fi

		src_dir="example/$opt"
		if [[ -d "$src_dir" ]]; then
			if [ -d ${ROOT_PWD}/build ]; then
				rm -rf ${ROOT_PWD}/build
			fi
			mkdir ${ROOT_PWD}/build
			cd ${ROOT_PWD}/build
			if [ -z "$DEVICE" ]; then
				cmake .. -DEXAMPLE_DIR="$src_dir" -DEXAMPLE_NAME="$opt"
			else
				cmake .. -DEXAMPLE_DIR="$src_dir" -DEXAMPLE_NAME="$opt" -D"$DEVICE"=ON
			fi
			make -j install
		else
			echo "错误：目录 $src_dir 不存在！"
			echo "Error: Directory $src_dir does not exist!"
		fi
		break
	else
		echo "Invalid selection, please try again."
		echo "无效的选择，请重新选择。"
	fi
done
