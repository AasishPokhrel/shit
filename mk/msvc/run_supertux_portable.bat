@echo off
pushd "%~dp0"
set "SUPERTUX2_USER_DIR=%~dp0\user\"
start "" "bin\supertux2.exe" %*
popd
