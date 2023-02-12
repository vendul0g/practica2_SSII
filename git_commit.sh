#!/bin/bash -e
if test $# -eq 0
then
  echo "commit.sh: Meteme el mensaje para el comit entre comillas dobles \"\" "
  exit
else
  if test $# -gt 1
  then
    echo "demasiados argumentos"
    exit
  fi
fi

#echo $1
git add -A
git commit -m "$1"

expect - <<_END_EXPECT
  spawn git push -u origin main
  expect 'User*'
  send "vendul0g\r"
  expect 'Pass*'
  send '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\r'
  set timeout -1 ; #no timeout
  expect eof
_END_EXPECT

#usuario github:	vendul0g
#token github: 		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

