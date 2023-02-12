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
  
  else
    if test $1="-h" 
    then
      echo "commit.sh: Meteme el mensaje para el commit entre comillas dobles \"\" "
    fi
  fi
fi

#echo $1
git add -A
git commit -m "$1"
git push -u origin main
#usuario github:	vendul0g
#token github: 		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
