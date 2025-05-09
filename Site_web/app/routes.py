from flask import render_template
from app import app

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/visualisation")
def visualisation():
    salles = [
        {"nom": "Infolab 1", "nb": 5},
        {"nom": "Infolab 2", "nb": 3},
        {"nom": "Bibliothèque", "nb": 7}
    ]
    return render_template("visualisation.html", salles=salles)


@app.route("/infolab2")
def infolab2():
    return render_template("infolab2.html")

@app.route("/bibliotheque")
def bibliotheque():
    return render_template("bibliotheque.html")

@app.route("/etage<int:numero>")
def etage(numero):
    return render_template(f"etage{numero}.html")
@app.route("/infolab1")
def infolab1():
    return render_template("infolab1.html", salle="Infolab 1", people=5)
