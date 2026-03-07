#!/bin/sh

[ ! -f "libxmp_fuzz_asan" ] &&  { echo "Couldn't find libxmp_fuzz_asan. Run 'make fuzzers'!"; exit 1; }

#export LD_LIBRARY_PATH=".fuzzer"
mkdir -p "ARTIFACTS"

DEFAULT_PARAMETERS="CORPUS data/ openmpt/ -artifact_prefix=ARTIFACTS/ -timeout=30"

COMMAND="$1"
shift

case "$COMMAND" in
	#
	# Merge corpus.
	#
	merge)
		NEW_CORPUS="NEW_CORPUS"
		OLD_CORPUS="CORPUS"
		[ -n "$1" ] && { NEW_CORPUS="$1"; }
		[ -n "$2" ] && { OLD_CORPUS="$2"; }
		mkdir -p "$NEW_CORPUS"
<<<<<<< HEAD
		./libxmp_fuzz_asan -merge=1 "$NEW_CORPUS" "$OLD_CORPUS"
		./libxmp_fuzz_msan -merge=1 "$NEW_CORPUS" "$OLD_CORPUS"
		./libxmp_fuzz_ubsan -merge=1 "$NEW_CORPUS" "$OLD_CORPUS"
=======
		./libxmp_fuzz_asan -merge=1 -rss_limit_mb=4096 -timeout=2 "$NEW_CORPUS" "$OLD_CORPUS"
		./libxmp_fuzz_msan -merge=1 -rss_limit_mb=4096 -timeout=2 "$NEW_CORPUS" "$OLD_CORPUS"
>>>>>>> db7344ebf (abc)
		;;

	#
	# Fuzz with given parameters.
	#
	asanx)
		./libxmp_fuzz_asan -artifact_prefix="ARTIFACTS/" "$@"
		;;
<<<<<<< HEAD
	msanx)
		./libxmp_fuzz_msan -artifact_prefix="ARTIFACTS/" "$@"
		;;
	ubsanx)
		./libxmp_fuzz_ubsan -artifact_prefix="ARTIFACTS/" "$@"
		;;
=======
	hwasanx)
		./libxmp_fuzz_hwasan -artifact_prefix="ARTIFACTS/" "$@"
		;;
	msanx)
		./libxmp_fuzz_msan -artifact_prefix="ARTIFACTS/" "$@"
		;;
>>>>>>> db7344ebf (abc)

	#
	# Fuzz with default parameters.
	#
	asan)
		mkdir -p "CORPUS"
		./libxmp_fuzz_asan $DEFAULT_PARAMETERS "$@"
		;;
<<<<<<< HEAD
=======
	hwasan)
		mkdir -p "CORPUS"
		./libxmp_fuzz_hwasan $DEFAULT_PARAMETERS "$@"
		;;
>>>>>>> db7344ebf (abc)
	msan)
		mkdir -p "CORPUS"
		./libxmp_fuzz_msan $DEFAULT_PARAMETERS "$@"
		;;
<<<<<<< HEAD
	ubsan)
		mkdir -p "CORPUS"
		./libxmp_fuzz_ubsan $DEFAULT_PARAMETERS "$@"
		;;
=======
>>>>>>> db7344ebf (abc)
esac
