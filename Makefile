all:
	echo "yaadv"

format:
	clang-format -i `find Classes/ -type f`
