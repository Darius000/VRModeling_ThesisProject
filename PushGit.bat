CALL git status
CALL git add *
CALL git status
set/p message=
CALL git commit -m "%message%"
CALL git push origin master
PAUSE