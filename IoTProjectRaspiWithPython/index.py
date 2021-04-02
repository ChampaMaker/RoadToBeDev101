from flask import Flask, render_template
import pyrebase

firebaseConfig = {
    "apiKey": "AIzaSyACjvHy9WX_Cx0NC8B24I1UIiGBC8ifyNA",
    "authDomain": "smart-farm-3ef1a.firebaseapp.com",
    "databaseURL": "https://smart-farm-3ef1a.firebaseio.com",
    "projectId": "smart-farm-3ef1a",
    "storageBucket": "smart-farm-3ef1a.appspot.com",
    "messagingSenderId": "849173162919",
    "appId": "1:849173162919:web:542600f2780ef5ed93e482",
    "measurementId": "G-XQ2P0TX564",
}
firebase = pyrebase.initialize_app(firebaseConfig)
db = firebase.database()

app = Flask(__name__)


@app.route("/")
def index():
    return render_template("index.html", led=0)


@app.route("/led/<led>")
def control_led(led):
    data = {"status_led": int(led)}
    db.child("my_led").update(data)
    return render_template("index.html", led=led)


if __name__ == "__main__":
    app.run(port=5011)
