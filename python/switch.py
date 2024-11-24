def foo():
    print("funktion foo")

def bar():
    print("funktion bar")

callable = [foo, bar]

modul = input("Funktion: ")
for i in callable:
    print(i.__name__)
    if i.__name__ == modul:
        i()
        break
