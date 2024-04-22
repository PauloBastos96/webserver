import pyjokes

def generate_joke():
    joke = pyjokes.get_joke()
    print("<html>")
    print("<body>")
    print("<h1>Here's a joke for you:</h1>")
    print("<p>{}</p>".format(joke))
    print("</body>")
    print("</html>")

generate_joke()