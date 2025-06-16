#!/usr/bin/env bash

shopt -s nullglob

for file in upload/SuperTux*; do
    file_base=$(basename $file)
    echo "Uploading $file_base";
    export URL="${PREFIX}/$file_base"
    export SIZE=$(($(wc -c < "$file")))
    if [ $IS_WINDOWS = true ] ; then
        export SHASUM=$(powershell -command "Get-FileHash \"$file\" -Algorithm SHA256 | Select-Object -ExpandProperty Hash")
    else
        export SHASUM=$(shasum -a 256 "$file" | cut -d " " -f 1)
    fi
    echo "Checksum: $SHASUM";
    echo "Branch: $BRANCH_NAME";
    jq -n '{"url": ("/" + env["URL"]), "shasum": env["SHASUM"], "size": env["SIZE"] | tonumber, "branch": env["BRANCH_NAME"] }' | \
        curl -X POST \
            -H "x-api-key: $DOWNLOAD_APIKEY" \
            -H "Content-Type: application/json" \
            --data-binary @- \
            -s https://download.supertux.org/api/download
done
