#!/bin/zsh

os=$(uname)

function darwin_env()
{
	export VK_ICD_FILENAMES=$PWD/ThirdParty/MoltenVK/macOS/MoltenVK_icd.json
	export VK_LAYER_PATH=$PWD/ThirdParty/vulkan/Validation_Layer/explicit_layer.d/macos/
}

function linux_env()
{
	export VK_LAYER_PATH=$PWD/ThirdParty/vulkan/Validation_Layer/explicit_layer.d/linux/
}

case $os in
	"Darwin")
		darwin_env ;;
	"Linux")
		linux_env ;;
	*)
		"We don't want windows !";
esac
