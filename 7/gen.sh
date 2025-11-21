filename="writeup"
pandoc --toc -s $filename.md -o tmp.html
pandoc tmp.html -s -o html/$filename.html --template ../template.html -F ../highlight_script.py
#pandoc tmp.html -s -o html/$filename.html --template ../template.html
rm tmp.html
