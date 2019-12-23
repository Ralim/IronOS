#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SECRETS="$DIR/encrypted/*.encrypted"
KEY="$DIR/../../codeship.aes"

mkdir -p $DIR/unencrypted

for f in $SECRETS
do
    out="${f/.encrypted/.secret}"
    out="${out/encrypted/unencrypted}"
    echo $out
    jet decrypt $f $out --key-path $KEY
done

echo "Done"
