.PHONY: format
format:
	git ls-files '*.cpp' '*.h' | xargs clang-format -i