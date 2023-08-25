LIBRARY_PATH=$(python -c "import sysconfig; import os;print(os.path.split(os.path.split(sysconfig.get_path('platlib'))[0])[0])")
export LD_LIBRARY_PATH=${LIBRARY_PATH}:$LD_LIBRARY_PATH

./bin/rollerball --port 8181 &
./bin/rollerball --port 8182 &

echo "Rollerball servers are running on ports 8181 and 8182"