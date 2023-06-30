#!/usr/bin/env bash

# little helper for docker deployment to:
# - start development environment for IronOS ("shell" sub-command)
# - generate full set of builds ("build" sub-command)

#set -x
#set -e

usage()
{
	echo -e "\nUsage: ${0} [CMD]\n"
	echo "CMD:"
	echo -e "\tshell - start docker container with shell inside to work on IronOS with all tools needed"
	echo -e "\tbuild - compile builds of IronOS inside docker container for supported hardware"
	echo -e "\tclean - delete created docker container (but not pre-downloaded data for it)\n"
	echo -e "STORAGE NOTICE: for \"shell\" and \"build\" commands extra files will be downloaded so make sure that you have ~5GB of free space.\n"
}

### main

docker_conf="IronOS.yml"

# allow providing custom path to docker tool using DOCKER_BIN external env. var.
# (compose sub-command must be included, i.e. DOCKER_BIN="/usr/local/bin/docker compose" ./deploy.sh)

if [ -z "${DOCKER_BIN}" ]; then
	docker_bin=""
else
	docker_bin="${DOCKER_BIN}"
fi;

# detect availability of docker

docker_compose="`command -v docker-compose`"
if [ "${?}" -eq 0 ] && [ -n "${docker_compose}" ] && [ -z "${docker_bin}" ]; then
	docker_bin="${docker_compose}"
fi;

docker_tool="`command -v docker`"
if [ "${?}" -eq 0 ] && [ -n "${docker_tool}" ] && [ -z "${docker_bin}" ]; then
	docker_bin="${docker_tool}  compose"
fi;

if [ -z "${docker_bin}" ]; then
	echo "ERROR: Can't find docker-compose nor docker tool. Please, install docker and try again."
	exit 1
fi;

# construct command to run

cmd="${1}"
if [ -z "${cmd}" ] || [ "${cmd}" = "shell" ]; then
	docker_cmd="run  --rm  builder"
elif [ "${cmd}" = "build" ]; then
	docker_cmd="run  --rm  builder  /bin/bash /build/ci/buildAll.sh"
elif [ "${cmd}" = "clean" ]; then
	docker  rmi  ironos-builder:latest
	exit "${?}"
else
	usage
	exit 1
fi;

# get absolute location of project root dir to make docker happy with config(s)
# (successfully tested on relatively POSIX-compliant Dash shell)

# this script
script_file="/deploy.sh"
# IronOS/scripts/deploy.sh
script_path="${PWD}"/"${0}"
# IronOS/scripts/
script_dir=${script_path%"${script_file}"}
# IronOS/
root_dir="${script_dir}/.."
# IronOS/IronOS.yml
docker_file="-f ${root_dir}/${docker_conf}"

# change dir to project root dir & run constructed command

cd "${root_dir}"
echo -e "\n====>>>> Firing up & starting container..."
if [ "${cmd}" = "shell" ]; then
echo -e "\t* type \"exit\" to end the session when done;"
fi;
echo -e "\t* type \"${0} clean\" to delete created container (but not cached data)"
echo -e "\n====>>>> ${docker_bin}  ${docker_file}  ${docker_cmd}\n"
${docker_bin}  ${docker_file}  ${docker_cmd}
exit "${?}"

