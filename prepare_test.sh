FOLDER="cpp-backend-tests-practicum"
REPO=${PWD}
cd ${REPO} || exit 1
if [ ! -d "$FOLDER" ] ; then
  git clone git@github.com:cpppracticum/"$FOLDER".git "$FOLDER"
else
  (
  cd "$FOLDER" || exit 1
  git pull
  )
fi


if [ ! -d "${REPO}/.venv" ] ; then
  python3 -m venv ${REPO}/.venv
fi
  source ${REPO}/.venv/bin/activate
  python3 -m pip install --upgrade pip
  python3 -m pip install -r requirements.txt
